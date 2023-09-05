//
// Created by bear on 9/3/2023.
//
#include "defs.h"
#include "ext2.h"
#include "disk.h"
#include "terminal.h"
#include "draw.h"
#include "utils.h"

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
	currentLBA = baseLBAOffset + (groupDesc->InodeTableStart - 1) * blockSec;
	ReadSects((void*)buffer, currentLBA, blockSec); // read the whole block
	EXT2INODE* rootInode = (EXT2INODE*)(buffer + sizeof(EXT2INODE)); // skip inode 1 to get inode 2
	TerminalPrintf("Root inode at LBA %d: 0x%x, %d, %d, %d, %d, %d\n",
			currentLBA, rootInode->TypeAndPerms,
			rootInode->UserID, rootInode->SizeLower, rootInode->LastAccessTime,
			rootInode->CreationTime, rootInode->LastModificationTime);
	buffer += blockSize;

	if ((rootInode->TypeAndPerms & 0x4000) == 0)
	{
		TerminalSetColor(RED, BLUE);
		TerminalWriteString("Root inode is not a directory.\n");
		return -1;
	}

	// We assume the neldr should present in direct blocks
	EXT2DIRENT* dirent = NULL;
	for (INT i = 0; i < 12; i++)
	{
		DWORD block = rootInode->DirectBlockPointers[i];
		char* blockBuffer = buffer; // we use the same buffer
		currentLBA = baseLBAOffset + (block - 1) * blockSec;
		ReadSects((void*)blockBuffer, currentLBA, blockSec);
		char* p = blockBuffer;
		while (p - blockBuffer < blockSize)
		{
			dirent = (EXT2DIRENT*)p;
			if (dirent->Inode == 0)
			{
				break;
			}
			if (dirent->FileType == EXT2DIRENT_TYPE_REGULAR)
			{
				if (MemCmp(dirent->Name, "neldr", 5) == 0)
				{
					TerminalPrintf("Found neldr on Inode %d\n", dirent->Inode);
					// read the neldr
					currentLBA = baseLBAOffset + (dirent->Inode - 1) * blockSec;
					ReadSects((void*)addr, currentLBA, blockSec);
					goto found;
				}
			}
			p += dirent->RecordLength;
		}
	}
	TerminalSetColor(RED, BLUE);
	TerminalWriteString("neldr not found.\n");
	return -1; // Not found
found:
	buffer += blockSize;

	for (;;);

	return 0;
}