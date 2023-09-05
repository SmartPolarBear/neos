//
// Created by bear on 9/3/2023.
//
#include "defs.h"
#include "ext2.h"
#include "disk.h"
#include "terminal.h"

#define BLOCK_OFFSET(block) (BASE_OFFSET+(block-1)*blockSize)


INT LoadKernelExt2(DWORD addr, EXT2SB* sb, PARTTABLEITEM* part, char* buffer)
{
	const DWORD blockSize = 1024 << sb->BlockSize;
	const DWORD blockSec = blockSize / SEC_SIZE;

	const DWORD baseLBAOffset = part->RelativeSector + 2; // 2 reserved sectors
	TerminalPrintf("Base LBA offset: %d\n", baseLBAOffset);
	DWORD currentLBA = baseLBAOffset;

	// read group descriptor
	currentLBA += blockSec; // skip superblock
	EXT2BGDT* groupDesc = (EXT2BGDT*)buffer;
	ReadSects((void*)groupDesc, currentLBA, 2);
	TerminalPrintf("Group descriptor at LBA %d: %d, %d, %d, %d, %d, %d\n",
			currentLBA, groupDesc->BlockUsageBitmap,
			groupDesc->InodeUsageBitmap, groupDesc->InodeTableStart, groupDesc->FreeBlockCount,
			groupDesc->FreeInodeCount, groupDesc->DirectoryCount);
	buffer += sizeof(EXT2BGDT);

	// read inode 2 (root directory)
	currentLBA = baseLBAOffset + groupDesc->InodeTableStart * blockSec;



	EXT2INODE* rootInode = (EXT2INODE*)buffer;
	ReadSects((void*)rootInode, 2050, 2);
	TerminalPrintf("Root inode at LBA %d: 0x%x, %d, %d, %d, %d, %d\n",
			currentLBA, rootInode->TypeAndPerms,
			rootInode->UserID, rootInode->SizeLower, rootInode->LastAccessTime,
			rootInode->CreationTime, rootInode->LastModificationTime);
	buffer += sizeof(EXT2INODE);

	for (;;);

	return 0;
}