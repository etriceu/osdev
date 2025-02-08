/*
 * Currently, the GDT is set in the bootloader (boot.s), and this code is not used.
 * I kept it in case someone needs it.
 */

#include "../include/gdt.h"

GDT gdt[3];
GDTRegister gdtReg;

extern void gdtFlush(GDTRegister *gdtReg);

void setGDT(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	gdt[num].baseLow = base & 0xffff;
	gdt[num].baseMid = (base>>16) & 0xff;
	gdt[num].baseHigh = (base>>24) & 0xff;
	gdt[num].limit = limit & 0xffff;
	gdt[num].granularity = (limit>>16) & 0x0f;
	gdt[num].granularity |= gran & 0xf0;
	gdt[num].access = access;
}

void gdtInit()
{
	setGDT(0, 0, 0, 0, 0);
	setGDT(1, 0, 0xffffffff, 0x9a, 0xcf);
	setGDT(2, 0, 0xffffffff, 0x92, 0xcf);
	
	gdtReg.limit = sizeof(GDT)*3 - 1;
	gdtReg.base = (uint32_t)&gdt;

	gdtFlush(&gdtReg);
}
