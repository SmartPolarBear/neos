//
// Created by bear on 8/31/2023.
//
#pragma once

#include "ext.h"
#include "type.h"
#include "boot/fs.h"

void InitializeBootFS();

INT LoadKernel(DWORD addr);
