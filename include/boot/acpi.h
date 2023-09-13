//
// Created by bear on 9/11/2023.
//

#pragma once

#include "ext.h"
#include "type.h"

typedef struct{
	BYTE AddressSpaceID;
	BYTE RegisterBitWidth;
	BYTE RegisterBitOffset;
	BYTE AccessSize;
	QWORD Address;
}PACKED ACPIGenericAddress;

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

typedef struct{
	ACPISDTHEADER Header;
	QWORD PointerToOtherSDT[0];
}PACKED ACPIXSDT;

typedef struct{
	ACPISDTHEADER Header;
	DWORD FirmwareControl;
	DWORD Dsdt;
	BYTE X_Reserved;
	BYTE PreferredPowerManagementProfile;
	WORD SCI_Interrupt;
	DWORD SMI_CommandPort;
	BYTE ACPIEnable;
	BYTE ACPIDisable;
	BYTE S4BIOS_REQ;
	BYTE PSTATE_Control;
	DWORD PM1aEventBlock;
	DWORD PM1bEventBlock;
	DWORD PM1aControlBlock;
	DWORD PM1bControlBlock;
	DWORD PM2ControlBlock;
	DWORD PMTimerBlock;
	DWORD GPE0Block;
	DWORD GPE1Block;
	BYTE PM1EventLength;
	BYTE PM1ControlLength;
	BYTE PM2ControlLength;
	BYTE PMTimerLength;
	BYTE GPE0Length;
	BYTE GPE1Length;
	BYTE GPE1Base;
	BYTE CStateControl;
	WORD WorstC2Latency;
	WORD WorstC3Latency;
	WORD FlushSize;
	WORD FlushStride;
	BYTE DutyOffset;
	BYTE DutyWidth;
	BYTE DayAlarm;
	BYTE MonthAlarm;
	BYTE Century;
	WORD BootArchitectureFlags;
	BYTE Reserved2;
	DWORD Flags;
	ACPIGenericAddress ResetReg;
	BYTE ResetValue;
	BYTE Reserved3[3];
	QWORD XFirmwareControl;
	QWORD XDsdt;
	ACPIGenericAddress XPM1aEventBlock;
	ACPIGenericAddress XPM1bEventBlock;
	ACPIGenericAddress XPM1aControlBlock;
	ACPIGenericAddress XPM1bControlBlock;
	ACPIGenericAddress XPM2ControlBlock;
	ACPIGenericAddress XPMTimerBlock;
	ACPIGenericAddress XGPE0Block;
	ACPIGenericAddress XGPE1Block;
}PACKED ACPIFADT;


