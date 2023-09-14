//
// Created by bear on 9/14/2023.
//

#pragma once

#include "type.h"
#include "boot/fs.h"

extern PARTTABLEITEM* activePartition;

void InitializeBootFs();

void LoadKernel();

void LoadDriver(const char *name);