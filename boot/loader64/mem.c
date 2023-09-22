//
// Created by bear on 9/10/2023.
//

#include "type.h"
#include "defs.h"
#include "terminal.h"
#include "mem.h"
#include "utils.h"
#include "param.h"

#include "kernel/mm/pmm.h"

BYTE* lowTop, * highTop;

#define LOW_LIMIT 0x80000

void InitializeMemory(VOID* low)
{
	lowTop = (BYTE*)PGROUNDUP((UINT_PTR)low);
	highTop = (BYTE*)NELDR_HIGH_BUFFER;
}

void* AllocateLow(int nPages)
{
	return AllocateLowBytes(nPages * PGSIZE);
}

void* AllocateLowBytes(SIZE_T nBytes)
{
	BYTE* ret = lowTop;
	if (ret >= (BYTE*)LOW_LIMIT)
	{
		return NULL;
	}
	lowTop += nBytes;
	return ret;
}

void* AllocateHigh(int nPages)
{
	BYTE* ret = highTop;
	highTop += 0x1000 * nPages;
	if (highTop >= (BYTE*)KERNEL_LOAD_ADDR)
	{
		return NULL;
	}
	return ret;
}

void* AllocateHighBytes(SIZE_T nBytes)
{
	BYTE* ret = highTop;
	highTop += nBytes;
	if (highTop >= (BYTE*)KERNEL_LOAD_ADDR)
	{
		return NULL;
	}
	return ret;
}

extern BYTE* loadMemory; // defined in loader.c

void InitializeMemoryPages()
{
	BYTE* pageStart = loadMemory;

	// Place memory pages
	PHYSICALPAGE* pages = (PHYSICALPAGE*)pageStart;

	// Start by casting MMAP_ADDR to the correct pointer type
	DWORD mmapCount = *(DWORD*)MMAP_ADDR;
	E820MEMMAP* e820Map = (E820MEMMAP*)(MMAP_ADDR + sizeof(DWORD));

	SIZE_T usablePages = 0, reservedPages = 0, acpiPages = 0, acpiNvsPages = 0;

	for (int i = 0; i < mmapCount; i++)
	{
		BYTE* baseAddr = (BYTE*)((UINT_PTR)e820Map[i].BaseAddrHigh << 32 | e820Map[i].BaseAddrLow);
		SIZE_T length = (SIZE_T)((UINT_PTR)e820Map[i].LengthHigh << 32 | e820Map[i].LengthLow);
		for (BYTE* p = baseAddr; p < baseAddr + length; p += PGSIZE)
		{
			MemSet(pages, 0, sizeof(PHYSICALPAGE));
			switch (e820Map[i].Type)
			{
			case MEM_TYPE_USABLE:
				pages->Flags |= PPF_NONE;
				usablePages++;
				break;
			case MEM_TYPE_ACPI:
				pages->Flags |= PPF_ACPI;
				pages->Flags |= PPF_RECLAIMABLE;
				acpiPages++;
				break;
			case MEM_TYPE_ACPI_NVS:
				pages->Flags |= PPF_ACPI;
				acpiNvsPages++;
				break;
			default:
			case MEM_TYPE_RESERVED:
			case MEM_TYPE_UNUSABLE:
				// Do not use this memory, so mark it as reserved
				pages->Flags |= PPF_RESERVED;
				reservedPages++;
				break;
			}
			pages->Ref = 0;
			pages++;
		}
	}

	// mark kernel-occupied memory as reserved
	for (BYTE* p = 0; p < (BYTE*)PGROUNDUP((UINT_PTR)pages); p += PGSIZE)
	{
		PHYSICALPAGE* kernPage = &((PHYSICALPAGE*)pageStart)[(UINT_PTR)p / PGSIZE];
		if (!(kernPage->Flags & PPF_RESERVED))
		{
			usablePages--;
		}
		reservedPages++;
		kernPage->Flags |= PPF_RESERVED;
	}

	TerminalPrintf("Memory map: %d usable pages, %d reserved pages, %d ACPI pages, %d ACPI NVS pages.\n",
			usablePages, reservedPages, acpiPages, acpiNvsPages);

	loadMemory = (BYTE*)PGROUNDUP((UINT_PTR)pages);

	gBootParam.PhysicalPages = (UINT_PTR)pages;
	gBootParam.PhysicalPageCount = usablePages + reservedPages + acpiPages + acpiNvsPages;
}

void PostInitializeMemory()
{
	// fill from loadMemory to 2GB with 0xdeadbeef to detect memory corruption
	TerminalPrintf("%p\n", loadMemory);
	__asm__ volatile("cld; rep stosq" : :
			"a"(0xdeadbeefdeadbeef),
			"D"(loadMemory),
			"c"((0x80000000 - (UINT_PTR)loadMemory) >> 3));
}