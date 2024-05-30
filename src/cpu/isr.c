#include "../include/isr.h"
#include "../include/screen.h"
#include "../include/systemCall.h"
#include "../include/irq.h"
#include "../include/keyboard.h"
#include "../include/timer.h"
#include "../include/functions.h"

static const char *exceptions[] = {
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

void isrHandler(Registers *reg)
{
	if(reg->id == 128)
		systemCall(reg);
	else if(reg->id >= 32)
	{
		if(reg->id == 32)
			timer();
		else if(reg->id == 33)
			keyboard();
			
		if(reg->id >= 40)
			out(0xa0, 0x20);
	
		out(0x20, 0x20);
	}
	else if(reg->id < 32)
	{
		print("\nISR: ");
		print(exceptions[reg->id]);
		print("\n");
		asm("hlt");
	}
}
