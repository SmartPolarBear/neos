//
// Created by bear on 8/31/2023.
//
#include "ext.h"
#include "terminal.h"
#include "vesa.h"
#include "draw.h"
#include "vgafont.h"
#include "defs.h"

struct
{
	INT row, col;
	DWORD fgColor, bgColor;
	INT rowCount, colCount;
} bootTerm;

inline void ALWAYS_INLINE IncreaseCursor()
{
	bootTerm.col++;
	if (bootTerm.col >= bootTerm.colCount)
	{
		bootTerm.col = 0;
		bootTerm.row++;
		if (bootTerm.row >= bootTerm.rowCount)
		{
			bootTerm.row = 0;
		}
	}
}

inline void ALWAYS_INLINE CursorToXY(INT* x, INT* y)
{
	*x = bootTerm.col << 3;
	*y = bootTerm.row << 4;
	*y += 12;
}

void InitializeTerminal()
{
	VBEMODEINFO* modeInfo = (VBEMODEINFO*)VESA_MODEINFO_ADDR;

	bootTerm.row = 0;
	bootTerm.col = 0;

	bootTerm.fgColor = LIGHTGRAY;
	bootTerm.bgColor = BLUE;

	bootTerm.rowCount = modeInfo->height >> 4;
	bootTerm.colCount = modeInfo->width >> 3;
}

void TerminalSetColor(DWORD fgcolor, DWORD bgcolor)
{
	bootTerm.fgColor = fgcolor;
	bootTerm.bgColor = bgcolor;
}

void TerminalWriteCharacter(char c)
{
	// check if c is displayable
	if (c == '\n')
	{
		bootTerm.col = 0;
		bootTerm.row++;
		if (bootTerm.row >= bootTerm.rowCount)
		{
			bootTerm.row = 0;
		}
		return;
	}

	int x = 0, y = 0;
	CursorToXY(&x, &y);
	DrawCharacterBackDrop(c, x, y, bootTerm.fgColor, bootTerm.bgColor);
	IncreaseCursor();
}

void TerminalWriteString(char* str)
{
	for (int i = 0; str[i] != 0; i++)
	{
		TerminalWriteCharacter(str[i]);
	}
}

void TerminalMoveCursor(INT x, INT y)
{
	bootTerm.row = x;
	bootTerm.col = y;
}

void TerminalClear()
{
	VBEMODEINFO* modeInfo = (VBEMODEINFO*)VESA_MODEINFO_ADDR;
	bootTerm.row = bootTerm.col = 0;
	PutRect(0, 0, modeInfo->width, modeInfo->height, 0, 0, 255);
}