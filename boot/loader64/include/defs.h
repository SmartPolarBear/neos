//
// Created by bear on 9/9/2023.
//

#pragma once

#include "boot/defs.h"

#define IDENTITY_PAGING_BASE BUFFER_START
#define IDENTITY_PAGING_SIZE 0x1000 * 6

#define ROUNDUP(val, alignment) (((val) + ((alignment) - 1)) & ~((alignment) - 1))