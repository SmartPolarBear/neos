#include "defs.h"
#include "ext.h"
#include "halops.h"


BOOTPARAM* gBootParam = NULL;


void HalEntry(
		BOOTPARAM* bootParam
)
{
	HalOps.Version.Major = 0;
	HalOps.Version.Minor = 1;
	HalOps.Version.Build = 100;

	gBootParam = bootParam;
}