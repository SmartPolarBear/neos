//
// Created by bear on 9/7/2023.
//

#pragma once
#include "type.h"

#define CHAR_BITMAP(c) (((volatile BYTE*)VGA_FONT_ADDR)+(((WORD)c)<<4))
