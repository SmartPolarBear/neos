#include "defs.h"
#include "ext.h"
#include "boot/param.h"

BOOTPARAM* gBootParam = NULL;

void NO_RETURN HalEntry(
		BOOTPARAM* bootParam
)
{
	gBootParam = bootParam;
	for (;;);
}