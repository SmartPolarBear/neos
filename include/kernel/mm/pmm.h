//
// Created by bear on 9/21/2023.
//

#pragma once

#include "ext.h"
#include "type.h"

typedef struct
{
	DWORD Flags;
	DWORD Ref;
	BYTE Private[24]; // for things like buddy allocator, page replacement, etc.
} PACKED PHYSICALPAGE;
_Static_assert(sizeof(PHYSICALPAGE) == 32, "PHYSICALPAGE size mismatch");

typedef enum
{
	PPF_NONE = 0x0,
	PPF_RESERVED = 0x1,
	PPF_ACPI = 0x2,
	PPF_RECLAIMABLE = 0x4,
	PPF_HAS_PROPERTY = 0x8,
} PHYSPAGEFLAGS;
