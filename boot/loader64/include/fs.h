//
// Created by bear on 9/14/2023.
//

#pragma once

#include "type.h"
#include "boot/fs.h"

extern PARTTABLEITEM* activePartition;

typedef struct
{
	void (*Initialize)(PARTTABLEITEM* part);

	SSIZE_T (* LoadKernel)(PARTTABLEITEM* part);

	SSIZE_T (* LoadDriver)(PARTTABLEITEM* part, const char*);
} BOOTFS;

void InitializeBootFs(PARTTABLEITEM* ap);

SSIZE_T LoadKernel();

SSIZE_T LoadDriver(const char* name);