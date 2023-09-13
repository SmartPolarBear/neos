//
// Created by bear on 9/7/2023.
//
#include "type.h"
#include "draw.h"


#define DEVSPACE 0xFE000000         // Other devices are at high addresses
#define DEVBASE  0xFFFFFFFF40000000 // First device virtual address
#define IO2V(a) ((((void *) (a)) - DEVSPACE) + DEVBASE )

void DrawPixel(INT x, INT y, BYTE r, BYTE g, BYTE b)
{
	VBEMODEINFO const* modeInfo = (VBEMODEINFO const*)VESA_MODEINFO_ADDR;
	VOID* fbAddr = (VOID*)(QWORD)modeInfo->framebuffer;
	switch (modeInfo->bpp)
	{
	case 32:
	{
		DWORD* frameBuffer = (DWORD*)fbAddr;
		frameBuffer[x + y * modeInfo->pitch / 4] = (r << 16) | (g << 8) | b;
		break;
	}
	case 24:
	{
		BYTE* frameBuffer = (BYTE*)fbAddr;
		frameBuffer[x * 3 + y * modeInfo->pitch] = b;
		frameBuffer[x * 3 + y * modeInfo->pitch + 1] = g;
		frameBuffer[x * 3 + y * modeInfo->pitch + 2] = r;
		break;
	}
	case 16:
	{
		WORD* frameBuffer = (WORD*)fbAddr;
		frameBuffer[x + y * modeInfo->pitch / 2] = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
		break;
	}
	case 15:
	{
		WORD* frameBuffer = (WORD*)fbAddr;
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

void FillScreen(BYTE r, BYTE g, BYTE b)
{
	VBEMODEINFO const* modeInfo = (VBEMODEINFO const*)VESA_MODEINFO_ADDR;
	// fast fill
	DWORD color = 0;
	switch (modeInfo->bpp)
	{
	case 32:
		color = (r << 16) | (g << 8) | b;
		__asm__ __volatile__ ("rep stosl" : : "a" (color), "D" (modeInfo->framebuffer), "c" (modeInfo->pitch *
																							 modeInfo->height / 4));
		break;
	case 24:
		color = (r << 16) | (g << 8) | b;
		__asm__ __volatile__ ("rep stosb" : : "a" (color), "D" (modeInfo->framebuffer), "c" (modeInfo->pitch *
																							 modeInfo->height));
		break;
	case 16:
		color = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
		__asm__ __volatile__ ("rep stosw" : : "a" (color), "D" (modeInfo->framebuffer), "c" (modeInfo->pitch *
																							 modeInfo->height / 2));
		break;
	case 15:
		color = ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3);
		__asm__ __volatile__ ("rep stosw" : : "a" (color), "D" (modeInfo->framebuffer), "c" (modeInfo->pitch *
																							 modeInfo->height / 2));
		break;
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

