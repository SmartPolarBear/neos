//
// Created by bear on 8/28/2023.
//
#include "ext.h"
#include "draw.h"
#include "terminal.h"
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
	for (;;);
}