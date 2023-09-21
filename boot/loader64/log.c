//
// Created by bear on 9/21/2023.
//
#include "log.h"
#include "draw.h"
#include "terminal.h"

void LogDebug(char* format, ...)
{
#ifdef  ENABLE_LOG
	__builtin_va_list args;
	__builtin_va_start(args, format);

	TerminalWriteString("[Debug] ");
	TerminalVPrintf(format, args);

	// Clean up the variable argument list
	__builtin_va_end(args);
#endif
}

void LogWarning(char* format, ...)
{
#ifdef ENABLE_LOG
	__builtin_va_list args;
	__builtin_va_start(args, format);

	DWORD oldfg = 0, oldbg = 0;
	TerminalSetColorR(YELLOW, BLACK, &oldfg, &oldbg);

	TerminalWriteString("[Warning] ");
	TerminalVPrintf(format, args);

	TerminalSetColor(oldfg, oldbg);

	// Clean up the variable argument list
	__builtin_va_end(args);
#endif
}