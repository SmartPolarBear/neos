//
// Created by bear on 9/16/2023.
//


#pragma once

#include "type.h"
#include "defs.h"

#define SEC_SIZE 512

void ReadSects(void* dst, DWORD secno, DWORD secs);