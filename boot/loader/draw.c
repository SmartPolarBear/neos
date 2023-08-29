//
// Created by bear on 8/27/2023.
//
#include "ext.h"
#include "defs.h"
#include "vesa.h"
#include "vga.h"
#include "draw.h"


void PutRect(INT x, INT y, INT w, INT h, BYTE r, BYTE g, BYTE b)
{
	for (INT i = 0; i < h; i++)
	{
		for (INT j = 0; j < w; j++)
		{
			PutPixel(x + j, y + i, r, g, b);
		}
	}
}


void PutPixel(INT x, INT y, BYTE r, BYTE g, BYTE b)
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


