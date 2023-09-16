//
// Created by bear on 8/28/2023.
//
#include "defs.h"
#include "entry.h"
#include "ext.h"
#include "draw.h"
#include "vesa.h"
#include "terminal.h"
#include "mem.h"
#include "fs.h"
#include "vgafont.h"

// 32bit code
__asm__(".code32");

extern PARTTABLEITEM* activePartition;

int LoaderMain32()
{
	VBEMODEINFO const* modeInfo = (VBEMODEINFO const*)VESA_MODEINFO_ADDR;
	// Blue background
	PutRect(0, 0, modeInfo->width, modeInfo->height, 0, 0, 255);
	// Boot-time terminal
	InitializeTerminal();
	TerminalWriteString("OS Loader (Built on " __DATE__ " " __TIME__ ")\n");

	// MAYBE 0xfd000000
	TerminalPrintf("VBE framebuffer at %x,%x\n", (modeInfo->framebuffer >> 16) & 0x0000ffff,
			modeInfo->framebuffer & 0x0000ffff);

	// Briefing the e820 mem map
	BriefMemoryMap();

	// Enumerate partition table and search for the kernel
	InitializeBootFS();

	// Load the kernel
	INT result = LoadKernel(NELDR_LOAD_ADDR);

	if (result > 0)
	{
		TerminalPrintf("Boot manager (%d bytes) loaded successfully.\n", result);
		// This lead to a jump to the kernel
//		for(;;);
		// save active partition address to ebx
		__asm__ volatile("movl %0, %%ebx" : : "r"(activePartition));
		return 0;
	}

	// Not able to jump to kernel so report the error and spin
	TerminalSetColor(RED, BLUE);
	TerminalWriteString("Unexpectedly failed to start.\n");
	BootPanic();
}