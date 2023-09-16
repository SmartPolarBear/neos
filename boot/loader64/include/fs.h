//
// Created by bear on 9/14/2023.
//

#pragma once

#include "type.h"
#include "boot/fs.h"

extern PARTTABLEITEM* activePartition;

#define KERNEL_PATH "neos/neosknl"

typedef struct
{
	void (* Initialize)(PARTTABLEITEM* part);

	SSIZE_T (* LoadKernel)(PARTTABLEITEM* part, BYTE** binary);

	SSIZE_T (* LoadDriver)(PARTTABLEITEM* part, const char*, BYTE** binary);
} BOOTFS;

void InitializeBootFs(PARTTABLEITEM* ap);

void LoadKernel();

void LoadDriver(const char* name);