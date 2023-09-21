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


// Custom lltoa function with reverse logic included
char* Lltoa(long long num, char* str, int base)
{
	if (base < 2 || base > 36)
	{
		str[0] = '\0'; // Invalid base
		return str;
	}

	int i = 0;
	int isNegative = 0;

	// Handle 0 explicitly, otherwise empty string is printed
	if (num == 0)
	{
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}

	// Handle negative numbers for base 10
	if (num < 0 && base == 10)
	{
		isNegative = 1;
		num = -num;
	}

	// Convert the number to a string in reverse order
	while (num != 0)
	{
		long long rem = num % base;
		str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		num = num / base;
	}

	// Add '-' for negative numbers (base 10)
	if (isNegative)
	{
		str[i++] = '-';
	}

	str[i] = '\0'; // Null-terminate the string

	// Reverse the string in-place
	int start = 0;
	int end = i - 1;
	while (start < end)
	{
		char temp = str[start];
		str[start] = str[end];
		str[end] = temp;
		start++;
		end--;
	}

	return str;
}

// Custom ulltoa function with reverse logic included
char* Ulltoa(unsigned long long num, char* str, int base)
{
	if (base < 2 || base > 36)
	{
		str[0] = '\0'; // Invalid base
		return str;
	}

	int i = 0;

	// Handle 0 explicitly, otherwise empty string is printed
	if (num == 0)
	{
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}

	// Convert the number to a string in reverse order
	while (num != 0)
	{
		unsigned long long rem = num % base;
		str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		num = num / base;
	}

	str[i] = '\0'; // Null-terminate the string

	// Reverse the string in-place
	int start = 0;
	int end = i - 1;
	while (start < end)
	{
		char temp = str[start];
		str[start] = str[end];
		str[end] = temp;
		start++;
		end--;
	}

	return str;
}

char* Utoa(unsigned int num, char* str, int base)
{
	if (base < 2 || base > 36)
	{
		str[0] = '\0'; // Invalid base
		return str;
	}

	int i = 0;

	// Handle 0 explicitly, otherwise empty string is printed
	if (num == 0)
	{
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}

	// Convert the number to a string in reverse order
	while (num != 0)
	{
		unsigned int rem = num % base;
		str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		num = num / base;
	}

	str[i] = '\0'; // Null-terminate the string

	// Reverse the string in-place
	int start = 0;
	int end = i - 1;
	while (start < end)
	{
		char temp = str[start];
		str[start] = str[end];
		str[end] = temp;
		start++;
		end--;
	}

	return str;
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

void NO_RETURN Panic(char* msg, ...)
{
	__builtin_va_list args;
	__builtin_va_start(args, msg);

	TerminalSetColor(RED, BLACK);
	TerminalWriteString("!Panicked:");
	TerminalVPrintf(msg, args);

	__builtin_va_end(args);

	for (;;)
		__asm__ volatile ("hlt");
}