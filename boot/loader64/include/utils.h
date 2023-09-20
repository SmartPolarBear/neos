//
// Created by bear on 9/11/2023.
//

#pragma once

#include "ext.h"
#include "type.h"


void Itoa(INT num, char* buffer, INT base);

INT MemCmp(const void* ptr1, const void* ptr2, DWORD size);

INT StrCmp(const char* str1, const char* str2);

void NO_RETURN Panic(char* msg);

#define MemSet __builtin_memset
#define MemCpy __builtin_memcpy
#define MemMove __builtin_memmove

