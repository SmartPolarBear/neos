//
// Created by bear on 9/20/2023.
//

#pragma once

#include "boot/param.h"

typedef void (* HalEntry)(
		BOOTPARAM* bootParam
);