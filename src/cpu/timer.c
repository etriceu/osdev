#include "../include/timer.h"
#include "../include/functions.h"

void timerInit()
{
	uint16_t divisor = (7159090 + 6/2)/(6*TIMER_FREQ);
	out(0x43, 0x36);
	out(0x40, divisor & 0xFF);
	out(0x40, divisor >> 8);
}

static uint32_t ticks = 0;

void timer()
{
	ticks++;
}

uint32_t getTicks()
{
	return ticks;
}

void sleep(uint32_t s)
{
	uint32_t end = ticks + s*TIMER_FREQ/1000;
	asm volatile("sti");
	while(end > ticks)
		asm volatile("hlt");
}
