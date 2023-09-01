//
// Created by bear on 9/1/2023.
//
#include "defs.h"
#include "mem.h"
#include "terminal.h"

// Define constants for memory types
#define MEM_TYPE_USABLE 1
#define MEM_TYPE_RESERVED 2
#define MEM_TYPE_ACPI 3
#define MEM_TYPE_ACPI_NVS 4
#define MEM_TYPE_UNUSABLE 5

void BriefMemoryMap(void)
{
	// Start by casting MMAP_ADDR to the correct pointer type
	E820MEMMAP* e820Map = (E820MEMMAP*)MMAP_ADDR;

	int entryCount = 0;  // Initialize the entry count to 0

	// Iterate through the E820 memory map until we reach an entry with a length of 0
	while (e820Map[entryCount].LengthLow != 0 || e820Map[entryCount].LengthHigh != 0)
	{
		char* typeStr;

		switch (e820Map[entryCount].Type)
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
				entryCount,
				e820Map[entryCount].BaseAddrHigh,
				e820Map[entryCount].BaseAddrLow,
				e820Map[entryCount].LengthHigh,
				e820Map[entryCount].LengthLow,
				e820Map[entryCount].Type,
				typeStr);

		entryCount++;
	}
}