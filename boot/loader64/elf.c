//
// Created by bear on 9/7/2023.
//

//Relocation becomes handy when you need to load, for example, modules or drivers.
// It's possible to use the "-r" option to ld to permit you to have multiple object files linked into one big one,
// which means easier coding and faster testing.
//
//The basic outline of things you need to do for relocation:
//
//Check the object file header (it has to be ELF, not PE, for example)
//Get a load address (e.g. all drivers start at 0xA0000000, need some method of keeping track of driver locations)
//Allocate enough space for all program sections (ST_PROGBITS)
//Copy from the image in RAM to the allocated space
//Go through all sections resolving external references against the kernel symbol table
//If all succeeded, you can use the "e_entry" field of the header as the offset from the load address to call the entry point (if one was specified),
// or do a symbol lookup, or just return a success error code.
//Once you can relocate ELF objects you'll be able to have drivers loaded when needed instead of at startup
// - which is always a Good Thing (tm).
// See:
// https://wiki.osdev.org/ELF#Relocation

#include "defs.h"
#include "ext.h"
#include "elf.h"
#include "error.h"
#include "defs.h"
#include "mem.h"
#include "terminal.h"
#include "utils.h"

static inline ERRORCODE ALWAYS_INLINE CheckElfCommon(const ELFHEADER64* header)
{
	if (header->Magic != ELF_MAGIC)
	{
		return -E_INVALID;
	}
	if (header->Bit != ELF_BIT_64) // 64-bit
	{
		return -E_INVALID;
	}
	if (header->Endian != 1) // little endian
	{
		return -E_INVALID;
	}
	if (header->Machine != ELFMACHINE_X8664)
	{
		return -E_INVALID;
	}
	return E_SUCCESS;
}

static inline BOOL ALWAYS_INLINE ShouldLoadModuleSection(const ELFSECTIONHEADER64* secHeader)
{
	if (secHeader->Type != SHT_PROGBITS && secHeader->Type != SHT_NOBITS)
	{
		return FALSE;
	}

	if ((secHeader->Flags & SHF_ALLOC) == 0)
	{
		return FALSE;
	}

	if (secHeader->Size == 0)
	{
		return FALSE;
	}

	if (secHeader->VirtualAddress < MODULE_LINK_ADDR)
	{
		return FALSE;
	}

	return TRUE;
}


static void RelocateElf(BYTE* binary, UINT_PTR* bases)
{
	ELFHEADER64* header = (ELFHEADER64*)binary;
	ELFSECTIONHEADER64* sh = (ELFSECTIONHEADER64*)(binary + header->SectionHeaderOffset);

	for (QWORD i = 0; i < header->SectionHeaderCount; i++)
	{
		if (sh[i].Type != SHT_RELA && sh[i].Type != SHT_REL)
		{
			continue;
		}

		if (!bases[sh[i].Info])
		{
			continue;
		}

		ELFSYMBOL64* symbol = (ELFSYMBOL64*)(binary + sh[i].Link);
		ELFSECTIONHEADER64* targetSec = &sh[sh[i].Info];
		BYTE* targetSecCode = (BYTE*)bases[sh[i].Info];


		for (BYTE* p = binary + sh[i].Offset;
			 p < binary + sh[i].Offset + sh[i].Size;
			 p += sh[i].EntrySize)
		{
			ELFRELA64* rela = (ELFRELA64*)p;
			const ELFSYMBOL64* sym = &symbol[ELF64_R_SYM(rela->Info)];
			if (sym->SectionIndex == SHN_UNDEF)
			{
				// TODO: handle undefined symbol by looking up in kernel symbol table
				continue;
			}

			if (!bases[sym->SectionIndex])
			{
				Panic("Unexpected symbol section index from unloaded section.");
			}

			BYTE* target = targetSecCode + rela->Offset;

			const QWORD A = sh[i].Type == SHT_RELA ? rela->Addend : 0;
			// Spec says: for relocatable files, symbol value is the offset from the beginning of the section
			const QWORD S = sym->Value - sh[sym->SectionIndex].Offset + bases[sym->SectionIndex];
			const QWORD P = (QWORD)target;

			switch (ELF64_R_TYPE(rela->Info))
			{
			case R_X86_64_NONE:
				// Do nothing
				break;
			case R_X86_64_64:
				*(QWORD*)target = (QWORD)(A + S);
				break;
			case R_X86_64_PC64:
				*(QWORD*)target = (QWORD)(A + S - P);
				break;
			case R_X86_64_PC32:
				*(DWORD*)target = (DWORD)(A + S - P);
				break;
			case R_X86_64_32:
				*(DWORD*)target = (DWORD)(A + S);
				break;
			case R_X86_64_32S:
				*(INT*)target = (INT)(A + S);
				break;
			default:
				TerminalPrintf("Unknown relocation type %d\n", ELF64_R_TYPE(rela->Info));
				Panic("Unable to load module.");
				break;
			}
		}
	}
}

