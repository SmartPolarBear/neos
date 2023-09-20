//
// Created by bear on 9/16/2023.
//

#include "defs.h"
#include "ext.h"
#include "type.h"
#include "disk.h"
#include "x86.h"


/* WaitDisk - wait for disk ready */
static inline void ALWAYS_INLINE WaitDisk(void)
{
	while ((inb(0x1F7) & 0xC0) != 0x40)
		/* do nothing */;
}

/* ReadSect - read a single sector at @secno into @dst */
void ReadSects(void* dst, DWORD secno, DWORD secs)
{
	// wait for disk to be ready
	WaitDisk();

	outb(0x1F2, (BYTE)(secs & 0xFF));                         // count = 1
	outb(0x1F3, secno & 0xFF);
	outb(0x1F4, (secno >> 8) & 0xFF);
	outb(0x1F5, (secno >> 16) & 0xFF);
	outb(0x1F6, ((secno >> 24) & 0xF) | 0xE0);
	outb(0x1F7, 0x20);                      // cmd 0x20 - read sectors

	for (int i = 0; i < secs; i++)
	{
		WaitDisk();
		insl(0x1F0, dst, SEC_SIZE / 4);
		dst += SEC_SIZE;
	}
}