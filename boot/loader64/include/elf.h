//
// Created by bear on 9/16/2023.
//

#pragma once

#include "type.h"
#include "boot/elf.h"

SSIZE_T LoadKernelElf(IN BYTE* binary, OUT UINT_PTR* entry);

SSIZE_T LoadModuleElf(IN BYTE* binary, IN BYTE* kernBinary, IN BYTE* halBinary, IN UINT_PTR base, OUT UINT_PTR* entry);

ELFSYMBOL64* LocateSymbolElf(IN BYTE* binary, IN const char* name, ELFSYMBOLBINDING bind, ELFSYMBOLTYPE type);
