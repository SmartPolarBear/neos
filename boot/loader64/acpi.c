//
// Created by bear on 9/10/2023.
//
#include "defs.h"
#include "acpi.h"
#include "utils.h"
#include "terminal.h"

#include "lai/core.h"

ACPIRSDP* gRSDP = NULL;

static inline BYTE ALWAYS_INLINE ACPIChecksum(const BYTE* buf, DWORD len)
{
	BYTE sum = 0;
	for (const BYTE* p = (BYTE*)buf; p < ((BYTE*)buf) + len; p++)
	{
		sum += *p;
	}
	return sum;
}

void InitializeAcpi(void)
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

	TerminalPrintf("ACPI(v%d) detected.\n", gRSDP->Revision);

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
			if (ACPIChecksum((BYTE*)header, header->Length) != 0)
				Panic("ACPI table checksum is invalid.");

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
	// It is fucking special case because ACPIv1 won't give it a fuck publicity.
	if (sig[0] == 'D' && sig[1] == 'S' && sig[2] == 'D' && sig[3] == 'T')
	{
		ACPIFADT* fadt = (ACPIFADT*)AcpiLocateTable("FACP", n);
		if (!fadt)
		{
			return NULL;
		}

		if (gRSDP->Revision < 2)
		{
			return (void*)(UINT_PTR)fadt->Dsdt;
		}
		else
		{
			return (void*)fadt->XDsdt;
		}
	}


	if (gRSDP->Revision < 2)
	{
		return AcpiLocateTableRSDT(sig, n);
	}
	else
	{
		return AcpiLocateTableXSDT(sig, n);
	}
}

static inline INT AcpiEnumerateDeviceNode(lai_nsnode_t* n,
		INT depth,
		AcpiEnumerationEnterFunction bn,
		AcpiEnumerationLeftFunction an)
{
	bn(depth, n);

	INT children = 0;
	struct lai_ns_child_iterator iter = LAI_NS_CHILD_ITERATOR_INITIALIZER(n);
	for (lai_nsnode_t* p = lai_ns_child_iterate(&iter); p; p = lai_ns_child_iterate(&iter))
	{
		children += AcpiEnumerateDeviceNode(p, depth + 1, bn, an);
	}
	an(depth, children, n);

	return children;
}

INT AcpiEnumerateDevices(AcpiEnumerationEnterFunction beforeNode, AcpiEnumerationLeftFunction afterNode)
{
	lai_nsnode_t* root = lai_ns_get_root();
	return AcpiEnumerateDeviceNode(root, 0, beforeNode, afterNode);
}

void AcpiLoadDriverForDevices()
{

}

void AcpiInitializeProcessors()
{
	
}