//
// Created by bear on 8/27/2023.
//

#pragma once

#include "type.h"

#define BOOTTIME_MIN_SCRN_WIDTH 640
#define BOOTTIME_MIN_SCRN_HEIGHT 480
#define BOOTTIME_MIN_SCRN_BPP 16

typedef struct
{
	BYTE signature[4];    // must be "VESA" to indicate valid VBE support
	WORD version;            // VBE version; high byte is major version, low byte is minor version
	DWORD oem;            // segment:offset pointer to OEM
	DWORD capabilities;        // bitfield that describes card capabilities
	DWORD video_modes;        // segment:offset pointer to list of supported video modes
	WORD video_memory;        // amount of video memory in 64KB blocks
	WORD software_rev;        // software revision
	DWORD vendor;            // segment:offset to card vendor string
	DWORD product_name;        // segment:offset to card model name
	DWORD product_rev;        // segment:offset pointer to product revision
	BYTE reserved[222];        // reserved for future expansion
	BYTE oem_data[256];        // OEM BIOSes store their strings in this area
} __attribute__ ((packed)) VBEINFO;

typedef struct
{
	WORD attributes;        // deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	BYTE window_a;            // deprecated
	BYTE window_b;            // deprecated
	WORD granularity;        // deprecated; used while calculating bank numbers
	WORD window_size;
	WORD segment_a;
	WORD segment_b;
	DWORD win_func_ptr;        // deprecated; used to switch banks from protected mode without returning to real mode
	WORD pitch;            // number of bytes per horizontal line
	WORD width;            // width in pixels
	WORD height;            // height in pixels
	BYTE w_char;            // unused...
	BYTE y_char;            // ...
	BYTE planes;
	BYTE bpp;            // bits per pixel in this mode
	BYTE banks;            // deprecated; total number of banks in this mode
	BYTE memory_model;
	BYTE bank_size;        // deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	BYTE image_pages;
	BYTE reserved0;

	BYTE red_mask;
	BYTE red_position;
	BYTE green_mask;
	BYTE green_position;
	BYTE blue_mask;
	BYTE blue_position;
	BYTE reserved_mask;
	BYTE reserved_position;
	BYTE direct_color_attributes;

	DWORD framebuffer;        // physical address of the linear frame buffer; write here to draw to the screen
	DWORD off_screen_mem_off;
	WORD off_screen_mem_size;    // size of memory in the framebuffer but not being displayed on the screen
	BYTE reserved1[206];
} __attribute__ ((packed)) VBEMODEINFO;