SSIZE_T LoadKernelElf(BYTE* binary, UINT_PTR* entry)
{
	ELFHEADER64* header = (ELFHEADER64*)binary;

	ERRORCODE ret = CheckElfCommon(header);
	if (ret < 0)
	{
		return ret;
	}

	if (header->Type != ELFTYPE_EXEC)
	{
		return -E_INVALID;
	}

	ELFPROGRAMHEADER64* ph = (ELFPROGRAMHEADER64*)(binary + header->ProgramHeaderOffset);
	SSIZE_T size = 0;
	for (QWORD i = 0; i < header->ProgramHeaderCount; i++)
	{
		SIZE_T loadSize = ph[i].VirtualAddress + ph[i].MemorySize;
		if (ph[i].VirtualAddress > KERNEL_LINK_ADDR)
		{
			loadSize -= KERNEL_LINK_ADDR;
		}
		size = MAX(size, (SSIZE_T)loadSize);
		if (ph[i].Type == ELFPROG_LOAD)
		{
			__builtin_memcpy((BYTE*)ph[i].VirtualAddress, binary + ph[i].Offset, ph[i].FileSize);
			__builtin_memset((BYTE*)ph[i].VirtualAddress + ph[i].FileSize, 0, ph[i].MemorySize - ph[i].FileSize);
		}
	}

	*entry = header->Entry;
	return size;
}

SSIZE_T LoadModuleElf(BYTE* binary, UINT_PTR base)
{
	ELFHEADER64* header = (ELFHEADER64*)binary;
	UINT_PTR bases[header->SectionHeaderCount];
	__builtin_memset(bases, 0, sizeof(bases));

	ERRORCODE ret = CheckElfCommon(header);
	if (ret < 0)
	{
		return ret;
	}

	if (header->Type != ELFTYPE_REL && header->Type != ELFTYPE_EXEC)
	{
		return -E_INVALID;
	}


	SSIZE_T size = 0;

	BYTE* loadMemory = (BYTE*)base;

	ELFSECTIONHEADER64* sh = (ELFSECTIONHEADER64*)(binary + header->SectionHeaderOffset);
	for (QWORD i = 0; i < header->SectionHeaderCount; i++)
	{
		if (!ShouldLoadModuleSection(&sh[i]))
		{
			continue;
		}

		BYTE* beforeLoad = loadMemory;

		if (sh[i].Alignment != 0 && sh[i].Alignment != 1)
		{
			loadMemory = (BYTE*)ROUNDUP((UINT_PTR)loadMemory, sh[i].Alignment);
		}

		if (sh[i].Type == SHT_NOBITS)
		{
			__builtin_memset(loadMemory, 0, sh[i].Size);
		}
		else if (sh[i].Type == SHT_PROGBITS)
		{
			__builtin_memcpy(loadMemory, binary + sh[i].Offset, sh[i].Size);
		}

		bases[i] = (UINT_PTR)loadMemory;
		loadMemory += sh[i].Size;
		size += (SSIZE_T)(loadMemory - beforeLoad);
	}

	RelocateElf(binary, bases);
	return size;
}

UINT_PTR LocateSymbolElf(BYTE* binary, const char* name)
{
	return 0;
}