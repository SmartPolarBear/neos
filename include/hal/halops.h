//
// Created by bear on 9/20/2023.
//

#pragma once

#include "type.h"

typedef struct
{
	struct
	{
		DWORD Major, Minor, Build;
	} Version;

	// Test function to say hello
	void (* Hello)(void);
} HALOPS;

