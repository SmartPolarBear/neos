//
// Created by bear on 9/17/2023.
//

#pragma once

#include "type.h"
#include "ext.h"


typedef struct
{
	UINT_PTR DriverLoadAddr;
	UINT_PTR DriverOpAddr;
	SIZE_T FileSize;
	SIZE_T MemSize;
} ALIGN(8) DRIVERLOAD;

typedef struct
{
	UINT_PTR BootPartition;

	struct
	{
		UINT_PTR KernelLoadAddr;
		SIZE_T KernelFileSize;
		SIZE_T KernelMemSize;
		UINT_PTR HalLoadAddr;
		UINT_PTR HalOpAddr;
		UINT_PTR HalEntryAddr;
		SIZE_T HalFileSize;
		SIZE_T HalMemSize;
		SIZE_T DriverCount;
		SIZE_T TotalDriverFileSize;
		SIZE_T TotalDriverMemSize;
	} NeosExecutive;

	struct
	{
		SIZE_T Width;
		SIZE_T Height;
		SIZE_T Depth;
		SIZE_T Pitch;
		UINT_PTR FrameBuffer;
		QWORD Misc[8];
	} BootVideo;

	struct
	{
		struct
		{
			void (* TerminalPrintf)(char* format, ...);

			void (* TerminalWriteString)(char* str);

			void (* TerminalClear)(void);
		} TerminalService;

		void (* ExitBootServices)();
	} BootService;

	UINT_PTR PhysicalPages;
	SIZE_T PhysicalPageCount;

	DRIVERLOAD DriverLoad[0];
} ALIGN(8) BOOTPARAM;

