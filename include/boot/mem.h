//
// Created by bear on 9/7/2023.
//

#pragma once
#include "type.h"
#include "ext.h"

typedef struct
{
	DWORD BaseAddrLow;
	DWORD BaseAddrHigh;
	DWORD LengthLow;
	DWORD LengthHigh;
	DWORD Type;
} PACKED E820MEMMAP;
