#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void out(uint16_t port, uint8_t data)
{
	asm volatile("out %%al, %%dx" : : "a" (data), "d" (port));
}

unsigned char in(uint16_t port)
{
	unsigned char ret;
	asm volatile ("inb %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}

#endif // FUNCTIONS_H_INCLUDED
