//
// Created by bear on 8/26/2023.
//
#include "ext.h"
#include "defs.h"
#include "vesa.h"
#include "vgafont.h"
#include "draw.h"

// 16bit code
__asm__(".code16");
//__asm__(".code16gcc"); do not use this, it will cause the code to be use 32bit things, which is bad

VBEINFO vbeInfo;
VBEMODEINFO vbeModeInfo;

static inline INT MemCmp(const void* ptr1, const void* ptr2, DWORD size)
{
	const BYTE* p1 = (const BYTE*)ptr1;
	const BYTE* p2 = (const BYTE*)ptr2;

	for (DWORD i = 0; i < size; i++)
	{
		if (p1[i] != p2[i])
		{
			return p1[i] - p2[i];
		}
	}

	return 0;
}

// In 16bit code, we cannot call BootPanic() directly, so we use this function to panic
static inline void ALWAYS_INLINE NO_RETURN PanicVesa(void)
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

