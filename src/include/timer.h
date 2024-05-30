#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#ifndef KERNEL
#include "userCall.h"
#endif

#define TIMER_FREQ 200

#ifdef KERNEL
void timerInit();
void timer();
#endif

uint32_t getTicks()
#ifndef KERNEL
{return call(34, 0, 0, 0, 0);}
#endif
;
void sleep(uint32_t ms)
#ifndef KERNEL
{call(35, ms, 0, 0, 0);}
#endif
;

#endif // TIMER_H_INCLUDED
