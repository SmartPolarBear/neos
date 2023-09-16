//
// Created by bear on 9/16/2023.
//

#pragma once

#include "type.h"
#include "boot/elf.h"

SSIZE_T LoadKernelElf(BYTE* binary);

SSIZE_T LoadModuleElf(BYTE* binary, UINT_PTR base);