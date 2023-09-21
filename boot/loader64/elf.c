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


static void PatchRelocatableElf(BYTE* binary, UINT_PTR* bases)
{
	ELFHEADER64* header = (ELFHEADER64*)binary;
	ELFSECTIONHEADER64* sh = (ELFSECTIONHEADER64*)(binary + header->SectionHeaderOffset);

	// process rela/rel sections
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


		ELFSECTIONHEADER64* targetSec = &sh[sh[i].Info];
		ELFSECTIONHEADER64* targetSecSymTbl = &sh[sh[i].Link];
		ELFSECTIONHEADER64* targetSecSymStrTbl = &sh[targetSecSymTbl->Link];
		BYTE* targetSecBinary = binary + targetSec->Offset;
		ELFSYMBOL64* symbol = (ELFSYMBOL64*)(binary + targetSecSymTbl->Offset);

//		TerminalPrintf("Relocating for section %d\n", sh[i].Info);

		for (BYTE* p = binary + sh[i].Offset;
			 p < binary + sh[i].Offset + sh[i].Size;
			 p += sh[i].EntrySize)
		{
			ELFRELA64* rela = (ELFRELA64*)p;
			ELFSYMBOL64* sym = &symbol[ELF64_R_SYM(rela->Info)];
			if (sym->SectionIndex == SHN_UNDEF)
			{
				// TODO: handle undefined symbol by looking up in kernel symbol table
				continue;
			}

			if (!bases[sym->SectionIndex])
			{
				Panic("Unexpected symbol section index from unloaded section.");
			}

			BYTE* targetInBinary = targetSecBinary + rela->Offset;

			QWORD A = sh[i].Type == SHT_RELA ? rela->Addend : 0;
			// Spec says: for relocatable files, symbol value is the offset from the beginning of the section
			const QWORD S = sym->Value;
			const QWORD P = (QWORD)rela->Offset;

			A += bases[sym->SectionIndex];

			TerminalPrintf("Patch symbol %s, type %d, A %p, S %p, P %p\n",
					binary + targetSecSymStrTbl->Offset + sym->Name, ELF64_R_TYPE(rela->Info), A, S, P);

			// patch the targets
			switch (ELF64_R_TYPE(rela->Info))
			{
			case R_X86_64_NONE:
				// Do nothing
				break;
			case R_X86_64_64:
				*(QWORD*)targetInBinary = (QWORD)(A + S);
				break;
			case R_X86_64_PC64:
				*(QWORD*)targetInBinary = (QWORD)(A + S - P);
				break;
			case R_X86_64_PC32:
				*(DWORD*)targetInBinary = (DWORD)(A + S - P);
				break;
			case R_X86_64_32:
				*(DWORD*)targetInBinary = (DWORD)(A + S);
				break;
			case R_X86_64_32S:
				*(INT*)targetInBinary = (INT)(A + S);
				break;
			default:
				TerminalPrintf("Unknown relocation type %d\n", ELF64_R_TYPE(rela->Info));
				Panic("Unable to load module.");
				break;
			}
		}
	}

	// process symbol tables
	for (QWORD i = 0; i < header->SectionHeaderCount; i++)
	{
		if (sh[i].Type != SHT_SYMTAB)
		{
			continue;
		}

		ELFSYMBOL64* symbol = (ELFSYMBOL64*)(binary + sh[i].Offset);

		for (QWORD j = 0; j < sh[i].Size / sh[i].EntrySize; j++)
		{
			if (symbol[j].SectionIndex == SHN_UNDEF)
			{
				continue;
			}

			if (ELF64_ST_BIND(symbol[j].Info) != STB_GLOBAL)
			{
				continue;
			}

			if (!bases[symbol[j].SectionIndex])
			{
				Panic("Unexpected symbol section index from unloaded section.");
			}

			symbol[j].Value = symbol[j].Value + bases[symbol[j].SectionIndex];
		}
	}

	// process the entry point
	// find the section that contains the entry point, then calculate the new entry point
	for (QWORD i = 0; i < header->SectionHeaderCount; i++)
	{
		if (sh[i].Type != SHT_PROGBITS)
		{
			continue;
		}

		if (sh[i].VirtualAddress <= header->Entry && header->Entry < sh[i].VirtualAddress + sh[i].Size)
		{
			header->Entry = header->Entry - sh[i].VirtualAddress + bases[i];
			break;
		}
	}
}

