//
// Created by bear on 9/16/2023.
//
#pragma once

#include "ext.h"
#include "type.h"

typedef struct
{
	DWORD Magic;
	BYTE Bit;
	BYTE Endian;
	BYTE Version;
	BYTE ABI;
	QWORD Padding;
	WORD Type;
	WORD Machine;
	DWORD Version2;
	QWORD Entry;
	QWORD ProgramHeaderOffset;
	QWORD SectionHeaderOffset;
	DWORD Flags;
	WORD HeaderSize;
	WORD ProgramHeaderEntrySize;
	WORD ProgramHeaderCount;
	WORD SectionHeaderEntrySize;
	WORD SectionHeaderCount;
	WORD SectionHeaderStringIndex;
}PACKED ELFHEADER64;
_Static_assert(sizeof(ELFHEADER64) == 64, "ELFHEADER64 size mismatch");

// "\x7fELF"
#define ELF_MAGIC 0x464c457f

#define ELF_BIT_32 1
#define ELF_BIT_64 2

typedef enum
{
	ELFMACHINE_NOSPEC = 0,
	ELFMACHINE_SPARC = 2,
	ELFMACHINE_X86 = 3,
	ELFMACHINE_MIPS = 8,
	ELFMACHINE_POWERPC = 0x14,
	ELFMACHINE_ARM = 0x28,
	ELFMACHINE_SUPERH = 0x2a,
	ELFMACHINE_IA64 = 0x32,
	ELFMACHINE_X8664 = 0x3e,
	ELFMACHINE_AARCH64 = 0xb7,
	ELFMACHINE_RISCV = 0xf3,
} ELFMACHINE;

typedef enum
{
	ELFTYPE_NONE = 0,
	ELFTYPE_REL = 1,
	ELFTYPE_EXEC = 2,
	ELFTYPE_DYN = 3,
	ELFTYPE_CORE = 4,
} ELFTYPE;

typedef struct
{
	DWORD Type;
	DWORD Flags;
	QWORD Offset;
	QWORD VirtualAddress;
	QWORD PhysicalAddress;
	QWORD FileSize;
	QWORD MemorySize;
	QWORD Alignment;
} PACKED ELFPROGRAMHEADER64;
_Static_assert(sizeof(ELFPROGRAMHEADER64) == 56, "ELFPROGRAMHEADER64 size mismatch");
typedef enum
{
	ELFPROG_NULL = 0,
	ELFPROG_LOAD = 1,
	ELFPROG_DYNAMIC = 2,
	ELFPROG_INTERP = 3,
	ELFPROG_NOTE = 4,
	ELFPROG_SHLIB = 5,
	ELFPROG_PHDR = 6,
	ELFPROG_TLS = 7,
	ELFPROG_LOOS = 0x60000000,
	ELFPROG_HIOS = 0x6fffffff,
	ELFPROG_LOPROC = 0x70000000,
	ELFPROG_HIPROC = 0x7fffffff,
} ELFPHTYPE;

typedef enum
{
	ELFFLAGS_NONE = 0,
	ELFFLAGS_EXECUTABLE = 1,
	ELFFLAGS_WRITABLE = 2,
	ELFFLAGS_READABLE = 4,
} ELFFLAGS;

typedef struct
{
	DWORD Name;
	DWORD Type;
	QWORD Flags;
	QWORD VirtualAddress;
	QWORD Offset;
	QWORD Size;
	DWORD Link;
	DWORD Info;
	QWORD Alignment;
	QWORD EntrySize;
} PACKED ELFSECTIONHEADER64;
_Static_assert(sizeof(ELFSECTIONHEADER64) == 64, "ELFSECTIONHEADER64 size mismatch");


