//
// Created by bear on 8/27/2023.
//
#include "type.h"
#include "defs.h"
#include "vesa.h"
#include "vgafont.h"
#include "draw.h"

// 32bit code
__asm__(".code32");



void DrawCharacterBackDrop(char c, int x, int y, COLOR fgcolor, COLOR bgcolor)
{
	volatile BYTE* glyph = CHAR_BITMAP(c);

	for (int cy = 0; cy < 16; cy++)
	{
		BYTE line = glyph[cy];
		for (int cx = 0; cx < 8; cx++)
		{
			if (line & (1 << cx))
			{
				PutPixel(x + 7 - cx, y + cy - 12, R(fgcolor), G(fgcolor), B(fgcolor));
			}
			else
			{
				PutPixel(x + 7 - cx, y + cy - 12, R(bgcolor), G(bgcolor), B(bgcolor));
			}
		}
	}
}

void DrawCharacter(char c, int x, int y, COLOR fgcolor)
{
	volatile BYTE* glyph = CHAR_BITMAP(c);

	for (int cy = 0; cy < 16; cy++)
	{
		BYTE line = glyph[cy];
		for (int cx = 0; cx < 8; cx++)
		{
			if (line & (1 << cx))
			{
				PutPixel(x + 7 - cx, y + cy - 12, R(fgcolor), G(fgcolor), B(fgcolor));
			}
		}
	}
}

void DrawString(char* str, int x, int y, COLOR fgcolor)
{
	for (int i = 0; str[i] != 0; i++)
	{
		DrawCharacter(str[i], x + i * 8, y, fgcolor);
	}
}

void DrawStringBackDrop(char* str, int x, int y, COLOR fgcolor, COLOR bgcolor)
{
	for (int i = 0; str[i] != 0; i++)
	{
		DrawCharacterBackDrop(str[i], x + i * 8, y, fgcolor, bgcolor);
	}
}

