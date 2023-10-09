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

#define IN
#define OUT

#define VA_LIST __builtin_va_list
#define VA_START __builtin_va_start
#define VA_ARG __builtin_va_arg
#define VA_END __builtin_va_end

#define VGA_FONT_ADDR 0x6000
#define MMAP_ADDR 0x8000
#define VESA_MODEINFO_ADDR 0x9000
#define NELDR_HIGH_ADDR 0x11000
#define BUFFER_START 0x80000

// 16MB, to give all low memory to the stupid DMA devices.
#define KERNEL_LOAD_ADDR 0x1000000

#define KERNEL_LINK_ADDR 0xffffffff81000000
#define MODULE_LINK_ADDR 0xffffffff8a000000
// just to avoid they are accidentally load to the same address as kernel itself.
#define HAL_LINK_ADDR MODULE_LINK_ADDR
#define DRIVER_LINK_ADDR MODULE_LINK_ADDR


