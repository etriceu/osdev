#ifndef USER_CALL_H
#define USER_CALL_H

#include <stdint.h>

/**
 * Makes a system call identified by the given ID with the specified parameters.
 * The number and type of parameters, as well as the return value, depend on the specific system call.
 *
 * @param id The identifier of the system call to invoke.
 * @param a The first argument.
 * @param b The second argument.
 * @param c The third argument.
 * @param d The fourth argument.
 * @return The return value of the system call, or 0 if the system call does not return a value.
 */

static uint32_t call(uint32_t id, uint32_t a, uint32_t b, uint32_t c, uint32_t d)
{
	uint32_t ret = 0;
	uint32_t args[] = {id, (uint32_t)&ret, a, b, c, d};
	asm volatile("mov %0, %%ebx;" :: "a" (args) : "%ebx");
	asm volatile("int $128");
	return ret;
}

#endif // USER_CALL_H_INCLUDED
