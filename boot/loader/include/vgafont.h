//
// Created by bear on 8/27/2023.
//

#pragma once
#include "boot/color.h"
#include "boot/vgafont.h"

void DrawCharacter(char c, INT x, INT y, COLOR fgcolor);

void DrawCharacterBackDrop(char c, INT x, INT y, COLOR fgcolor, COLOR bgcolor);

void DrawString(char* str, INT x, INT y, COLOR fgcolor);

void DrawStringBackDrop(char* str, INT x, INT y, COLOR fgcolor, COLOR bgcolor);
