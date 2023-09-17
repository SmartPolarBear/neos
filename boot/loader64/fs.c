//
// Created by bear on 9/7/2023.
//

#include "defs.h"
#include "fs.h"
#include "ext2.h"
#include "utils.h"
#include "elf.h"
#include "mem.h"
#include "param.h"
#include "terminal.h"

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

SSIZE_T BootFsLoadKernel(OUT VOID* buf)
{
	return fs->LoadKernel(activePartition, buf);
}

SSIZE_T BootFsLoadModule(const char* name, OUT VOID* buf)
{
	return fs->LoadModule(activePartition, name, buf);
}