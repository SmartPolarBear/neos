//
// Created by bear on 9/16/2023.
//

#include "defs.h"
#include "ext.h"

#include "boot/param.h"
#include "hal/hal.h"
#include "hal/halops.h"


BOOTPARAM* gBootParam = NULL;

void NO_RETURN KernelMain(
		BOOTPARAM* bootParam
)
{
	gBootParam = bootParam;

//	gBootParam->BootService.TerminalService.TerminalClear();

	bootParam->BootService.TerminalService.TerminalPrintf("Hello, world!\n");

	HalEntry halEntry = (HalEntry)bootParam->NeosExecutive.HalEntryAddr;
//	halEntry(gBootParam); // Start the hardware abstraction layer

	HALOPS* ops = (HALOPS*)bootParam->NeosExecutive.HalOpAddr;
	ops->Hello();	// Test if HAL really works.

	gBootParam->BootService.ExitBootServices();

	for (;;);
}