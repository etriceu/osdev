#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>
#include <stdint.h>

#ifndef KERNEL
#include "userCall.h"
#define _PREFIX static inline
#else
#define _PREFIX
#endif

#ifdef KERNEL
void memInit();
#endif

/**
 * Allocates a block of memory of the specified size.
 *
 * @param size The amount of memory, in bytes, to allocate.
 * @return A pointer to the allocated memory, or NULL in case of an error.
 */
_PREFIX void* malloc(size_t size)
#ifndef KERNEL
{return (uint8_t*)call(6, size, 0, 0, 0);}
#endif
;

/**
 * Resizes a previously allocated memory block.
 *
 * @param ptr Pointer to the previously allocated memory block.
 * @param size The new size, in bytes, for the memory block.
 * @return A pointer to the allocated memory, or NULL in case of an error.
 */
_PREFIX void* realloc(void *ptr, size_t size)
#ifndef KERNEL
{return (uint8_t*)call(33, (uint32_t)ptr, size, 0, 0);}
#endif
;

/**
 * Frees a previously allocated block of memory.
 *
 * @param ptr Pointer to the previously allocated memory block.
 */
_PREFIX void free(void *ptr)
#ifndef KERNEL
{call(7, (uint32_t)ptr, 0, 0, 0);}
#endif
;

#endif // MALLOC_H_INCLUDED
