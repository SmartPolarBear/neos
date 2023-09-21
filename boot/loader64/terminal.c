//
// Created by bear on 9/7/2023.
//
#include "param.h"
#include "ext.h"
#include "utils.h"
#include "terminal.h"
#include "draw.h"
#include "defs.h"
#include "x86.h"

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
			// just clear the screen.
			// we output everything to serial, so it records everything.
			// and we thus do not need something like scrolling
			TerminalClear();
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
	bootTerm.bgColor = BLACK;

	bootTerm.rowCount = modeInfo->height >> 4;
	bootTerm.colCount = modeInfo->width >> 3;

	gBootParam.BootService.TerminalService.TerminalClear = TerminalClear;
	gBootParam.BootService.TerminalService.TerminalPrintf = TerminalPrintf;
	gBootParam.BootService.TerminalService.TerminalWriteString = TerminalWriteString;
}

void TerminalSetColor(DWORD fgcolor, DWORD bgcolor)
{
	bootTerm.fgColor = fgcolor;
	bootTerm.bgColor = bgcolor;
}

void TerminalSetColorR(DWORD fgcolor, DWORD bgcolor,DWORD *oldfg,DWORD *oldbg)
{
	*oldfg = bootTerm.fgColor;
	*oldbg = bootTerm.bgColor;
	bootTerm.fgColor = fgcolor;
	bootTerm.bgColor = bgcolor;
}


void TerminalWriteCharacter(char c)
{
	// in qemu, this fucking cheesy way outputs bytes to serial even without any setups.
	outb(0x3f8, c);

	// check if c is displayable
	if (c == '\n')
	{
		outb(0x3f8, '\r'); // make serial output look better

		bootTerm.col = 0;
		bootTerm.row++;
		if (bootTerm.row >= bootTerm.rowCount)
		{
			bootTerm.row = 0;
		}
		return;
	}

	// print to screen
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
	FillScreen(R(bootTerm.bgColor), G(bootTerm.bgColor), B(bootTerm.bgColor));
}

static char hex[] = "0123456789abcdef";

static char buffer[64] = { 0 };

void TerminalVPrintf(char* format, VA_LIST args)
{
	MemSet(buffer, 0, sizeof(buffer));
	// Iterate through the format string
	for (int i = 0; format[i] != '\0'; ++i)
	{
		if (format[i] == '%')
		{
			++i;  // Move to the next character after '%'

			int j = i;
			while (format[j] == 'l')j++;

			SIZE_T width = 32;
			if (j - i == 2 && format[j - 1] == 'l' && format[j - 2] == 'l')
			{
				width = 64;
			}
			else if (j - i == 1 && format[j - 1] == 'l')
			{
				width = 32;
			}


			// Check for format specifiers
			if (format[j] == 'd')
			{
				MemSet(buffer, 0, width);

				// Handle %d (decimal integer)
				switch (width)
				{
				default:
				case 32:
				{
					int num = VA_ARG(args, int);
					Itoa(num, buffer, 10);
					TerminalWriteString(buffer);
					break;
				}
				case 64:
				{
					long long num = VA_ARG(args, long long);
					Lltoa(num, buffer, 10);
					TerminalWriteString(buffer);
					break;
				}
				}
			}
			else if (format[j] == 'x')
			{
				MemSet(buffer, 0, width);

				// Handle %x (hexadecimal integer, !unsigned according to C99 standard!)
				switch (width)
				{
				default:
				case 32:
				{
					unsigned int num = __builtin_va_arg(args, unsigned int);
					Utoa(num, buffer, 16);
					TerminalWriteString(buffer);
					break;
				}
				case 64:
				{
					unsigned long long num = __builtin_va_arg(args, unsigned long long);
					Ulltoa(num, buffer, 16);
					TerminalWriteString(buffer);
					break;
				}
				}
			}
			else if (format[j] == 'u')
			{
				MemSet(buffer, 0, width);

				// Handle %u (unsigned integer)
				switch (width)
				{
				default:
				case 32:
				{
					unsigned int num = VA_ARG(args, unsigned int);
					Utoa(num, buffer, 10);
					TerminalWriteString(buffer);
					break;
				}
				case 64:
				{
					unsigned long long num = VA_ARG(args, unsigned long long);
					Ulltoa(num, buffer, 10);
					TerminalWriteString(buffer);
					break;
				}
				}
			}
			else if (format[j] == 'p')
			{
				UINT_PTR num = VA_ARG(args, UINT_PTR);
				for (int j = 0; j < (sizeof(UINT_PTR) << 1); j++, num <<= 4)
				{
					TerminalWriteCharacter(hex[(num >> ((sizeof(UINT_PTR) << 3) - 4)) & 0xf]);
				}
			}
			else if (format[j] == 's')
			{
				// Handle %s (string)
				char* str = VA_ARG(args, char*);
				TerminalWriteString(str);
			}
		}
		else
		{
			// Regular character, not a format specifier
			TerminalWriteCharacter(format[i]);
		}
	}
}


void TerminalPrintf(char* format, ...)
{
	// Initialize the variable argument list using GCC's built-in macros
	__builtin_va_list args;
	__builtin_va_start(args, format);

	TerminalVPrintf(format, args);

	// Clean up the variable argument list
	__builtin_va_end(args);
}