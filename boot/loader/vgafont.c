//
// Created by bear on 8/27/2023.
//
#include "type.h"
#include "defs.h"
#include "vesa.h"
#include "vga.h"
#include "draw.h"

// 32bit code
__asm__(".code32");

volatile BYTE* font = (BYTE*)VGA_FONT_ADDR;

void DrawCharacter(char c, int x, int y, int fgcolor, int bgcolor)
{
	int cx, cy;
	int mask[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };
	volatile BYTE* glyph = font + (int)c * 16;

	for (cy = 0; cy < 16; cy++)
	{
		for (cx = 0; cx < 8; cx++)
		{
			INT color = glyph[cy] & mask[cx] ? fgcolor : bgcolor;
			PutPixel(x + cx, y + cy - 12, R(color), G(color), B(color));
		}
	}
}

void DrawCharacterTransparent(char c, int x, int y, int fgcolor)
{
	int cx, cy;
	int mask[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };
	volatile BYTE* glyph = font + (int)c * 16;

	for (cy = 0; cy < 16; cy++)
	{
		for (cx = 0; cx < 8; cx++)
		{
			if (glyph[cy] & mask[cx])
			{
				PutPixel(x + cx, y + cy - 12, R(fgcolor), G(fgcolor), B(fgcolor));
			}
		}
	}
}