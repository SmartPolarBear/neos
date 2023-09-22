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

// Define constants for memory types
#define MEM_TYPE_USABLE 1
#define MEM_TYPE_RESERVED 2
#define MEM_TYPE_ACPI 3
#define MEM_TYPE_ACPI_NVS 4
#define MEM_TYPE_UNUSABLE 5
