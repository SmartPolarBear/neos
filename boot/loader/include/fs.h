//
// Created by bear on 8/31/2023.
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

void InitializeBootFS();

INT LoadKernel(DWORD addr);
