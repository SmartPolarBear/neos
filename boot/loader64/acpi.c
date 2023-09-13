//
// Created by bear on 9/10/2023.
//
#include "defs.h"
#include "acpi.h"
#include "utils.h"

#include "lai/core.h"

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


	// Initialize LAI
	lai_set_acpi_revision(gRSDP->Revision);
	lai_create_namespace();
}

static inline void* ALWAYS_INLINE AcpiLocateTableRSDT(char* sig, SIZE_T n)
{
	ACPIRSDT* rsdt = (ACPIRSDT*)(UINT_PTR)gRSDP->RSDTAddress;
	const SIZE_T entries = (rsdt->Header.Length - sizeof(ACPISDTHEADER)) / sizeof(DWORD);
	int k = 0;
	for (SIZE_T i = 0; i < entries; i++)
	{
		ACPISDTHEADER* header = (ACPISDTHEADER*)(UINT_PTR)rsdt->PointerToOtherSDT[i];
		if (MemCmp(header->Signature, sig, 4) == 0)
		{
			if (k++ == n)
			{
				return header;
			}
		}
	}
	return NULL;
}

static inline void* ALWAYS_INLINE AcpiLocateTableXSDT(char* sig, SIZE_T n)
{
	ACPIXSDT* xsdt = (ACPIXSDT*)gRSDP->XSDTAddress;
	const SIZE_T entries = (xsdt->Header.Length - sizeof(ACPISDTHEADER)) / sizeof(QWORD);
	int k = 0;
	for (SIZE_T i = 0; i < entries; i++)
	{
		ACPISDTHEADER* header = (ACPISDTHEADER*)xsdt->PointerToOtherSDT[i];
		if (MemCmp(header->Signature, sig, 4) == 0)
		{
			if (k++ == n)
			{
				return header;
			}
		}
	}
	return NULL;
}

void* AcpiLocateTable(char* sig, SIZE_T n)
{
	if (gRSDP->Revision == 0)
		return AcpiLocateTableRSDT(sig, n);
	else
		return AcpiLocateTableXSDT(sig, n);
}

