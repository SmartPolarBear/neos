//
// Created by bear on 9/5/2023.
//
#include "ext.h"
#include "draw.h"

// NELDR do following things:
// 1) first, load kernel binaries, as well as smp initialization code
// 2) place physical memory pages just after kernel binaries
// 3) scan hardware and place information after pages
// 4) jmp to kernel entry
void NO_RETURN LoaderMain64()
{
	// remap vbe framebuffer
	InitializeGraphics();

	VBEMODEINFO const* modeInfo = (VBEMODEINFO const*)VESA_MODEINFO_ADDR;
	// Blue background
	DrawRect(0, 0, modeInfo->width, modeInfo->height, 0, 0, 0);

	for (;;)
		__asm__ volatile("hlt");
}