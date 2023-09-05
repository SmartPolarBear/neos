//
// Created by bear on 8/31/2023.
//
#include "entry.h"
#include "defs.h"
#include "fs.h"
#include "ext2.h"
#include "terminal.h"
#include "disk.h"

PARTTABLEITEM* activePartition = NULL;

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

	if (activePartition == NULL)
	{
		TerminalWriteString("No bootable partition found.\n");
		BootPanic();
	}
}

static inline INT ALWAYS_INLINE LoadKernelLinuxNative(DWORD addr)
{
	// Read the superblock
	char* buffer = (char*)FS_BUFFER_START;

	EXT2SB* superBlock = (EXT2SB*)buffer;
	buffer += sizeof(EXT2SB);
	ReadSects((void*)superBlock, activePartition->RelativeSector + 2/*2 reserved sector*/, 2);

	if (superBlock->Magic != EXT2_SB_MAGIC)
	{
		TerminalPrintf("Invalid superblock magic: %x\n", superBlock->Magic);
		return -1;
	}

	// dump basic info
	TerminalWriteString("EXT2 filesystem detected.\n");
	TerminalPrintf("Inode count: %d, ", superBlock->InodeCount);
	TerminalPrintf("Block count: %d, ", superBlock->BlockCount);
	TerminalPrintf("Reserved block count: %d, ", superBlock->ReservedBlockCount);
	TerminalPrintf("Free block count: %d, ", superBlock->FreeBlockCount);
	TerminalPrintf("Free inode count: %d, ", superBlock->FreeInodeCount);
	TerminalPrintf("First data block: %d. ", superBlock->FirstDataBlock);
	TerminalPrintf("Block size: %d, ", 1024 << superBlock->BlockSize);
	TerminalPrintf("Fragment size: %d, ", 1024 << superBlock->FragmentSize);
	TerminalPrintf("Blocks per group: %d. \n", superBlock->BlocksPerGroup);

	return LoadKernelExt2(addr, superBlock, activePartition, buffer);
}

INT LoadKernel(DWORD addr)
{
	switch (activePartition->SystemID)
	{
	case MBR_SYSID_LINUXNATIVE:
		return LoadKernelLinuxNative(addr);
	default:
		TerminalPrintf("Unsupported partition type %d.\n", activePartition->SystemID);
	}
	return -1;
}

