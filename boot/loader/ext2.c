//
// Created by bear on 9/3/2023.
//
#include "defs.h"
#include "entry.h"
#include "ext2.h"
#include "disk.h"
#include "terminal.h"
#include "draw.h"
#include "utils.h"

#define BLOCK_OFFSET(block) (BASE_OFFSET+(block-1)*blockSize)

char* globalBuffer;

static inline char* ALWAYS_INLINE AllocateBuffer(DWORD sizeInBytes)
{
	char* ret = globalBuffer;
	globalBuffer += sizeInBytes;
	return ret;
}

INT LoadKernelExt2(DWORD addr, EXT2SB* sb, PARTTABLEITEM* part, char* buf)
{
	globalBuffer = buf;

	const DWORD BG_COUNT = (sb->BlockCount / sb->BlocksPerGroup) + 1;
	const DWORD BLOCK_SIZE = 1024 << sb->BlockSize;
	const DWORD BLOCK_SECS = BLOCK_SIZE / SEC_SIZE;
	const DWORD BASE_LBA = part->RelativeSector + 2; // 2 reserved sectors

	TerminalPrintf("Base LBA offset: %d\n", BASE_LBA);
	DWORD currentLBA = BASE_LBA;

	// read group descriptor
	currentLBA += BLOCK_SECS; // skip superblock
	EXT2BGDT* groupDesc = (EXT2BGDT*)AllocateBuffer(sizeof(EXT2BGDT) * BG_COUNT);
	ReadSects((void*)groupDesc, currentLBA, 2);
	TerminalPrintf("Group descriptor at LBA %d: %d, %d, %d, %d, %d, %d\n",
			currentLBA, groupDesc->BlockUsageBitmap,
			groupDesc->InodeUsageBitmap, groupDesc->InodeTableStart, groupDesc->FreeBlockCount,
			groupDesc->FreeInodeCount, groupDesc->DirectoryCount);

	// read inode 2 (root directory)
	currentLBA = BASE_LBA + (groupDesc->InodeTableStart - 1) * BLOCK_SECS;
	// Read the Inode table.
	// Because inode id is 2, so it must locate in the first block
	char* inodeTable = AllocateBuffer(BLOCK_SIZE);
	ReadSects((void*)inodeTable, currentLBA, BLOCK_SECS);
	EXT2INODE* rootInode = (EXT2INODE*)(inodeTable + sizeof(EXT2INODE)); // skip inode 1 to get inode 2
	TerminalPrintf("Root inode at LBA %d: 0x%x, %d, %d, %d, %d, %d\n",
			currentLBA, rootInode->TypeAndPerms,
			rootInode->UserID, rootInode->SizeLower, rootInode->LastAccessTime,
			rootInode->CreationTime, rootInode->LastModificationTime);

	if ((rootInode->TypeAndPerms & 0x4000) == 0)
	{
		TerminalSetColor(RED, BLUE);
		TerminalWriteString("Root inode is not a directory.\n");
		return -1;
	}

	// We assume the neldr should present in direct blocks of root directory inode
	EXT2DIRENT* dirent = NULL;
	for (INT i = 0; i < 12; i++)
	{
		DWORD block = rootInode->DirectBlockPointers[i];
		char* blockBuffer = (char*)AllocateBuffer(BLOCK_SIZE); // we use the same buffer
		currentLBA = BASE_LBA + (block - 1) * BLOCK_SECS;
		ReadSects((void*)blockBuffer, currentLBA, BLOCK_SECS);
		char* p = blockBuffer;
		while (p - blockBuffer < BLOCK_SIZE)
		{
			dirent = (EXT2DIRENT*)p;
			if (dirent->Inode == 0 || dirent->RecordLength == 0)
			{
				break;
			}
			if (dirent->FileType == EXT2DIRENT_TYPE_REGULAR)
			{
				if (MemCmp(dirent->Name, "neldr", 5) == 0)
				{
					goto Found;
				}
			}
			p += dirent->RecordLength;
		}
	}
	TerminalSetColor(RED, BLUE);
	TerminalWriteString("neldr not found.\n");
	return -1; // Not found
Found:
	TerminalPrintf("Found neldr Inode #%d\n", dirent->Inode);

	// read the neldr inode
	DWORD neldrBGDTIndex = (dirent->Inode - 1) / sb->InodesPerGroup;
	DWORD neldrInodeIndex = (dirent->Inode - 1) % sb->InodesPerGroup;
	DWORD neldrBlockIndex = (neldrInodeIndex * sb->InodeSize) / BLOCK_SIZE;

	//  groupDesc for neldr BGDT
	EXT2BGDT* neldrBGDT = groupDesc + neldrBGDTIndex;
	TerminalPrintf("neldr BGDT at index %d: 0x%x, 0x%x, %d, %d, %d, %d\n",
			neldrBGDTIndex, groupDesc->BlockUsageBitmap,
			groupDesc->InodeUsageBitmap, groupDesc->InodeTableStart, groupDesc->FreeBlockCount,
			groupDesc->FreeInodeCount, groupDesc->DirectoryCount);

	// read in the neldr inode table
	const DWORD inodeTableLenSec = (sb->InodeSize * sb->InodesPerGroup / SEC_SIZE) + 1;
	currentLBA = BASE_LBA + (neldrBGDT->InodeTableStart - 1) * BLOCK_SECS;
	// This time cannot only read first block because neldr inode may locate in arbitrary block
	char* fullInodeTable = AllocateBuffer(inodeTableLenSec * SEC_SIZE);
	ReadSects((void*)fullInodeTable, currentLBA, inodeTableLenSec); // read the whole Inode list
	EXT2INODE* neldrInode = (EXT2INODE*)(fullInodeTable + sb->InodeSize * neldrInodeIndex);
	const DWORD neldrSize = neldrInode->SizeLower; //  neldr size must be less than 4GB, no size higher.
	const DWORD neldrBlockCount = neldrSize / BLOCK_SIZE + 1;

	TerminalPrintf("neldr inode at LBA %d: type 0x%x, size %d, bc %d\n",
			currentLBA, neldrInode->TypeAndPerms, neldrInode->SizeLower, neldrBlockCount);

	// Read in the neldr blocks.
	// We assume only direct and l1 indirect blocks are used.
	// Or we will report error
	const DWORD MAX_SUPPORT_BC = 12 + BLOCK_SIZE / sizeof(DWORD);
	if (neldrBlockCount >= MAX_SUPPORT_BC)
	{
		TerminalSetColor(RED, BLUE);
		TerminalPrintf("neldr of %d blocks is too large (%d at max).\n", neldrBlockCount, MAX_SUPPORT_BC);
		BootPanic();
	}

	for (DWORD i = 0; i < 12 && i < neldrBlockCount; i++)
	{
		DWORD block = neldrInode->DirectBlockPointers[i];
		currentLBA = BASE_LBA + (block - 1) * BLOCK_SECS;
		ReadSects((void*)addr, currentLBA, BLOCK_SECS);
		addr += BLOCK_SIZE;
	}

	if (neldrBlockCount < 12)
	{
		goto Finish;
	}

	// l1-indirect
	currentLBA = BASE_LBA + (neldrInode->SinglyIndirectBlockPointer - 1) * BLOCK_SECS;
	DWORD* indirectBlocks = (DWORD*)AllocateBuffer(BLOCK_SIZE);
	ReadSects((void*)indirectBlocks, currentLBA, BLOCK_SECS);
	for (INT i = 12; i < neldrBlockCount; i++)
	{
		DWORD block = indirectBlocks[i - 12];
		currentLBA = BASE_LBA + (block - 1) * BLOCK_SECS;
		ReadSects((void*)addr, currentLBA, BLOCK_SECS);
		addr += BLOCK_SIZE;
	}

Finish:
	return (INT)neldrSize;
}