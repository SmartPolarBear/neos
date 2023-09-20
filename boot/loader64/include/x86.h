//
// Created by bear on 9/20/2023.
//

#pragma once

#include "type.h"
#include "ext.h"

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