#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#define TIMER_FREQ 200

void timerInit();
void timer();
uint32_t getTicks();
void sleep(uint32_t s);

#endif // TIMER_H_INCLUDED
