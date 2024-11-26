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

_PREFIX void* malloc(size_t size)
#ifndef KERNEL
{return (uint8_t*)call(6, size, 0, 0, 0);}
#endif
;
_PREFIX void* realloc(void *ptr, size_t size)
#ifndef KERNEL
{return (uint8_t*)call(33, (uint32_t)ptr, size, 0, 0);}
#endif
;
_PREFIX void free(void *ptr)
#ifndef KERNEL
{call(7, (uint32_t)ptr, 0, 0, 0);}
#endif
;

#endif // MALLOC_H_INCLUDED
