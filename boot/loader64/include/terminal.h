//
// Created by bear on 9/11/2023.
//

#pragma once

#include "type.h"

void InitializeTerminal();

void TerminalSetColor(DWORD fgcolor, DWORD bgcolor);

void TerminalWriteCharacter(char c);

void TerminalWriteString(char* str);

void TerminalClear();

void TerminalPrintf(char* format, ...);

void TerminalMoveCursor(INT x, INT y);
