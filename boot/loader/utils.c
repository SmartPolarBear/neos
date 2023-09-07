//
// Created by bear on 9/5/2023.
//
#include "utils.h"

// Custom implementation of itoa (integer to ASCII) function
// Converts an integer to its ASCII representation
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