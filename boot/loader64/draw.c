//
// Created by bear on 9/7/2023.
//
#include "draw.h"

void DrawPixel(INT x, INT y, BYTE r, BYTE g, BYTE b)
{
	VBEMODEINFO const* modeInfo = (VBEMODEINFO const*)VESA_MODEINFO_ADDR;
	switch (modeInfo->bpp)
	{
	case 32:
	{
		DWORD* frameBuffer = (DWORD*)modeInfo->framebuffer;
		frameBuffer[x + y * modeInfo->pitch / 4] = (r << 16) | (g << 8) | b;
		break;
	}
	case 24:
	{
		BYTE* frameBuffer = (BYTE*)modeInfo->framebuffer;
		frameBuffer[x * 3 + y * modeInfo->pitch] = b;
		frameBuffer[x * 3 + y * modeInfo->pitch + 1] = g;
		frameBuffer[x * 3 + y * modeInfo->pitch + 2] = r;
		break;
	}
	case 16:
	{
		WORD* frameBuffer = (WORD*)modeInfo->framebuffer;
		frameBuffer[x + y * modeInfo->pitch / 2] = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
		break;
	}
	case 15:
	{
		WORD* frameBuffer = (WORD*)modeInfo->framebuffer;
		frameBuffer[x + y * modeInfo->pitch / 2] = ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3);
		break;
	}
	}
}

void DrawRect(INT x, INT y, INT w, INT h, BYTE r, BYTE g, BYTE b)
{
	for (INT i = 0; i < h; i++)
	{
		for (INT j = 0; j < w; j++)
		{
			DrawPixel(x + j, y + i, r, g, b);
		}
	}
}

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
				DrawPixel(x + 7 - cx, y + cy - 12, R(fgcolor), G(fgcolor), B(fgcolor));
			}
			else
			{
				DrawPixel(x + 7 - cx, y + cy - 12, R(bgcolor), G(bgcolor), B(bgcolor));
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
				DrawPixel(x + 7 - cx, y + cy - 12, R(fgcolor), G(fgcolor), B(fgcolor));
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

