//
// Created by bear on 9/10/2023.
//

#include "type.h"
#include "defs.h"
#include "terminal.h"
#include "mem.h"

BYTE* lowTop, * highTop;

#define LOW_LIMIT 0x80000

void InitializeMemory(VOID* low)
{
	lowTop = (BYTE*)PGROUNDUP((UINT_PTR)low);
	highTop = (BYTE*)KERNEL_REAL_ADDR;
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
	return ret;
}

void InitializeMemoryPages()
{
}

void PostInitializeMemory()
{
// todo: Fill some sentinel values for the sake of debugging
}