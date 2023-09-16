//
// Created by bear on 9/7/2023.
//

#pragma once

#include "ext.h"
#include "type.h"

typedef struct
{
	BYTE BootIndicator;
	BYTE StartHead;
	BYTE StartSector:6;
	WORD StartCylinder:10;
	BYTE SystemID;
	BYTE EndHead;
	BYTE EndSector:6;
	WORD EndCylinder:10;
	DWORD RelativeSector;
	DWORD TotalSector;
} PACKED PARTTABLEITEM;

#define MBR_SYSID_LINUXNATIVE 0x83