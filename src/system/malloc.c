#include "../include/malloc.h"
#define SIZE sizeof(size_t)

#define RAM_BEG (void*)0x100000
#define RAM_END 0x40000000 //(1GB) //idk how to check ram size

size_t *last;

void memInit()
{
	last = RAM_BEG;
	for(size_t *n = (size_t*)RAM_BEG; n < (size_t*)RAM_END; n++)
		*n = 0;
}

void* malloc(size_t size)
{
	if(size == 0)
		return NULL;
	
	size = (size+SIZE-1)/SIZE;
	
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
		
		if(s > size)
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

void* realloc(void *ptr, size_t size)
{
	if(ptr == NULL)
		return malloc(size);

	if(size == 0)
	{
		free(ptr);
		return NULL;
	}
	
	size = (size+SIZE-1)/SIZE;

	size_t *oldSize = (size_t*)ptr-1;

	if(*oldSize == size)
		return ptr;

	if(*oldSize > size)
	{
		for(size_t *it = (size_t*)ptr+size; it < (size_t*)ptr+*oldSize; it++)
			*it = 0;
		*oldSize = size;
	}
	else
	{
		uint8_t isFree = 1;
		for(size_t *it = (size_t*)ptr+*oldSize; it < (size_t*)ptr+size; it++)
			if(it >= (size_t*)RAM_END || *it != 0)
			{
				isFree = 0;
				break;
			}

		if(isFree)
		{
			if((size_t*)ptr+*oldSize == last)
				last = (size_t*)ptr+size;
			*oldSize = size;
		}
		else
		{
			size_t *tmp = (size_t*)malloc(size*SIZE);
			if(tmp == NULL)
				return NULL;

			for(size_t *src = ptr, *des = tmp; src < (size_t*)ptr+*oldSize; src++, des++)
				*des = *src;

			free(ptr);
			return tmp;
		}
	}

	return ptr;
}

void free(void *ptr)
{
	size_t *end = (size_t*)ptr + *((size_t*)ptr-1);
	for(size_t *it = (size_t*)ptr-1; it < end; it++)
		*it = 0;
}
