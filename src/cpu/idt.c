#include "../include/idt.h"
#include "../include/isr.h"
#include "../include/malloc.h"
#include "../include/functions.h"

IDT idt[256];
IDTRegister idtReg;

void setIDT(int n, uint32_t handler)
{
	idt[n].low = handler & 0xffff;
	idt[n].sel = 8;
	idt[n].zero = 0;
	idt[n].flags = 0x8e;
	idt[n].high = (handler>>16) & 0xffff;
}

void idtInit()
{
	setIDT(0, (uint32_t)isr0);
	setIDT(1, (uint32_t)isr1);
	setIDT(2, (uint32_t)isr2);
	setIDT(3, (uint32_t)isr3);
	setIDT(4, (uint32_t)isr4);
	setIDT(5, (uint32_t)isr5);
	setIDT(6, (uint32_t)isr6);
	setIDT(7, (uint32_t)isr7);
	setIDT(8, (uint32_t)isr8);
	setIDT(9, (uint32_t)isr9);
	setIDT(10, (uint32_t)isr10);
	setIDT(11, (uint32_t)isr11);
	setIDT(12, (uint32_t)isr12);
	setIDT(13, (uint32_t)isr13);
	setIDT(14, (uint32_t)isr14);
	setIDT(15, (uint32_t)isr15);
	setIDT(16, (uint32_t)isr16);
	setIDT(17, (uint32_t)isr17);
	setIDT(18, (uint32_t)isr18);
	setIDT(19, (uint32_t)isr19);
	setIDT(20, (uint32_t)isr20);
	setIDT(21, (uint32_t)isr21);
	setIDT(22, (uint32_t)isr22);
	setIDT(23, (uint32_t)isr23);
	setIDT(24, (uint32_t)isr24);
	setIDT(25, (uint32_t)isr25);
	setIDT(26, (uint32_t)isr26);
	setIDT(27, (uint32_t)isr27);
	setIDT(28, (uint32_t)isr28);
	setIDT(29, (uint32_t)isr29);
	setIDT(30, (uint32_t)isr30);
	setIDT(31, (uint32_t)isr31);
	
	setIDT(32, (uint32_t)irqTimer);
	setIDT(33, (uint32_t)irqKbd);
	
	setIDT(128, (uint32_t)int128);
	
	idtReg.base = (uint32_t)&idt;
	idtReg.limit = 256*sizeof(IDT)-1;
	
	asm volatile("lidt (%0)" : : "r" (&idtReg));
}
