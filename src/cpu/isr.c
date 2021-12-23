#include "../include/isr.h"
#include "../include/screen.h"
#include "../include/systemCall.h"

char *exceptions[] = {
	"Divide by zero Error",
	"Debug",
	"Non-maskable Interrupt",
	"Breakpoint",
	"Overflow",
	"Bound Range Exceeded",
	"Invalid Opcode",
	"Device Not Available",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Invalid TSS",
	"Segment Not Present",
	"Stack-Segment Fault",
	"General Protection Fault",
	"Page Fault",
	"Reserved (15)",
	"x87 Floating-Point Exception",
	"Alignment Check",
	"Machine Check",
	"SIMD Floating-Point Exception",
	"Virtualization Exception",
	"Reserved (21)",
	"Reserved (22)",
	"Reserved (23)",
	"Reserved (24)",
	"Reserved (25)",
	"Reserved (26)",
	"Reserved (27)",
	"Reserved (28)",
	"Reserved (29)",
	"Security Exception",
	"Reserved (31)"
};

void isrHandler(Registers reg)
{
	if(reg.id == 128)
	{
		systemCall(reg);
		return;
	}
	
	print("\nISR: ");
	print(exceptions[reg.id]);
	print("\n");
}
