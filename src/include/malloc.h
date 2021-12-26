#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>
#include <stdint.h>

void memInit();
void* malloc(size_t size);
void free(void *ptr);

#endif // MALLOC_H_INCLUDED
