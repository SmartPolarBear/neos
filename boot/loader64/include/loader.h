//
// Created by bear on 9/17/2023.
//

#pragma once

#include "type.h"

UINT_PTR LoadKernel();

void LoadHal();

void LoadDriver(const char* name);