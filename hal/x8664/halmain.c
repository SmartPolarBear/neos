#include "defs.h"
#include "ext.h"
#include "halops.h"


BOOTPARAM* gBootParam = NULL;

// global interface symbol for bootloader to read
HALOPS* HalOps = &gHalOps;

void NO_RETURN HalEntry(
		BOOTPARAM* bootParam
)
{
	gBootParam = bootParam;
	gHalOps.Hello = NULL;
	for (;;);
}