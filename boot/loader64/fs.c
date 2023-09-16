//
// Created by bear on 9/7/2023.
//

#include "defs.h"
#include "fs.h"
#include "ext2.h"
#include "utils.h"

PARTTABLEITEM* activePartition = NULL;

BOOTFS* fs = NULL;

void InitializeBootFs(PARTTABLEITEM* ap)
{
	activePartition = ap;
	switch (ap->SystemID)
	{
	case MBR_SYSID_LINUXNATIVE:
		fs = &ext2Fs;
		break;
	default:
		Panic("Unknown file system");
	}

	fs->Initialize(ap);
}

SSIZE_T LoadKernel()
{
	return fs->LoadKernel(activePartition);
}

SSIZE_T LoadDriver(const char* name)
{
	return fs->LoadDriver(activePartition, name);
}