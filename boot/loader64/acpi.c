//
// Created by bear on 9/10/2023.
//
#include "defs.h"
#include "acpi.h"
#include "utils.h"


ACPIRSDP* gRSDP = NULL;

static inline BYTE ALWAYS_INLINE ACPIChecksum(BYTE* buf, DWORD len)
{
	BYTE sum = 0;
	for (const BYTE* p = (BYTE*)buf; p < ((BYTE*)buf) + len; p++)
	{
		sum += *p;
	}
	return sum;
}


void InitializeACPI(void)
{
	// Scan memory for RSDP

	// search EBDA area
	for (BYTE* p = (BYTE*)ACPI_EBDA_BASE; !gRSDP && p < (BYTE*)ACPI_EBDA_LIMIT; p += ACPI_SCAN_BOUNDARY)
	{
		if (MemCmp(p, ACPI_RSDP_SIGNATURE, 8) == 0)
		{
			gRSDP = (ACPIRSDP*)p;
		}
	}

	// search main BIOS area
	for (BYTE* p = (BYTE*)ACPI_MAIN_BIOS_BASE; !gRSDP && p < (BYTE*)ACPI_MAIN_BIOS_LIMIT; p += ACPI_SCAN_BOUNDARY)
	{
		if (MemCmp(p, ACPI_RSDP_SIGNATURE, 8) == 0)
		{
			gRSDP = (ACPIRSDP*)p;
		}
	}

	if (!gRSDP)
	{
		Panic("ACPI is not supported on this machine.");
	}

	// checksum verification
	if (ACPIChecksum((BYTE*)gRSDP, gRSDP->Length) != 0)
	{
		Panic("ACPI RSDP is invalid.");
	}

}

