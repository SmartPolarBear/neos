//
// Created by bear on 9/5/2023.
//
#include "ext.h"
#include "defs.h"
#include "draw.h"
#include "mem.h"
#include "acpi.h"
#include "terminal.h"

// NELDR do following things:
// 0) Initialize memory stuffs
// 1) first, load kernel binaries, as well as smp initialization code
// 2) place physical memory pages just after kernel binaries
// 3) scan hardware and place information after pages
// 4) jmp to kernel entry
void NO_RETURN LoaderMain64(UINT_PTR bufferTop)
{
	// boot-time terminal
	InitializeTerminal();
	TerminalClear();
	TerminalWriteString("NELOS is scanning hardware...\n");

	// boot-time memory allocator
	InitializeMemory((BYTE*)bufferTop);

	// ACPI
	InitializeACPI();

	for (;;)
		__asm__ volatile("hlt");
}