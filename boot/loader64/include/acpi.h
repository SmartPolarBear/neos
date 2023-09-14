//
// Created by bear on 9/11/2023.
//

#pragma once

#include "type.h"
#include "boot/acpi.h"

#include "lai/core.h"

#define ACPI_EBDA_BASE 0x040E
#define ACPI_EBDA_LIMIT (ACPI_EBDA_BASE+0x400)

#define ACPI_MAIN_BIOS_BASE 0x000E0000
#define ACPI_MAIN_BIOS_LIMIT (ACPI_MAIN_BIOS_BASE+0x20000)

#define ACPI_SCAN_BOUNDARY 16


// for neldr, let's keep things fucking simple with no getters and setters
extern ACPIRSDP* gRSDP;

void InitializeAcpi(void);

void* AcpiLocateTable(char* sig, SIZE_T n);

typedef void (* AcpiEnumerationEnterFunction)(INT depth, lai_nsnode_t* node);
typedef void (* AcpiEnumerationLeftFunction)(INT depth, INT childCount, lai_nsnode_t* node);

INT AcpiEnumerateDevices(AcpiEnumerationEnterFunction beforeNode, AcpiEnumerationLeftFunction afterNode);

void AcpiLoadDriverForDevices();

