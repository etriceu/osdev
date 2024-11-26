#ifndef FUNCS_H
#define FUNCS_H

#include <stdint.h>

#define FUNC_COUNT 20
#define VAR_COUNT 26

extern const char* NAMES[FUNC_COUNT];
extern const void (*FUNCS[FUNC_COUNT])(char**, const char*, const char*);

struct Var
{
	union{
		int64_t i;
		double f;
	} val;
	uint8_t isFloat;
};



#endif // FUNCS_H_INCLUDED
