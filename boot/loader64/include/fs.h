//
// Created by bear on 9/14/2023.
//

#pragma once

#include "type.h"
#include "boot/fs.h"

extern PARTTABLEITEM* activePartition;

#define KERNEL_PATH "neos/neosknl"
#define HAL_PATH "neos/hal.sys"

typedef struct
{
	void (* Initialize)(PARTTABLEITEM* part);

	SSIZE_T (* LoadKernel)(PARTTABLEITEM* part, BYTE** binary);

	SSIZE_T (* LoadModule)(PARTTABLEITEM* part, const char*, BYTE** binary);
} BOOTFS;

void InitializeBootFs(PARTTABLEITEM* ap);

UINT_PTR LoadKernel();

void LoadDriver(const char* name);