//
// Created by bear on 8/28/2023.
//
#include "defs.h"
#include "entry.h"
#include "ext.h"
#include "draw.h"
#include "terminal.h"
#include "mem.h"
#include "fs.h"
#include "vgafont.h"

// 32bit code
__asm__(".code32");


int LoaderMain32()
{
	// Blue background
	PutRect(0, 0, 800, 600, 0, 0, 255);
	// Boot-time terminal
	InitializeTerminal();
	TerminalWriteString("OS Loader (Built on " __DATE__ " " __TIME__ ")\n");

	// Briefing the e820 mem map
	BriefMemoryMap();

	// Enumerate partition table and search for the kernel
	InitializeBootFS();

	// Load the kernel
	INT result = LoadKernel(KERNEL_LOAD_ADDR);

	if (result > 0)
	{
		TerminalPrintf("Boot manager (%d bytes) loaded successfully.\n", result);
		// This lead to a jump to the kernel
		return 0;
	}

	// Not able to jump to kernel so report the error and spin
	TerminalSetColor(RED, BLUE);
	TerminalWriteString("Unexpectedly failed to start.\n");
	BootPanic();
}