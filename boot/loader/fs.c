//
// Created by bear on 8/31/2023.
//
#include "fs.h"
#include "terminal.h"

PARTTABLEITEM* activePartition;

void InitializeBootFS()
{
	for (PARTTABLEITEM* p = (PARTTABLEITEM*)(0x7c00 + 0x1be); p < (PARTTABLEITEM*)(0x7c00 + 0x1be + 0x40); p++)
	{

	}
}