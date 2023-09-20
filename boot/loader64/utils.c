//
// Created by bear on 9/7/2023.
//
#include "ext.h"
#include "utils.h"
#include "terminal.h"
#include "draw.h"

void Itoa(int num, char* buffer, int base)
{
	int i = 0;
	int isNegative = 0;

	// Handle negative numbers for base 10
	if (num < 0 && base == 10)
	{
		isNegative = 1;
		num = -num;
	}

	// Handle the special case of 0
	if (num == 0)
	{
		buffer[i++] = '0';
	}
	else
	{
		while (num != 0)
		{
			int remainder = num % base;
			buffer[i++] = (remainder < 10) ? (remainder + '0') : (remainder - 10 + 'a');
			num /= base;
		}
	}

	// Add the negative sign for base 10 if necessary
	if (isNegative && base == 10)
	{
		buffer[i++] = '-';
	}

	// Reverse the string
	int left = 0;
	int right = i - 1;
	while (left < right)
	{
		char temp = buffer[left];
		buffer[left] = buffer[right];
		buffer[right] = temp;
		left++;
		right--;
	}

	// Null-terminate the string
	buffer[i] = '\0';
}




INT MemCmp(const void* ptr1, const void* ptr2, DWORD size)
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

INT StrCmp(const char* str1, const char* str2)
{
	while (*str1 != '\0' && *str2 != '\0')
	{
		if (*str1 != *str2)
		{
			return *str1 - *str2;
		}

		str1++;
		str2++;
	}

	return *str1 - *str2;
}

void NO_RETURN Panic(char* msg)
{
	TerminalSetColor(RED, BLACK);
	TerminalWriteString("!Panicked:");
	TerminalWriteString(msg);
	for (;;)
		__asm__ volatile ("hlt");
}