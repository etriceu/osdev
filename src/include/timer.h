#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#ifndef KERNEL
#include "userCall.h"
#define _PREFIX static inline
#else
#define _PREFIX
#endif

#define TIMER_FREQ 200

#ifdef KERNEL
void timerInit();
void timer();
#endif

/**
 * Retrieves the current value of the system's tick counter.
 *
 * @return The current tick count.
 */
_PREFIX uint32_t getTicks()
#ifndef KERNEL
{return call(34, 0, 0, 0, 0);}
#endif
;

/**
 * Pauses the execution for the specified amount of time.
 *
 * @param ms The duration to sleep, in milliseconds.
 */
_PREFIX void sleep(uint32_t ms)
#ifndef KERNEL
{call(35, ms, 0, 0, 0);}
#endif
;

#endif // TIMER_H_INCLUDED
