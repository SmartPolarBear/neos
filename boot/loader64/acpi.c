//
// Created by bear on 9/10/2023.
//
#include "defs.h"
#include "acpi.h"
#include "utils.h"
#include "terminal.h"
#include "log.h"

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
		Panic("ACPI RSDP checksum is invalid.");
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
	AcpiEnumerationEnterCallback enter,
	AcpiEnumerationLeftCallback left)
{
	void* enterParam = enter(depth, n);

	INT children = 0;
	struct lai_ns_child_iterator iter = LAI_NS_CHILD_ITERATOR_INITIALIZER(n);
	for (lai_nsnode_t* p = lai_ns_child_iterate(&iter); p; p = lai_ns_child_iterate(&iter))
	{
		children += AcpiEnumerateDeviceNode(p, depth + 1, enter, left);
	}

	left(depth, children, n, enterParam);

	return children + 1;
}

INT AcpiEnumerateDevices(AcpiEnumerationEnterCallback beforeNode, AcpiEnumerationLeftCallback afterNode)
{
	lai_nsnode_t* root = lai_ns_get_root();
	return AcpiEnumerateDeviceNode(root, 0, beforeNode, afterNode);
}

SSIZE_T deviceDriverCount = 0;
SSIZE_T processorCount = 0;

// count, and log if needed
static inline void* AcpiLoadDriverEnterCallback(INT depth, lai_nsnode_t* node)
{
	if (node->type != LAI_NAMESPACE_DEVICE && node->type != LAI_NAMESPACE_PROCESSOR)
	{
		return NULL;
	}

	switch (node->type)
	{
	default:
	case LAI_NAMESPACE_DEVICE:
		deviceDriverCount++;
		TerminalPrintf("Name: %s at depth %d\n", node->name, depth);
		// TODO: allocate a slot for driver, which will be return for left callback
		
		break;

	case LAI_NAMESPACE_PROCESSOR:
		processorCount++;
		TerminalPrintf("Processor: %s at depth %d\n", node->name, depth);
		break;
	}

	return NULL;
}

// load driver
static inline void AcpiLoadDriverLeftCallback(INT depth, INT childCount, lai_nsnode_t* node, void* enterReturn)
{
	if (!enterReturn)
	{
		return;
	}
}

void AcpiLoadDriverForDevices()
{
	TerminalPrintf("Loading device drivers...\n");
	INT count = AcpiEnumerateDevices(AcpiLoadDriverEnterCallback, AcpiLoadDriverLeftCallback);
	TerminalPrintf("ACPI: %d nodes enumerated, %d drivers loaded.\n", count, deviceDriverCount);
}
