#include "../include/malloc.h"

#define SIZE sizeof(size_t)

#define RAM_BEG (void*)0x100000
#define RAM_END 0x40000000 //(1GB) //idk how to check ram size

static size_t *last = RAM_BEG;

void memInit()
{
	for(size_t *n = (size_t*)RAM_BEG; n < (size_t*)RAM_END; n++)
		*n = 0;
}

void* malloc(size_t size)
{
	if(size == 0)
		return NULL;
	
	size_t s = 0;
	int try = 0;
	while(1)
	{
		if(last >= (size_t*)RAM_END)
		{
			last = RAM_BEG;
			s = 0;
			if(++try > 3)
				return NULL;
		}
		
		if(*(size_t*)last == 0)
			s++;
		else
		{
			s = 0;
			last += *(size_t*)last;
		}
		
		if(s*SIZE >= size+SIZE)
			break;

		last++;
	}
	s--;
	last -= s;
	*(size_t*)last = s;
	last++;
	void *ptr = last;
	last += s;
	
	return ptr;
}

void free(void *ptr)
{
	size_t *end = *(size_t*)(ptr-SIZE)*SIZE + ptr;
	for(size_t *it = ptr-SIZE; it < end; it++)
		*it = 0;
}
