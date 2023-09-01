//
// Created by bear on 8/31/2023.
//
#include "defs.h"
#include "fs.h"
#include "terminal.h"

PARTTABLEITEM* activePartition;

void InitializeBootFS()
{
	PARTTABLEITEM* partitions[] = {
			(PARTTABLEITEM*)(0x7c00 + 0x1be),
			(PARTTABLEITEM*)(0x7c00 + 0x1ce),
			(PARTTABLEITEM*)(0x7c00 + 0x1de),
			(PARTTABLEITEM*)(0x7c00 + 0x1ee)
	};
	for (int i = 0; i < 4; i++)
	{
		if (partitions[i]->BootIndicator != 0x80)
		{
			continue;
		}

		if (partitions[i]->SystemID != MBR_SYSID_LINUXNATIVE)
		{
			continue;
		}

		TerminalPrintf("Found bootable partition #%d.\n", i);
		activePartition = partitions[i];
	}
}