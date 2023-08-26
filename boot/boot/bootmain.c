//
// Created by bear on 8/23/2023.
//
#include "ext.h"
#include "type.h"

#define OSLDR_SECS 5

// 16bit code
__asm__(".code16gcc");

volatile BYTE* vga = (BYTE*)0xb8000;

inline ALWAYS_INLINE void Puts(char* s)
{
	while (*s)
	{
		*vga++ = *s++;
		*vga++ = 0x07;
	}
}

void NO_RETURN BootMain(void)
{
	Puts("Starting NEOS...");



	Puts("Booting unexpectedly failed.");
	for (;;);
	UNREACHABLE;
}
