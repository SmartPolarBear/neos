//
// Created by bear on 9/16/2023.
//

#include "defs.h"
#include "ext.h"

#include "boot/param.h"

BOOTPARAM* gBootParam = NULL;

void NO_RETURN KernelMain(
		BOOTPARAM* bootParam
)
{
	gBootParam = bootParam;

//	gBootParam->BootService.TerminalService.TerminalClear();

	bootParam->BootService.TerminalService.TerminalPrintf("Hello, world!\n");

	gBootParam->BootService.ExitBootServices();

	for (;;);
}