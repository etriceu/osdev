#ifndef IDT_H
#define IDT_H

#include <stdint.h>

typedef struct
{
	uint16_t low;
	uint16_t sel;
	uint8_t zero;
	uint8_t flags; 
	uint16_t high;
} __attribute__((packed)) IDT;

typedef struct
{
	uint16_t limit;
	uint32_t base;
} __attribute__((packed)) IDTRegister;

void idtInit();

#endif // IDT_H_INCLUDED
