#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>
#include <stdint.h>

void memInit();
uint8_t* malloc(size_t size);
void free(uint8_t *ptr);

#endif // MALLOC_H_INCLUDED
