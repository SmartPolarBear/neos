//
// Created by bear on 8/28/2023.
//
#include "ext.h"
#include "draw.h"
#include "terminal.h"
#include "mem.h"
#include "fs.h"
#include "vgafont.h"

// 32bit code
__asm__(".code32");


void NO_RETURN LoaderMain32()
{
	// Blue background
	PutRect(0, 0, 800, 600, 0, 0, 255);
	// Boot-time terminal
	InitializeTerminal();
	TerminalWriteString("OS Loader (Built on " __DATE__ " " __TIME__ ")\n");

	// Briefing the e820 mem map

	// Enumerate partition table and search for the kernel
	InitializeBootFS();

	// Not able to jump to kernel so report the error and spin
	TerminalSetColor(RED, BLUE);
	TerminalWriteString("Failed to load the kernel.\n");
	for (;;)
		__asm__ volatile ("hlt");
}