//
// Created by bear on 9/7/2023.
//

#pragma once

#include "type.h"

#define SUPERBLOCK_OFFSET 1024

typedef struct
{
	DWORD InodeCount;
	DWORD BlockCount;
	DWORD ReservedBlockCount;
	DWORD FreeBlockCount;
	DWORD FreeInodeCount;
	DWORD FirstDataBlock;
	DWORD BlockSize;
	DWORD FragmentSize;
	DWORD BlocksPerGroup;
	DWORD FragmentsPerGroup;
	DWORD InodesPerGroup;
	DWORD MountTime;
	DWORD WriteTime;
	WORD MountCount;
	WORD MaxMountCount;
	WORD Magic;
	WORD State;
	WORD Errors;
	WORD MinorRevisionLevel;
	DWORD LastCheck;
	DWORD CheckInterval;
	DWORD CreatorOS;
	DWORD RevisionLevel;
	WORD DefaultUID;
	WORD DefaultGID;
	DWORD FirstInode;
	WORD InodeSize;
	WORD BlockGroupNumber;
	DWORD FeatureCompat;
	DWORD FeatureIncompat;
	DWORD FeatureROCompat;
	BYTE UUID[16];
	BYTE VolumeName[16];
	BYTE LastMounted[64];
	DWORD AlgorithmUsageBitmap;
	BYTE PreallocBlocks;
	BYTE PreallocDirBlocks;
	WORD ReservedGDTBlocks;
	BYTE JournalUUID[16];
	DWORD JournalInode;
	DWORD JournalDev;
	DWORD LastOrphan;
	DWORD HashSeed[4];
	BYTE DefHashVersion;
	BYTE Reserved[3];
	DWORD DefaultMountOptions;
	DWORD FirstMetaBG;
	BYTE Reserved2[760];
} PACKED EXT2SB;
_Static_assert(sizeof(EXT2SB) == 1024, "EXT2SB size mismatch");

#define EXT2_SB_MAGIC 0xEF53

typedef struct
{
	DWORD BlockUsageBitmap;
	DWORD InodeUsageBitmap;
	DWORD InodeTableStart;
	WORD FreeBlockCount;
	WORD FreeInodeCount;
	WORD DirectoryCount;
	BYTE Reserved[14];
}PACKED EXT2BGDT;

typedef struct
{
	WORD TypeAndPerms;
	WORD UserID;
	DWORD SizeLower;
	DWORD LastAccessTime;
	DWORD CreationTime;
	DWORD LastModificationTime;
	DWORD DeletionTime;
	WORD GroupID;
	WORD HardLinkCount;
	DWORD SectorCount;
	DWORD Flags;
	DWORD OSValue1;
	DWORD DirectBlockPointers[12];
	DWORD SinglyIndirectBlockPointer;
	DWORD DoublyIndirectBlockPointer;
	DWORD TriplyIndirectBlockPointer;
	DWORD GenerationNumber;
	DWORD ExtendedAttributeBlock;
	DWORD SizeUpper;
	DWORD FragmentBlockAddress;
	BYTE OSValue2[12];
}PACKED EXT2INODE;
_Static_assert(sizeof(EXT2INODE) == 128, "EXT2INODE size mismatch");

typedef struct
{
	DWORD Inode;
	WORD RecordLength;
	BYTE NameLength;
	BYTE FileType;
	char Name[0];
}PACKED EXT2DIRENT;

typedef enum
{
	EXT2DIRENT_TYPE_UNKNOWN = 0,
	EXT2DIRENT_TYPE_REGULAR = 1,
	EXT2DIRENT_TYPE_DIRECTORY = 2,
	EXT2DIRENT_TYPE_CHARDEV = 3,
	EXT2DIRENT_TYPE_BLOCKDEV = 4,
	EXT2DIRENT_TYPE_FIFO = 5,
	EXT2DIRENT_TYPE_SOCKET = 6,
	EXT2DIRENT_TYPE_SYMLINK = 7,
} EXT2DIRENT_TYPE;
