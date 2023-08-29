//
// Created by bear on 8/28/2023.
//
#include "ext.h"
#include "draw.h"
#include "vga.h"

// 32bit code
__asm__(".code32");

void NO_RETURN LoaderMain32()
{
	// Blue background
	PutRect(0, 0, 800, 600, 0, 0, 255);

	for (int i = 0; i < 26; i++)
	{
		DrawCharacterTransparent('A' + i, 100 + i * 20, 100, YELLOW);
	}
	for (;;);
}