//
// Created by bear on 8/24/2023.
//

#pragma once

typedef void VOID;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned long long QWORD;

_Static_assert(sizeof(BYTE) == 1, "BYTE size is not 1");
_Static_assert(sizeof(WORD) == 2, "WORD size is not 2");
_Static_assert(sizeof(DWORD) == 4, "DWORD size is not 4");
_Static_assert(sizeof(QWORD) == 8, "QWORD size is not 8");

typedef int INT;
typedef unsigned int UINT;
typedef long LONG;
typedef unsigned long ULONG;
typedef long long LONGLONG;
typedef unsigned long long ULONGLONG;

typedef unsigned long long UINT_PTR;

typedef unsigned long long SIZE_T;
typedef long long SSIZE_T;

typedef long BOOL;
#define TRUE 1
#define FALSE 0


