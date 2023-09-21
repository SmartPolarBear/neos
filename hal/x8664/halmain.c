#include "defs.h"
#include "ext.h"
#include "halops.h"


BOOTPARAM* gBootParam = NULL;

extern void FuckKernel(char* one);

void HalEntry(
		BOOTPARAM* bootParam
)
{
	HalOps.Version.Major = 0;
	HalOps.Version.Minor = 1;
	HalOps.Version.Build = 100;

	gBootParam = bootParam;

	gBootParam->BootService.TerminalService.TerminalWriteString("HalEntry is called!\n");
	gBootParam->BootService.TerminalService.TerminalPrintf("FuckKernel is resolved at %p\n", FuckKernel);
	FuckKernel("the hal");
}