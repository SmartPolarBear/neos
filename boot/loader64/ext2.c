//
// Created by bear on 9/7/2023.
//
#include "defs.h"
#include "fs.h"
#include "ext2.h"
#include "mem.h"
#include "disk.h"
#include "log.h"
#include "terminal.h"
#include "utils.h"
#include "error.h"

EXT2SB superBlock;
EXT2BGDT* groupDescs;
EXT2DIRENT* rootDir = NULL;
BYTE* l1IndirectBuf = NULL, * l2IndirectBuf = NULL;
BYTE* dirBuf = NULL;
BYTE* inodeTableBuf = NULL;


DWORD BG_COUNT = 0;
DWORD BLOCK_SIZE = 0;
DWORD BLOCK_SECS = 0;
DWORD BASE_LBA = 0;

static inline void DebugDumpInode(EXT2INODE* node)
{
	LogDebug("\nInode: 0x%x, %d, %d, %d, %d, %d\n",
			node->TypeAndPerms,
			node->UserID, node->SizeLower, node->LastAccessTime,
			node->CreationTime, node->LastModificationTime);
}

static SSIZE_T ReadInode(EXT2INODE* inode, BYTE* buf)
{
	SSIZE_T result = 0;
	char* p = buf;
	const SIZE_T blockCount = inode->SizeLower / BLOCK_SIZE + 1;

	// direct blocks
	for (int i = 0; i < 12; i++)
	{
		if (inode->DirectBlockPointers[i] == 0)
		{
			break;
		}
		ReadSects(p, BASE_LBA + (inode->DirectBlockPointers[i] - 1) * BLOCK_SECS, BLOCK_SECS);
		p += BLOCK_SIZE;
		result += BLOCK_SIZE;
	}

	if (blockCount <= 12)
	{
		goto Finish;
	}

	// l1 indirect blocks
	ReadSects(l1IndirectBuf, BASE_LBA + (inode->SinglyIndirectBlockPointer - 1) * BLOCK_SECS, BLOCK_SECS);
	for (int i = 0; i < BLOCK_SIZE / sizeof(DWORD); i++)
	{
		if (((DWORD*)l1IndirectBuf)[i] == 0)
		{
			break;
		}
		ReadSects(p, BASE_LBA + (((DWORD*)l1IndirectBuf)[i] - 1) * BLOCK_SECS, BLOCK_SECS);
		p += BLOCK_SIZE;
		result += BLOCK_SIZE;
	}

	if (blockCount <= 12 + BLOCK_SIZE / sizeof(DWORD))
	{
		goto Finish;
	}

	// l2 indirect blocks
	ReadSects(l2IndirectBuf, BASE_LBA + (inode->DoublyIndirectBlockPointer - 1) * BLOCK_SECS, BLOCK_SECS);
	for (int i = 0; i < BLOCK_SIZE / sizeof(DWORD); i++)
	{
		if (((DWORD*)l2IndirectBuf)[i] == 0)
		{
			break;
		}
		ReadSects(l1IndirectBuf, BASE_LBA + (((DWORD*)l2IndirectBuf)[i] - 1) * BLOCK_SECS, BLOCK_SECS);
		for (int j = 0; j < BLOCK_SIZE / sizeof(DWORD); j++)
		{
			if (((DWORD*)l1IndirectBuf)[j] == 0)
			{
				break;
			}
			ReadSects(p, BASE_LBA + (((DWORD*)l1IndirectBuf)[j] - 1) * BLOCK_SECS, BLOCK_SECS);
			p += BLOCK_SIZE;
			result += BLOCK_SIZE;
		}
	}

Finish:
	return result;
}


void InitializeExt2()
{
	ReadSects((void*)&superBlock, activePartition->RelativeSector + 2/*2 reserved sector*/, 2);

	if (superBlock.Magic != EXT2_SB_MAGIC)
	{
		Panic("Invalid EXT2 superblock magic %d", superBlock.Magic);
	}

	BG_COUNT = (superBlock.BlockCount / superBlock.BlocksPerGroup) + 1;
	BLOCK_SIZE = 1024 << superBlock.BlockSize;
	BLOCK_SECS = BLOCK_SIZE / SEC_SIZE;
	BASE_LBA = activePartition->RelativeSector + 2; // 2 reserved sectors

	groupDescs = AllocateLowBytes(sizeof(EXT2BGDT) * BG_COUNT);
	if (groupDescs == NULL)
	{
		Panic("Cannot allocate memory for group descriptors");
	}

	const SIZE_T groupDescSecs = (sizeof(EXT2BGDT) * BG_COUNT + BLOCK_SIZE - 1) / BLOCK_SIZE;
	ReadSects((void*)groupDescs, BASE_LBA + BLOCK_SECS, groupDescSecs);

	inodeTableBuf = AllocateLowBytes(BLOCK_SIZE);
	if (inodeTableBuf == NULL)
	{
		Panic("Cannot allocate memory for inode table");
	}

	const DWORD inodeTableAddr = BASE_LBA + (groupDescs->InodeTableStart - 1) * BLOCK_SECS;
	ReadSects((void*)inodeTableBuf, inodeTableAddr, BLOCK_SECS);

	EXT2INODE* rootInode = (EXT2INODE*)(inodeTableBuf + sizeof(EXT2INODE)); // skip inode 1 to get inode 2
	if ((rootInode->TypeAndPerms & 0x4000) == 0)
	{
		Panic("Invalid root inode with %d.", rootInode->TypeAndPerms);
	}

	if (!(l1IndirectBuf = AllocateLowBytes(BLOCK_SIZE)))
	{
		Panic("Cannot allocate memory for l1 indirect buffer");
	}

	if (!(l2IndirectBuf = AllocateLowBytes(BLOCK_SIZE)))
	{
		Panic("Cannot allocate memory for l2 indirect buffer");
	}

	// Ignore upper size. Should not be so big.
	rootDir = AllocateLowBytes(rootInode->SizeLower);
	__builtin_memset(rootDir, 0, rootInode->SizeLower);
	if (ReadInode(rootInode, (BYTE*)rootDir) < 0)
	{
		Panic("Cannot read root directory.");
	}
}

