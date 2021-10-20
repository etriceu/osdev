#include "../include/malloc.h"

#define SIZE sizeof(size_t)

extern uint8_t kernelEnd;

size_t ramBegin, ramEnd = 0x40000000; //idk how to check ram size

size_t last;

void memInit()
{
	ramBegin = (size_t)&kernelEnd;
	for(int *n = (int*)ramBegin; n != (int*)ramEnd; n++)
		*n = 0;
}

uint8_t* malloc(size_t size)
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
	uint8_t *ptr = (uint8_t*)last;
	last += size;
	
	return ptr;
}

void free(uint8_t *ptr)
{
	size_t *size = (size_t*)(ptr-SIZE);
	for(uint8_t *end = ptr+*size; ptr != end; ptr++)
		*ptr = 0;
	
	*size = 0;
}
