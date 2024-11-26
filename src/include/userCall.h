#ifndef USER_CALL_H
#define USER_CALL_H

#include <stdint.h>

static uint32_t call(uint32_t id, uint32_t a, uint32_t b, uint32_t c, uint32_t d)
{
	uint32_t ret = 0;
	uint32_t args[] = {id, (uint32_t)&ret, a, b, c, d};
	asm volatile("mov %0, %%ebx;" :: "a" (args) : "%ebx");
	asm volatile("int $128");
	return ret;
}

#endif // USER_CALL_H_INCLUDED
