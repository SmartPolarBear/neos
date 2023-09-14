//
// Created by bear on 9/5/2023.
//
#include "ext.h"
#include "defs.h"
#include "utils.h"
#include "fs.h"
#include "mem.h"
#include "acpi.h"
#include "terminal.h"

#include "boot/fs.h"

extern PARTTABLEITEM* activePartition;

// NELDR do following things:
// 0) Initialize memory stuffs
// 1) first, load kernel binaries, as well as smp initialization code
// 2) place physical memory pages just after kernel binaries
// 3) scan hardware and place information after pages with ACPI
// 4) jmp to kernel entry
void NO_RETURN LoaderMain64(UINT_PTR bufferTop, UINT_PTR activePartAddr)
{
	activePartition = (PARTTABLEITEM*)activePartAddr;

	// boot-time terminal
	InitializeTerminal();
	TerminalClear();
	TerminalWriteString("NELOS is scanning hardware...");

	// boot-time memory allocator
	InitializeMemory((BYTE*)bufferTop);

	// ACPI
	InitializeAcpi();

	// Boot-time file system
	InitializeBootFs();

	// Load kernel
	LoadKernel();

	// Place memory pages
	InitializeMemoryPages();

	// Load drivers based on ACPI information
	AcpiLoadDriverForDevices();

	// Memory post-initialization
	PostInitializeMemory();

	Panic("Cannot jump to kernel.");
}