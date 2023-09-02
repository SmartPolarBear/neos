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

// Custom implementation of itoa (integer to ASCII) function
// Converts an integer to its ASCII representation
static inline ALWAYS_INLINE void Itoa(int num, char* buffer, int base)
{
	int i = 0;
	int isNegative = 0;

	// Handle negative numbers for base 10
	if (num < 0 && base == 10)
	{
		isNegative = 1;
		num = -num;
	}

	// Handle the special case of 0
	if (num == 0)
	{
		buffer[i++] = '0';
	}
	else
	{
		while (num != 0)
		{
			int remainder = num % base;
			buffer[i++] = (remainder < 10) ? (remainder + '0') : (remainder - 10 + 'a');
			num /= base;
		}
	}

	// Add the negative sign for base 10 if necessary
	if (isNegative && base == 10)
	{
		buffer[i++] = '-';
	}

	// Reverse the string
	int left = 0;
	int right = i - 1;
	while (left < right)
	{
		char temp = buffer[left];
		buffer[left] = buffer[right];
		buffer[right] = temp;
		left++;
		right--;
	}

	// Null-terminate the string
	buffer[i] = '\0';
}

void TerminalPrintf(char* format, ...)
{
	// Initialize the variable argument list using GCC's built-in macros
	__builtin_va_list args;
	__builtin_va_start(args, format);

	// Iterate through the format string
	for (int i = 0; format[i] != '\0'; ++i)
	{
		if (format[i] == '%')
		{
			++i;  // Move to the next character after '%'

			// Check for format specifiers
			if (format[i] == 'd')
			{
				// Handle %d (decimal integer)
				int num = __builtin_va_arg(args, int);
				char buffer[32];  // Adjust the size as needed for your environment
				Itoa(num, buffer, 10);
				TerminalWriteString(buffer);
			}
			else if (format[i] == 'x')
			{
				// Handle %x (hexadecimal integer)
				int num = __builtin_va_arg(args, int);
				char buffer[32];  // Adjust the size as needed for your environment
				Itoa(num, buffer, 16);
				TerminalWriteString(buffer);
			}
			else if (format[i] == 's')
			{
				// Handle %s (string)
				char* str = __builtin_va_arg(args, char*);
				TerminalWriteString(str);
			}
		}
		else
		{
			// Regular character, not a format specifier
			TerminalWriteCharacter(format[i]);
		}
	}

	// Clean up the variable argument list
	__builtin_va_end(args);
}