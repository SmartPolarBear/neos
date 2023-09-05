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

	// read the neldr inode
	DWORD neldrBGDTIndex = (dirent->Inode - 1) / sb->InodesPerGroup;
	DWORD neldrInodeIndex = (dirent->Inode - 1) % sb->InodesPerGroup;
	DWORD neldrBlockIndex = (neldrInodeIndex * sb->InodeSize) / blockSize;

	//  groupDesc for neldr BGDT
	EXT2BGDT* neldrBGDT = groupDesc + neldrBGDTIndex;
	TerminalPrintf("neldr BGDT at index %d: %d, %d, %d, %d, %d, %d\n",
			neldrBGDTIndex, groupDesc->BlockUsageBitmap,
			groupDesc->InodeUsageBitmap, groupDesc->InodeTableStart, groupDesc->FreeBlockCount,
			groupDesc->FreeInodeCount, groupDesc->DirectoryCount);

	// read in the neldr inode table
	currentLBA = baseLBAOffset + (neldrBGDT->InodeTableStart - 1) * blockSec;
	ReadSects((void*)buffer, currentLBA, blockSec); // read the whole block
	EXT2INODE* neldrInode = (EXT2INODE*)(buffer + sizeof(EXT2INODE) * neldrInodeIndex);
	TerminalPrintf("neldr inode at LBA %d: 0x%x, %d, %d, %d, %d, %d\n",
			currentLBA, neldrInode->TypeAndPerms,
			neldrInode->UserID, neldrInode->SizeLower, neldrInode->LastAccessTime,
			neldrInode->CreationTime, neldrInode->LastModificationTime);

	const DWORD neldrSize = neldrInode->SizeLower; //  neldr size must be less than 4GB
	const DWORD neldrBlockCount = neldrSize / blockSize + 1;

	// read in the neldr blocks
	// TODO: read to buffer and copy to addr.!! First investigate the mkdisk tool.
	for (DWORD i = 0; i < neldrBlockCount; i++)
	{
		if (i < 12)
		{

		}
		else if(i < 12 + blockSec / sizeof(DWORD))
		{

		}
		else if (i < 12 + blockSec / sizeof(DWORD) + blockSec / sizeof(DWORD) * blockSec / sizeof(DWORD))
		{

		}
		else
		{
			TerminalSetColor(RED, BLUE);
			TerminalWriteString("neldr too large.\n");
			return -1;
		}
	}


	for (;;);
	return (INT)neldrSize;
}