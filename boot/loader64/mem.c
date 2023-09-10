//
// Created by bear on 9/10/2023.
//

#include "type.h"
#include "defs.h"
#include "mem.h"

BYTE* lowTop, * highTop;

void InitializeMemory(VOID* low)
{
	lowTop = (BYTE*)PGROUNDUP((UINT_PTR)low);
	highTop = (BYTE*)KERNEL_REAL_ADDR;
}

void* AllocateLow(int nPages)
{
	BYTE* ret = lowTop;
	if (ret >= highTop)
	{
		return NULL;
	}
	lowTop += 0x1000 * nPages;
	return ret;
}

void* AllocateHigh(int nPages)
{
	BYTE* ret = highTop;
	highTop += 0x1000 * nPages;
	return ret;
}