static EXT2INODE* FindInode(const char* path)
{
	EXT2DIRENT* buf = AllocateLowBytes(BLOCK_SIZE);
	if (!buf)
	{
		Panic("Cannot allocate memory for dirent buffer");
	}

	__builtin_memcpy(buf, rootDir, BLOCK_SIZE);

	char* p = (char*)path;
	while (p && *p)
	{
		char* name = p;
		while (p && *p && *p != '/')
		{
			p++;
		}
//		LogDebug("Searching for %s %s\n", name, p);
		EXT2DIRENT* dirent = buf;
		while (dirent->Inode != 0 && dirent->RecordLength != 0)
		{
//			LogDebug("Ent %s %d %d\n", dirent->Name, dirent->NameLength, dirent->RecordLength);
			if (dirent->NameLength == p - name && MemCmp(dirent->Name, name, p - name) == 0)
			{
				const DWORD blockGroup = (dirent->Inode - 1) / superBlock.InodesPerGroup;
				const DWORD inodeIndex = (dirent->Inode - 1) % superBlock.InodesPerGroup;

				// read corresponding inode table
				const DWORD inodeTableAddr = BASE_LBA + (groupDescs[blockGroup].InodeTableStart - 1) * BLOCK_SECS;
				ReadSects((void*)inodeTableBuf, inodeTableAddr, BLOCK_SECS);

				EXT2INODE* node = (EXT2INODE*)(inodeTableBuf + inodeIndex * sizeof(EXT2INODE));

				if (dirent->FileType == EXT2DIRENT_TYPE_DIRECTORY)
				{
//					LogDebug("Found directory %s, inode %d\n", dirent->Name, dirent->Inode);
//					DebugDumpInode(node);
					ReadInode(node, (BYTE*)buf);
				}
				else if (dirent->FileType == EXT2DIRENT_TYPE_REGULAR && *p == 0)
				{
					return node;
				}

				break;
			}

			dirent = (EXT2DIRENT*)((char*)dirent + dirent->RecordLength);
		}

		if (*p == '/')
		{
			p++;
		}
	}
	return NULL;
}

SSIZE_T LoadKernelExt2(PARTTABLEITEM* part, BYTE** binary)
{
	EXT2INODE* inode = FindInode(KERNEL_PATH);
	if (!inode)
	{
		TerminalWriteString("neosknl is missing.");
		return -E_NOT_FOUND;
	}
//	DebugDumpInode(inode);
	// Also ignore upper size
	BYTE* kernBinary = AllocateLowBytes(inode->SizeLower);
	if (!kernBinary)
	{
		TerminalWriteString("Cannot allocate memory for the kernel binary");
		return -E_MEMORY;
	}
	SSIZE_T realSize = ReadInode(inode, kernBinary);
	if (realSize < 0)
	{
		TerminalWriteString("Cannot read the kernel binary.");
		return -E_IO;
	}
	*binary = kernBinary;
	return realSize;
}

SSIZE_T LoadDriverExt2(PARTTABLEITEM* part, const char* name, BYTE** binary)
{
	EXT2INODE* inode = FindInode(name);
	if (!inode)
	{
		TerminalPrintf("Device driver %s is missing.", name);
		return -E_NOT_FOUND;
	}
//	DebugDumpInode(inode);
	BYTE* drvBinary = AllocateLowBytes(inode->SizeLower);
	if (!drvBinary)
	{
		TerminalPrintf("Cannot allocate memory of %d bytes for the driver %s.", inode->SizeLower, name);
		return -E_MEMORY;
	}
	SSIZE_T realSize = ReadInode(inode, drvBinary);
	if (realSize < 0)
	{
		TerminalPrintf("Cannot read the driver %s.", name);
		return -E_IO;
	}
	*binary = drvBinary;
	return realSize;
}

BOOTFS ext2Fs = {
		.Initialize=InitializeExt2,
		.LoadKernel = LoadKernelExt2,
		.LoadModule=LoadDriverExt2
};

