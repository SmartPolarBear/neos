//
// Created by bear on 9/1/2023.
//
#include "defs.h"
#include "mem.h"
#include "terminal.h"


void BriefMemoryMap(void)
{
	// Start by casting MMAP_ADDR to the correct pointer type
	DWORD mmapCount = *(DWORD*)MMAP_ADDR;
	E820MEMMAP* e820Map = (E820MEMMAP*)(MMAP_ADDR + sizeof(DWORD));

	// Iterate through the E820 memory map until we reach an entry with a length of 0
	for (int i = 0; i < mmapCount; i++)
	{
		char* typeStr;

		switch (e820Map[i].Type)
		{
		case MEM_TYPE_USABLE:
			typeStr = "Usable RAM";
			break;
		case MEM_TYPE_RESERVED:
			typeStr = "Reserved";
			break;
		case MEM_TYPE_ACPI:
			typeStr = "ACPI Tables";
			break;
		case MEM_TYPE_ACPI_NVS:
			typeStr = "ACPI NVS";
			break;
		case MEM_TYPE_UNUSABLE:
			typeStr = "Unusable";
			break;
		default:
			typeStr = "Unknown";
			break;
		}

		// Print the memory map entry using TerminalPrintf
		TerminalPrintf("Memory Region #%d: Base=0x%x%x Length=0x%x%x Type=%d (%s)\n",
				i,
				e820Map[i].BaseAddrHigh,
				e820Map[i].BaseAddrLow,
				e820Map[i].LengthHigh,
				e820Map[i].LengthLow,
				e820Map[i].Type,
				typeStr);
	}

//	for (;;);
}