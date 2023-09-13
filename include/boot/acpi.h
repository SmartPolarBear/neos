//
// Created by bear on 9/11/2023.
//

#pragma once

#include "ext.h"
#include "type.h"

typedef struct
{
	BYTE Signature[8];
	BYTE Checksum;
	BYTE OEMID[6];
	BYTE Revision;
	DWORD RSDTAddress;
	DWORD Length;
	QWORD XSDTAddress;
	BYTE ExtendedChecksum;
	BYTE Reserved[3];
}PACKED ACPIRSDP;
_Static_assert(sizeof(ACPIRSDP) == 36, "ACPIRSDP size mismatch");

#define ACPI_RSDP_SIGNATURE "RSD PTR "

typedef struct
{
	BYTE Signature[4];
	DWORD Length;
	BYTE Revision;
	BYTE Checksum;
	BYTE OEMID[6];
	BYTE OEMTableID[8];
	DWORD OEMRevision;
	DWORD CreatorID;
	DWORD CreatorRevision;
}PACKED ACPISDTHEADER;
_Static_assert(sizeof(ACPISDTHEADER) == 36, "ACPISDTHEADER size mismatch");

typedef struct
{
	ACPISDTHEADER Header;
	DWORD PointerToOtherSDT[0];
}PACKED ACPIRSDT;

typedef struct
{
	ACPISDTHEADER Header;
	DWORD FirmwareCtrl;
	DWORD Dsdt;
}PACKED ACPIFADT;
