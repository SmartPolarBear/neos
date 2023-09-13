//
// Created by bear on 9/13/2023.
//

#pragma once

#include "type.h"

static inline BYTE 
inb(WORD port)
{
	BYTE data;

	asm volatile("in %1,%0" : "=a" (data) : "d" (port));
	return data;
}

static inline WORD
inw(int port)
{
	WORD data;
	asm volatile("inw %w1,%0": "=a" (data): "d" (port));
	return data;
}

static inline DWORD
inl(int port)
{
	DWORD data;
	asm volatile("inl %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

static inline void
insl(int port, void *addr, int cnt)
{
	asm volatile("cld; rep insl" :
			"=D" (addr), "=c" (cnt) :
			"d" (port), "0" (addr), "1" (cnt) :
			"memory", "cc");
}

static inline void
outb(WORD port, BYTE data)
{
	asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

static inline void
outw(WORD port, WORD data)
{
	asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

static inline void
outl(int port, DWORD data)
{
	asm volatile("outl %0,%w1" : : "a" (data), "d" (port));
}

static inline void
outsl(int port, const void *addr, int cnt)
{
	asm volatile("cld; rep outsl" :
			"=S" (addr), "=c" (cnt) :
			"d" (port), "0" (addr), "1" (cnt) :
			"cc");
}

static inline void
stosb(void *addr, int data, int cnt)
{
	asm volatile("cld; rep stosb" :
			"=D" (addr), "=c" (cnt) :
			"0" (addr), "1" (cnt), "a" (data) :
			"memory", "cc");
}

static inline void
stosl(void *addr, int data, int cnt)
{
	asm volatile("cld; rep stosl" :
			"=D" (addr), "=c" (cnt) :
			"0" (addr), "1" (cnt), "a" (data) :
			"memory", "cc");
}
