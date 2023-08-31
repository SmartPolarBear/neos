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


#define CHAR_BITMAP(c) (((volatile BYTE*)VGA_FONT_ADDR)+(((WORD)c)<<4))

void DrawCharacter(char c, int x, int y, int fgcolor, int bgcolor)
{
	volatile BYTE* glyph = CHAR_BITMAP(c);

	for (int cy = 0; cy < 16; cy++)
	{
		BYTE line = glyph[cy];
		for (int cx = 0; cx < 8; cx++)
		{
			if (line & (1 << cx))
			{
				PutPixel(x + cx, y + cy - 12, R(fgcolor), G(fgcolor), B(fgcolor));
			}
			else
			{
				PutPixel(x + cx, y + cy - 12, R(bgcolor), G(bgcolor), B(bgcolor));
			}
		}
	}
}

void DrawCharacterTransparent(char c, int x, int y, int fgcolor)
{
	volatile BYTE* glyph = CHAR_BITMAP(c);

	for (int cy = 0; cy < 16; cy++)
	{
		BYTE line = glyph[cy];
		for (int cx = 0; cx < 8; cx++)
		{
			if (line & (1 << cx))
			{
				PutPixel(x + cx, y + cy - 12, R(fgcolor), G(fgcolor), B(fgcolor));
			}
		}
	}
}