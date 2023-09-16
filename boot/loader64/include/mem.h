//
// Created by bear on 9/10/2023.
//

#pragma once

#include "type.h"

#define PGSIZE 0x1000
#define PGROUNDUP(sz)  (((sz)+((UINT_PTR)PGSIZE-1)) & ~((UINT_PTR)(PGSIZE-1)))

void InitializeMemory(VOID* lowTop);

void* AllocateLow(int nPages);

void* AllocateHigh(int nPages);

// Do not enforce alignment on memory buffers, but enforce above kernel load address
void* AllocateLowBytes(SIZE_T nBytes);

void* AllocateHighBytes(SIZE_T nBytes);

void * AllocateKernelLoad(int nPages);

// Initialize memory pages
void InitializeMemoryPages();

// Fill some sentinel values for the sake of debugging
void PostInitializeMemory();