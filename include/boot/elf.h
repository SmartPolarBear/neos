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

typedef enum
{
	SHT_NULL = 0,
	SHT_PROGBITS = 1,
	SHT_SYMTAB = 2,
	SHT_STRTAB = 3,
	SHT_RELA = 4,
	SHT_HASH = 5,
	SHT_DYNAMIC = 6,
	SHT_NOTE = 7,
	SHT_NOBITS = 8,
	SHT_REL = 9,
	SHT_SHLIB = 10,
	SHT_DYNSYM = 11,
	SHT_LOPROC = 0x70000000,
	SHT_HIPROC = 0x7fffffff,
	SHT_LOUSER = 0x80000000,
	SHT_HIUSER = 0xffffffff,
} ELFSHTYPE;

typedef enum
{
	SHF_WRITE = 1,
	SHF_ALLOC = 2,
	SHF_EXECINSTR = 4,
	SHF_MERGE = 0x10,
	SHF_STRINGS = 0x20,
	SHF_INFO_LINK = 0x40,
	SHF_LINK_ORDER = 0x80,
	SHF_OS_NONCONFORMING = 0x100,
	SHF_GROUP = 0x200,
	SHF_TLS = 0x400,
	SHF_MASKOS = 0x0ff00000,
	SHF_MASKPROC = 0xf0000000,
} ELFSHFLAGS;

typedef enum
{
	SHN_UNDEF = 0,
	SHN_LOPROC = 0xff00,
	SHN_HIOPROC = 0xff1f,
	SHN_LOOS = 0xff20,
	SHN_HIOS = 0xff3f,
	SHN_ABS = 0xfff1,
	SHN_COMMON = 0xfff2,
} ELFSHINDEX;


typedef struct
{
	QWORD Offset;
	QWORD Info;
}PACKED ELFREL64;

typedef struct
{
	QWORD Offset;
	QWORD Info;
	LONGLONG Addend;
}PACKED ELFRELA64;

typedef struct
{
	DWORD Name;
	BYTE Info;
	BYTE Other;
	WORD SectionIndex;
	QWORD Value;
	QWORD Size;
}PACKED ELFSYMBOL64;

typedef enum{
	STB_LOCAL = 0,
	STB_GLOBAL = 1,
	STB_WEAK = 2,
	STB_LOOS = 10,
	STB_HIOS = 12,
	STB_LOPROC = 13,
	STB_HIPROC = 15,
}ELFSYMBOLBINDING;

typedef enum
{
	STT_NOTYPE = 0,
	STT_OBJECT = 1,
	STT_FUNC = 2,
	STT_SECTION = 3,
	STT_FILE = 4,
	STT_LOOS = 10,
	STT_HIOS = 12,
	STT_LOPROC = 13,
	STT_HIPROC = 15,
}ELFSYMBOLTYPE;

#define ELF64_ST_BIND(i)((i) >> 4)
#define ELF64_ST_TYPE(i)((i) & 0xf)

typedef enum
{
	R_X86_64_NONE = 0,
	R_X86_64_64 = 1,
	R_X86_64_PC32 = 2,
	R_X86_64_GOT32 = 3,
	R_X86_64_PLT32 = 4,
	R_X86_64_COPY = 5,
	R_X86_64_GLOB_DAT = 6,
	R_X86_64_JUMP_SLOT = 7,
	R_X86_64_RELATIVE = 8,
	R_X86_64_GOTPCREL = 9,
	R_X86_64_32 = 10,
	R_X86_64_32S = 11,
	R_X86_64_16 = 12,
	R_X86_64_PC16 = 13,
	R_X86_64_8 = 14,
	R_X86_64_PC8 = 15,
	R_X86_64_PC64 = 24,
	R_X86_64_GOTOFF64 = 25,
	R_X86_64_GOTPC32 = 26,
	R_X86_64_SIZE32 = 32,
	R_X86_64_SIZE64 = 33,
} ELFRELTYPE64;

#define ELF64_R_SYM(i)((i) >> 32)
#define ELF64_R_TYPE(i)((i) & 0xffffffffL)
#define ELF64_R_INFO(s, t)(((s) << 32) + ((t) & 0xffffffffL))

