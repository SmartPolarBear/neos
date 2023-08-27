//
// Created by bear on 8/26/2023.
//
#include "ext.h"
#include "defs.h"
#include "vesa.h"
#include "vga.h"

// 16bit code
__asm__(".code16gcc");

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

VBEINFO vbeInfo;
VBEMODEINFO vbeModeInfo;


void NO_RETURN PanicVesa(void)
{
	__asm__ __volatile__ ("int $0x10" : : "a" (0x0E00 | 'F'), "b" (0x0007));
	__asm__ __volatile__ ("int $0x10" : : "a" (0x0E00 | 'V'), "b" (0x0007));
	for (;;);
}

void ALWAYS_INLINE CopyVgaFont()
{

}

void InitializeVesa()
{
	CopyVgaFont();
	
	// validate vbe information
	if (vbeInfo.signature[0] != 'V' || vbeInfo.signature[1] != 'E' || vbeInfo.signature[2] != 'S' ||
		vbeInfo.signature[3] != 'A')
		PanicVesa();

	if (vbeInfo.version < 0x0200)
		PanicVesa();

	// iterate through all video modes
	BOOL foundMode = FALSE;
	for (WORD* mode = (WORD*)vbeInfo.video_modes; *mode != 0xFFFF; mode++)
	{
		WORD result = 0;
		// get mode information
		__asm__ __volatile__ ("int $0x10" : "=a" (result) : "a" (0x4F01), "c" (*mode), "D" (&vbeModeInfo));
		if (result != 0x004F)
		{
			continue;
		}

		// at least 640x480
		if (vbeModeInfo.width < BOOTTIME_MIN_SCRN_WIDTH || vbeModeInfo.height < BOOTTIME_MIN_SCRN_HEIGHT)
		{
			continue;
		}

		// at least 16 bits per pixel
		if (vbeModeInfo.bpp < BOOTTIME_MIN_SCRN_BPP)
		{
			continue;
		}

		// support linear frame buffer
		if ((vbeModeInfo.attributes & 0x80) == 0)
		{
			continue;
		}

		// found a suitable mode
		foundMode = TRUE;

		// set vbe mode
		__asm__ __volatile__ ("int $0x10" : : "a" (0x4F02), "b" (*mode) : "memory");
		break;
	}

	if (!foundMode)
	{
		PanicVesa();
	}


}

void PutPixel(BYTE color, INT x, INT y)
{

}