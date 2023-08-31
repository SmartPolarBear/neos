//
// Created by bear on 8/28/2023.
//
#include "ext.h"
#include "draw.h"
#include "vgafont.h"

// 32bit code
__asm__(".code32");


void NO_RETURN LoaderMain32()
{
	// Blue background
	PutRect(0, 0, 800, 600, 0, 0, 255);
	// Draw string
	DrawString("OSLoader Built at", 0, 12, YELLOW);
	for (;;);
}