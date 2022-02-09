#include "../include/timer.h"
#include "../include/functions.h"

void timerInit()
{
	uint16_t divisor = (7159090 + 6/2)/(6*TIMER_FREQ);
	out(0x43, 0x36);
	out(0x40, divisor & 0xFF);
	out(0x40, divisor >> 8);
}

uint32_t ticks = 0;

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
	uint32_t end = getTicks() + s*TIMER_FREQ/1000;
	while(end > getTicks())
		asm("nop");
}
