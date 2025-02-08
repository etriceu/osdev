/*
 * Currently, the GDT is set in the bootloader (boot.s), and this code is not used.
 * I kept it in case someone needs it.
 */


#ifndef GDT_H
#define GDT_H

#include <stdint.h>

typedef struct
{
	uint16_t limit;
	uint16_t baseLow;
	uint8_t baseMid;
	uint8_t access;
	uint8_t granularity;
	uint8_t baseHigh;
} __attribute__((packed)) GDT;

typedef struct
{
	uint16_t limit;
	uint32_t base;
} __attribute__((packed)) GDTRegister;

void gdtInit();

#endif // GDT_H_INCLUDED
