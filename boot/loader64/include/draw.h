//
// Created by bear on 9/7/2023.
//

#pragma once

#include "boot/defs.h"
#include "boot/vbe.h"
#include "boot/color.h"
#include "boot/vgafont.h"


void FillScreen(BYTE r, BYTE g, BYTE b);
void DrawPixel(INT x, INT y, BYTE r, BYTE g, BYTE b);
void DrawRect(INT x, INT y, INT w, INT h, BYTE r, BYTE g, BYTE b);
void DrawCharacterBackDrop(char c, int x, int y, COLOR fgcolor, COLOR bgcolor);
void DrawCharacter(char c, int x, int y, COLOR fgcolor);
void DrawString(char* str, int x, int y, COLOR fgcolor);
void DrawStringBackDrop(char* str, int x, int y, COLOR fgcolor, COLOR bgcolor);