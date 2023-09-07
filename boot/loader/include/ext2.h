//
// Created by bear on 9/2/2023.
//

#pragma once

#include "fs.h"
#include "ext.h"
#include "boot/ext2.h"

INT LoadKernelExt2(DWORD addr, EXT2SB* sb, PARTTABLEITEM* part, char* buffer);