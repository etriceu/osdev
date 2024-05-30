#ifndef SYSTEM_H
#define SYSTEM_H

#ifndef KERNEL
#include "userCall.h"
#endif

void system(const char *cmd)
#ifndef KERNEL
{call(33, (uint32_t)cmd, 0, 0, 0);}
#endif
;

#endif // SYSTEM_H_INCLUDED
