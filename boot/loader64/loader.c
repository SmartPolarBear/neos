//
// Created by bear on 9/17/2023.
//
#include "loader.h"
#include "defs.h"
#include "fs.h"
#include "ext2.h"
#include "utils.h"
#include "elf.h"
#include "mem.h"
#include "param.h"
#include "terminal.h"

BYTE* loadMemory = (BYTE*)KERNEL_LOAD_ADDR;

static inline BYTE* ALWAYS_INLINE GetLoadAddr()
{
	return loadMemory;
}

static inline void AllocateLoadMemory(UINT_PTR size)
{
	loadMemory += size;
	loadMemory = (BYTE*)PGROUNDUP((UINT_PTR)loadMemory);
}


UINT_PTR LoadKernel()
{
	BYTE* binary = NULL;

	// NeosExecutive kernel binary
	SSIZE_T fileSize = BootFsLoadKernel(&binary);
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
	gBootParam.NeosExecutive.KernelLoadAddr = (UINT_PTR)GetLoadAddr();
	gBootParam.NeosExecutive.KernelMemSize = loadSize;
	gBootParam.NeosExecutive.KernelFileSize = fileSize;
	AllocateLoadMemory(loadSize);

	TerminalPrintf("Loaded kernel " KERNEL_PATH " (%d bytes in file, %d bytes in mem).\n", fileSize, loadSize);

	// NeosExecutive HAL
	fileSize = BootFsLoadModule(HAL_PATH, &binary);
	if (fileSize < 0)
	{
		Panic("Cannot load the HAL.");
	}
	BYTE* halLoadAddress = GetLoadAddr();
	loadSize = LoadModuleElf(binary, (UINT_PTR)halLoadAddress);
	if (loadSize < 0)
	{
		Panic("Invalid HAL format.");
	}
	gBootParam.NeosExecutive.HalLoadAddr = (UINT_PTR)halLoadAddress;
	gBootParam.NeosExecutive.HalOpAddr = LocateSymbolElf(binary, "HalOps");
	gBootParam.NeosExecutive.HalMemSize = loadSize;
	gBootParam.NeosExecutive.HalFileSize = fileSize;
	AllocateLoadMemory(loadSize);

	TerminalPrintf("Loaded HAL " HAL_PATH " (%d bytes, %d bytes in mem).\n", fileSize, loadSize);

	return kernEntryPoint;
}

void LoadDriver(const char* name)
{
	if (loadMemory < (BYTE*)DRIVER_LINK_ADDR)
	{
		loadMemory = (BYTE*)DRIVER_LINK_ADDR;
	}

	BYTE* binary = NULL;
	SSIZE_T size = BootFsLoadModule(name, &binary);
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