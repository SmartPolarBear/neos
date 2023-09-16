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


// NELDR do following things:
// 0) Initialize memory stuffs
// 1) first, load kernel binaries, as well as smp initialization code
// 2) place physical memory pages just after kernel binaries. Also initialize other processors.
// 3) scan hardware and place information after pages with ACPI
// 4) jmp to kernel entry
void NO_RETURN LoaderMain64(UINT_PTR bufferTop, UINT_PTR activePartAddr)
{
	// boot-time terminal
	InitializeTerminal();
	TerminalClear();
	TerminalWriteString("NELOS is scanning hardware...");

	// boot-time memory allocator
	InitializeMemory((BYTE*)bufferTop);

	// Boot-time file system
	InitializeBootFs((PARTTABLEITEM*)activePartAddr);

	// ACPI
	InitializeAcpi();

	// Load kernel
	LoadKernel();

	// Place memory pages
	InitializeMemoryPages();

	// Load drivers based on ACPI information
	AcpiLoadDriverForDevices();
	// Initialize other processors, but not start them, leaving works to kernel.
	AcpiInitializeProcessors();

	// Memory post-initialization
	PostInitializeMemory();

	Panic("Cannot jump to kernel.");
}