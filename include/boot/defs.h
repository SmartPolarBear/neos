//
// Created by bear on 9/7/2023.
//

#pragma once

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#define MMAP_ADDR 0x8000
#define VGA_FONT_ADDR 0x6000
#define VESA_MODEINFO_ADDR 0x9000
#define BUFFER_START 0x10000
#define NELDR_LOAD_ADDR 0x100000
#define NELDR_HIGH_BUFFER 0x200000

// 16MB, to give all low memory to the stupid DMA devices.
#define KERNEL_LOAD_ADDR 0x1000000
#define KERNEL_LINK_ADDR 0xffffffff81000000
#define HAL_LINK_ADDR 0xffffffff8a000000

