//
// Created by bear on 8/27/2023.
//

#pragma once

#include "type.h"

// 16bit
void PutPixel16(DWORD color, INT x, INT y);

void PutRect16(INT w, INT h, BYTE r, BYTE g, BYTE b);

// 32bit
void PutPixel32(DWORD color, INT x, INT y);

void PutRect32(INT w, INT h, BYTE r, BYTE g, BYTE b);