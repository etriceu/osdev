#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stddef.h>
#include <stdint.h>

static inline void out(uint16_t port, uint8_t data)
{
	asm volatile("out %1, %0" : : "d" (port), "a" (data));
}

static inline void out2(uint16_t port, uint16_t data)
{
	asm volatile("outw %1, %0" : : "d" (port), "a" (data));
}

static inline uint8_t in(uint16_t port)
{
	uint8_t ret;
	asm volatile ("inb %1, %0" : "=a" (ret) : "d" (port));
	return ret;
}

static inline uint16_t in2(uint16_t port)
{
	uint16_t ret;
	asm volatile ("inw %1, %0" : "=a" (ret) : "d" (port));
	return ret;
}

static inline int8_t strcmp(const char* a, const char* b)
{
	for(; *a != '\0' && *b != '\0'; a++, b++)
		if(*a != *b)
			return 0;
		
	return *a == *b;
}

#endif // FUNCTIONS_H_INCLUDED
