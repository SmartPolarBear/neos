//
// Created by bear on 8/27/2023.
//

#pragma once

#ifndef NULL
#define NULL ((void*)0)
#endif

#define MMAP_ADDR 0x8000
#define VGA_FONT_ADDR 0x5000
#define VESA_MODEINFO_ADDR 0x9000
#define FS_BUFFER_START 0x10000
#define KERNEL_LOAD_ADDR 0x80000

#define MBR_SYSID_LINUXNATIVE 0x83