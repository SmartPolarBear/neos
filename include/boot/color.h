//
// Created by bear on 9/7/2023.
//

#pragma once

#include "type.h"

typedef DWORD COLOR;

#define R(ival) ((ival) & 0xff)
#define G(ival) (((ival) >> 8) & 0xff)
#define B(ival) (((ival) >> 16) & 0xff)

// Simple platte
#define BLACK 0x000000
#define WHITE 0xffffff
#define RED 0x0000FF
#define GREEN 0x00ff00
#define BLUE 0xff0000
#define YELLOW 0x00ffff
#define LIGHTGRAY 0xAAAAAA
#define NOCOLOR ((WORD)-1)