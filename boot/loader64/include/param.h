//
// Created by bear on 9/17/2023.
//

#pragma once

#include "boot/param.h"

typedef union
{
	BOOTPARAM BootParam;
	BYTE BootParamBuf[4096];
} PACKED BOOTPARAMBUF;

extern BOOTPARAMBUF gBootParamBuf; // defined in boot\loader64\entry64.c

#define gBootParam gBootParamBuf.BootParam
