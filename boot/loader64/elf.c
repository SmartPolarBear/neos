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

#include "ext.h"
#include "elf.h"
#include "error.h"

static inline ERRORCODE ALWAYS_INLINE CheckElfCommon(ELFHEADER64* header)
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

SSIZE_T LoadKernelElf(BYTE* binary, UINT_PTR* entry)
{
	ELFHEADER64* header = (ELFHEADER64*)binary;

	SSIZE_T ret = CheckElfCommon(header);
	if (ret < 0)
	{
		return ret;
	}

	if (header->Type != ELFTYPE_EXEC)
	{
		return -E_INVALID;
	}

	ELFPROGRAMHEADER64* ph = (ELFPROGRAMHEADER64*)(binary + header->ProgramHeaderOffset);
	for (QWORD i = 0; i < header->ProgramHeaderCount; i++)
	{
		if (ph[i].Type == ELFPROG_LOAD)
		{
			__builtin_memcpy((BYTE*)ph[i].VirtualAddress, binary + ph[i].Offset, ph[i].FileSize);
			__builtin_memset((BYTE*)ph[i].VirtualAddress + ph[i].FileSize, 0, ph[i].MemorySize - ph[i].FileSize);
		}
	}

	*entry = header->Entry;
	return E_SUCCESS;
}

SSIZE_T LoadModuleElf(BYTE* binary, UINT_PTR base)
{
	return 0;
}