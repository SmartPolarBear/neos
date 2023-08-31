//
// Created by bear on 8/26/2023.
//
#include "ext.h"
#include "defs.h"
#include "vesa.h"
#include "vgafont.h"
#include "draw.h"

// 16bit code
__asm__(".code16gcc");

VBEINFO vbeInfo;
VBEMODEINFO vbeModeInfo;

void NO_RETURN PanicVesa(void)
{
	__asm__ __volatile__ ("int $0x10" : : "a" (0x0E00 | 'F'), "b" (0x0007));
	__asm__ __volatile__ ("int $0x10" : : "a" (0x0E00 | 'V'), "b" (0x0007));
	for (;;);
}

void InitializeVesa()
{
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
		__builtin_memcpy((void*)VESA_MODEINFO_ADDR, &vbeModeInfo, sizeof(VBEMODEINFO));
		__asm__ __volatile__ ("int $0x10" : : "a" (0x4F02), "b" (*mode) : "memory");
		break;
	}


	if (!foundMode)
	{
		PanicVesa();
	}
}

