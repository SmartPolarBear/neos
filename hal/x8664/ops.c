//
// Created by bear on 9/20/2023.
//
#include "defs.h"
#include "halops.h"

void Hello()
{
	gBootParam->BootService.TerminalService.TerminalPrintf("Hello from HAL!\n");
}

HALOPS HalOps = {
		.Hello=Hello
}; // in ops.c