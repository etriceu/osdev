#include "../include/malloc.h"

#define SIZE sizeof(size_t)

size_t ramBegin = 0x100000, ramEnd = 0x40000000; //idk how to check ram size

size_t last;

void memInit()
{
	for(int *n = (int*)ramBegin; n != (int*)ramEnd; n++)
		*n = 0;
}

void* malloc(size_t size)
{
	size_t s = 0;
	while(1)
	{
		if(last > ramEnd)
		{
			last = ramBegin+SIZE;
			s = 0;
		}
		
		if(*(uint8_t*)last == 0)
			s++;
		else
		{
			s = 0;
			last += *(size_t*)(last-SIZE)-1;
		}
		
		if(s == size+SIZE)
			break;

		last++;
	}
	last -= s-1;
	*(size_t*)last = size;
	last += SIZE;
	void *ptr = (void*)last;
	last += size;
	
	return ptr;
}

void free(void *ptr)
{
	size_t *size = (size_t*)(ptr-SIZE);
	for(uint8_t *end = ptr+*size; ptr != end; ptr++)
		*((uint8_t*)ptr) = 0;
	
	*size = 0;
}
