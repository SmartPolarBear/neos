//
// Created by bear on 9/7/2023.
//

#include "defs.h"
#include "fs.h"
#include "ext2.h"
#include "utils.h"
#include "elf.h"
#include "mem.h"
#include "terminal.h"

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

UINT_PTR LoadKernel()
{
	BYTE* binary = NULL;

	// Load kernel binary
	SSIZE_T fileSize = fs->LoadKernel(activePartition, &binary);
	if (fileSize < 0)
	{
		Panic("Cannot load kernel.");
	}
	UINT_PTR kernEntryPoint = 0;
	SSIZE_T loadSize = LoadKernelElf(binary, &kernEntryPoint);
	if (loadSize < 0)
	{
		Panic("Invalid kernel format.");
	}
	loadMemory += loadSize;
	loadMemory = (BYTE*)PGROUNDUP((UINT_PTR)loadMemory);

	TerminalPrintf("Loaded kernel " KERNEL_PATH " (%d bytes in file, %d bytes in mem).\n", fileSize, loadSize);

	// Load HAL
	fileSize = fs->LoadModule(activePartition, HAL_PATH, &binary);
	if (fileSize < 0)
	{
		Panic("Cannot load the HAL.");
	}
	loadSize = LoadModuleElf(binary, (UINT_PTR)loadMemory);
	if (loadSize < 0)
	{
		Panic("Invalid HAL format.");
	}
	loadMemory += loadSize;
	loadMemory = (BYTE*)PGROUNDUP((UINT_PTR)loadMemory);

	TerminalPrintf("Loaded HAL " HAL_PATH " (%d bytes, %d bytes in mem).\n", fileSize, loadSize);

	return kernEntryPoint;
}

void LoadDriver(const char* name)
{
	BYTE* binary = NULL;
	SSIZE_T size = fs->LoadModule(activePartition, name, &binary);
	if (size < 0)
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

	TerminalPrintf("Loaded device driver %s (%d bytes).\n", name, size);
}