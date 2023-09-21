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

BYTE* kernBin = NULL;
BYTE* halBin = NULL;

UINT_PTR LoadKernel()
{
	// NeosExecutive kernel
	SSIZE_T fileSize = BootFsLoadKernel(&kernBin);
	if (fileSize < 0)
	{
		Panic("Cannot load kernel.");
	}
	UINT_PTR kernEntryPoint = 0;
	SSIZE_T loadSize = LoadKernelElf(kernBin, &kernEntryPoint);
	if (loadSize < 0)
	{
		Panic("Invalid kernel format.");
	}
	gBootParam.NeosExecutive.KernelLoadAddr = (UINT_PTR)GetLoadAddr();
	gBootParam.NeosExecutive.KernelMemSize = loadSize;
	gBootParam.NeosExecutive.KernelFileSize = fileSize;
	AllocateLoadMemory(loadSize);

	TerminalPrintf("Loaded kernel " KERNEL_PATH " (%d bytes in file, %d bytes in mem).\n", fileSize, loadSize);

	return kernEntryPoint;
}

void LoadHal()
{
	if (!kernBin)
	{
		Panic("Cannot load HAL: kernel is not loaded.");
	}

	// NeosExecutive HAL
	SSIZE_T halFileSize = BootFsLoadModule(HAL_PATH, &halBin);
	if (halFileSize < 0)
	{
		Panic("Cannot load the HAL.");
	}
	BYTE* halLoadAddress = GetLoadAddr();
	UINT_PTR halEntryPoint = 0;
	SSIZE_T loadSize = LoadModuleElf(halBin, kernBin, NULL, (UINT_PTR)halLoadAddress, &halEntryPoint);
	if (loadSize < 0)
	{
		Panic("Invalid HAL format.");
	}
	ELFSYMBOL64* halOpsSymbol = LocateSymbolElf(halBin, "HalOps", STB_GLOBAL, STT_OBJECT);
	if (!halOpsSymbol)
	{
		Panic("Invalid HAL format: HalOps is missing.");
	}
	gBootParam.NeosExecutive.HalLoadAddr = (UINT_PTR)halLoadAddress;
	gBootParam.NeosExecutive.HalOpAddr = halOpsSymbol->Value;
	gBootParam.NeosExecutive.HalEntryAddr = halEntryPoint;
	gBootParam.NeosExecutive.HalMemSize = loadSize;
	gBootParam.NeosExecutive.HalFileSize = halFileSize;
	AllocateLoadMemory(loadSize);

	TerminalPrintf("Loaded HAL " HAL_PATH " (%d bytes, %d bytes in mem). HALOPS is at %p. Hal entry point is at %p.\n",
			halFileSize, loadSize,
			halOpsSymbol->Value,
			halEntryPoint);
}

void LoadDriver(const char* name)
{
	if (!kernBin)
	{
		Panic("Cannot load device driver: kernel is not loaded.");
	}

	if (!halBin)
	{
		Panic("Cannot load device driver: HAL is not loaded.");
	}

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
	UINT_PTR driverEntryPoint = 0;
	SSIZE_T ret = LoadModuleElf(binary, kernBin, halBin, (UINT_PTR)loadMemory, &driverEntryPoint);
	if (ret < 0)
	{
		Panic("Invalid driver executable format.");
	}
	loadMemory += ret;
	loadMemory = (BYTE*)PGROUNDUP((UINT_PTR)loadMemory);

	TerminalPrintf("Loaded device driver %s (%d bytes).\n", name, size);
}