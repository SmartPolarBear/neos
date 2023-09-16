//
// Created by bear on 9/7/2023.
//

#include "defs.h"
#include "fs.h"
#include "ext2.h"
#include "utils.h"
#include "elf.h"
#include "mem.h"

PARTTABLEITEM* activePartition = NULL;

BOOTFS* fs = NULL;
BYTE* loadMemory = (BYTE*)KERNEL_LOAD_ADDR;

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

void LoadKernel()
{
	BYTE* binary = NULL;
	SSIZE_T size = fs->LoadKernel(activePartition, &binary);
	if (size < 0)
	{
		Panic("Cannot load kernel.");
	}
	SSIZE_T ret = LoadKernelElf(binary);
	if (ret < 0)
	{
		Panic("Invalid kernel format.");
	}
	loadMemory += ret;
	loadMemory = (BYTE*)PGROUNDUP((UINT_PTR)loadMemory);
}

void LoadDriver(const char* name)
{
	BYTE* binary = NULL;
	fs->LoadDriver(activePartition, name, &binary);
	if (!binary)
	{
		Panic("Cannot load device driver.");
	}
	SSIZE_T ret = LoadModuleElf(binary, (UINT_PTR)loadMemory);
	if (ret < 0)
	{
		Panic("Invalid driver executable format.");
	}
	loadMemory += ret;
	loadMemory = (BYTE*)PGROUNDUP((UINT_PTR)loadMemory);
}