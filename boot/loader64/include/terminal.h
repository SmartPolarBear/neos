//
// Created by bear on 9/11/2023.
//

#pragma once

#include "type.h"
#include "defs.h"

void InitializeTerminal();

void TerminalSetColor(DWORD fgcolor, DWORD bgcolor);
void TerminalSetColorR(DWORD fgcolor, DWORD bgcolor,DWORD *oldfg,DWORD *oldbg);

void TerminalWriteCharacter(char c);

void TerminalWriteString(char* str);

void TerminalClear();

void TerminalMoveCursor(INT x, INT y);

void TerminalVPrintf(char* format, VA_LIST args);

void TerminalPrintf(char* format, ...);

