//
// Created by bear on 8/31/2023.
//

#pragma once

#include "type.h"

void InitializeTerminal();

void TerminalSetColor(DWORD fgcolor, DWORD bgcolor);

void TerminalWriteCharacter(char c);

void TerminalWriteString(char* str);

void TerminalMoveCursor(INT x, INT y);

void TerminalClear();
