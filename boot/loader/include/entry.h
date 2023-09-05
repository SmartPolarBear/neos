//
// Created by bear on 9/5/2023.
//

#pragma once

#include "ext.h"

static inline void ALWAYS_INLINE NO_RETURN BootPanic()
{
	for (;;)
		__asm__ volatile ("hlt");
}