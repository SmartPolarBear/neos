//
// Created by bear on 9/2/2023.
//

#include "ext.h"
#include "type.h"
#include "disk.h"


static inline void ALWAYS_INLINE outb(WORD port, BYTE val)
{
	__asm__ volatile("outb %0, %1" : : "a"(val), "d"(port));
}

static inline BYTE ALWAYS_INLINE inb(WORD port)
{
	BYTE ret;
	__asm__ volatile("inb %1, %0" : "=a"(ret) : "d"(port));
	return ret;
}

static inline void ALWAYS_INLINE insl(WORD port, void* addr, DWORD cnt)
{
	__asm__ volatile("cld; rep insl" :
			"=D"(addr), "=c"(cnt) :
			"d"(port), "0"(addr), "1"(cnt) :
			"memory", "cc");
}

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