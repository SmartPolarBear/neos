//
// Created by bear on 9/13/2023.
//

typedef unsigned int size_t;

// Implementation of memcmp
int memcmp(const void* ptr1, const void* ptr2, size_t num)
{
	const unsigned char* p1 = (const unsigned char*)ptr1;
	const unsigned char* p2 = (const unsigned char*)ptr2;

	for (size_t i = 0; i < num; i++)
	{
		if (p1[i] != p2[i])
		{
			return p1[i] - p2[i];
		}
	}

	return 0;
}

// Implementation of memcpy
void* memcpy(void* destination, const void* source, size_t num)
{
	unsigned char* dest = (unsigned char*)destination;
	const unsigned char* src = (const unsigned char*)source;

	for (size_t i = 0; i < num; i++)
	{
		dest[i] = src[i];
	}

	return destination;
}

// Implementation of memset
void* memset(void* ptr, int value, size_t num)
{
	unsigned char* p = (unsigned char*)ptr;

	for (size_t i = 0; i < num; i++)
	{
		p[i] = (unsigned char)value;
	}

	return ptr;
}