SSIZE_T LoadKernelElf(BYTE* binary, OUT UINT_PTR* entry)
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

		if (ph[i].VirtualAddress >= KERNEL_LINK_ADDR)
		{
			loadSize -= KERNEL_LINK_ADDR;
		}
		size = MAX(size, (SSIZE_T)loadSize);
		if (ph[i].Type == ELFPROG_LOAD)
		{
//			TerminalPrintf(
//					"Loading segment %d, type %d, offset %p, vaddr %p, paddr %p, filesz %p, memsz %p, align %p\n",
//					i, ph[i].Type, ph[i].Offset, ph[i].VirtualAddress, ph[i].PhysicalAddress, ph[i].FileSize,
//					ph[i].MemorySize, ph[i].Alignment);

			MemCpy((BYTE*)ph[i].VirtualAddress, binary + ph[i].Offset, ph[i].FileSize);

			if (ph[i].FileSize < ph[i].MemorySize)
			{
				MemSet((BYTE*)ph[i].VirtualAddress + ph[i].FileSize, 0, ph[i].MemorySize - ph[i].FileSize);
			}
		}
	}

	*entry = header->Entry;
	return size;
}

SSIZE_T LoadModuleElf(BYTE* binary, UINT_PTR base, OUT UINT_PTR* entry)
{
	ELFHEADER64* header = (ELFHEADER64*)binary;

	ERRORCODE ret = CheckElfCommon(header);
	if (ret < 0)
	{
		return ret;
	}

	if (header->Type != ELFTYPE_REL && header->Type != ELFTYPE_DYN)
	{
		return -E_INVALID;
	}

	// for every loadable section, calculate the load address
	UINT_PTR bases[header->SectionHeaderCount];
	ELFSECTIONHEADER64* sh = (ELFSECTIONHEADER64*)(binary + header->SectionHeaderOffset);
	for (QWORD i = 0; i < header->SectionHeaderCount; i++)
	{
		if (!ShouldLoadModuleSection(&sh[i]))
		{
			bases[i] = 0;
			continue;
		}

		UINT_PTR secBase = base + (sh[i].VirtualAddress - MODULE_LINK_ADDR);
		if (sh[i].Alignment != 0 && sh[i].Alignment != 1)
		{
			secBase = ROUNDUP(secBase, sh[i].Alignment);
		}

		bases[i] = secBase;
	}

	// patch the executable based on .rela and .rel sections
	PatchRelocatableElf(binary, bases);

	// load the sections
	SSIZE_T size = 0;
	for (QWORD i = 0; i < header->SectionHeaderCount; i++)
	{
		if (!bases[i])
		{
			continue;
		}

		if (sh[i].Type == SHT_NOBITS)
		{
			MemSet((void*)bases[i], 0, sh[i].Size);
		}
		else if (sh[i].Type == SHT_PROGBITS)
		{
			MemCpy((void*)bases[i], binary + sh[i].Offset, sh[i].Size);
		}

		size = MAX(size, (SSIZE_T)(bases[i] + sh[i].Size - base));
	}


	*entry = header->Entry;
	return size;
}

ELFSYMBOL64* LocateSymbolElf(BYTE* binary, const char* name, ELFSYMBOLBINDING bind, ELFSYMBOLTYPE type)
{
	ELFHEADER64* header = (ELFHEADER64*)binary;
	ELFSECTIONHEADER64* sh = (ELFSECTIONHEADER64*)(binary + header->SectionHeaderOffset);

	for (QWORD i = 0; i < header->SectionHeaderCount; i++)
	{
		if (sh[i].Type != SHT_SYMTAB)
		{
			continue;
		}

		ELFSECTIONHEADER64* strtab = &sh[sh[i].Link];
		BYTE* strtabData = binary + strtab->Offset;

		ELFSYMBOL64* symbol = (ELFSYMBOL64*)(binary + sh[i].Offset);
		for (QWORD j = 0; j < sh[i].Size / sh[i].EntrySize; j++)
		{
			if (symbol[j].Name == 0)
			{
				continue;
			}

			const char* symbolName = (const char*)(strtabData + symbol[j].Name);

			if (ELF64_ST_BIND(symbol[j].Info) != bind)
			{
				continue;
			}

			if (ELF64_ST_TYPE(symbol[j].Info) != type)
			{
				continue;
			}

			if (StrCmp(name, symbolName) == 0)
			{
				return &symbol[j];
			}
		}
	}

	return NULL;
}