#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>
#include <stdint.h>

#ifndef KERNEL
#include "userCall.h"
#endif

#ifdef KERNEL
void memInit();
#endif
void* malloc(size_t size)
#ifndef KERNEL
{return (uint8_t*)call(6, size, 0, 0, 0);}
#endif
;
void free(void *ptr)
#ifndef KERNEL
{call(7, (uint32_t)ptr, 0, 0, 0);}
#endif
;

#endif // MALLOC_H_INCLUDED
