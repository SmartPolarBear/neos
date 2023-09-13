//
// Created by bear on 9/10/2023.
//

#pragma once

#include "type.h"

#define PGSIZE 0x1000
#define PGROUNDUP(sz)  (((sz)+((UINT_PTR)PGSIZE-1)) & ~((UINT_PTR)(PGSIZE-1)))

void InitializeMemory(VOID* lowTop);

void* AllocateLow(int nPages);

// Do not enforce alignment on low memory, but enforce that shit on higher memory.
void* AllocateLowBytes(SIZE_T nBytes);

void* AllocateHigh(int nPages);