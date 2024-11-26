#include "shell.h"
#include "funcs.h"

#include "../../include/userCall.h"
#include "../../include/exec.h"
#include "../../include/myfs.h"
#include "../../include/malloc.h"
#include "../../include/screen.h"

const char* NAMES[FUNC_COUNT] = {
	"", //nop
	"set", "mov", "ld", "st",
	"add", "sub", "mul", "div", "mod",
	"and", "or", "xor", "not",
	"eq", "lt",
	"jmp", "jnz",
	"int",
	"run"
};

static struct Var variables[VAR_COUNT];

struct Var* getVar(char** code, const char* end)
{
	if(*code < end)
	{
		int n = (**code) - 'a';
		if(n < 0 || n >= VAR_COUNT)
			n = 0;
		
		*code += 2;
		return variables+n;
	}
		
	return variables;
}

struct Var parseNumber(char** code, const char* end)
{
	struct Var ret;
	ret.val.i = 0;
	ret.isFloat = 0;
	if(*code >= end)
		return ret;
	
	int sign = 1;
	if(**code == '-')
	{
		sign = -1;
		(*code)++;
	}
	
	double tmp = 1;
	for(; *code < end && **code != EOL; (*code)++)
	{
		if(**code >= '0' && **code <= '9')
		{
			ret.val.i *= 10;
			ret.val.i += (**code) - '0';
			if(ret.isFloat)
				tmp *= 10;
		}
		else if(**code == '.')
		{
			ret.isFloat = 1;
		}
		else
			break;
	}
	(*code)++;
	
	ret.val.i *= sign;
	if(ret.isFloat)
		ret.val.f = ret.val.i/tmp;
	
	return ret;
}

void _nop(char** code, const char* end, const char* start)
{
	if(**code >= '!' && **code <= '~')
	{
		uint16_t style = getStyle();
		setStyle(FG_RED);
		print("\"");
		print(*code);
		print("\" is not recognized as a command.\n");
		setStyle(FG_YELLOW);
		print("You can use the 'run' command to run a file.\n"
			"Usage: run \"filename\" [\"arg1\" \"arg2\" ... \"argN\"]\n"
		);
		struct mount *mnt = getMountPoints();
		if(mnt)
		{
			struct node *node = mnt->nodes;
			if(node)
			{
				setStyle(FG_GREEN);
				print("Available files:\n");
				for(; node; node = node->next)
				{
					print(node->name);
					if(node->next)
						print(", ");
				}
				print("\n");
			}
		}
		setStyle(style);
		*code = (char*)end;
	}
}

void _set(char** code, const char* end, const char* start)
{
	struct Var *v = getVar(code, end);
	*v = parseNumber(code, end);
}

void _mov(char** code, const char* end, const char* start)
{
	struct Var *v = getVar(code, end);
	*v = *getVar(code, end);
}

void _ld(char** code, const char* end, const char* start)
{
	char type = **code;
	*code += 2;
	struct Var *v1 = getVar(code, end);
	struct Var *v2 = getVar(code, end);
	
	size_t tmp = v2->val.i;
	if(v2->isFloat)
		tmp = v2->val.f;
	void *ptr = (void*)tmp;
	switch(type)
	{
		case 'b':
			v1->val.i = *(int8_t*)ptr;
			v1->isFloat = 0;
			break;
		case 'w':
			v1->val.i = *(int16_t*)ptr;
			v1->isFloat = 0;
			break;
		case 'q':
			v1->val.i = *(int64_t*)ptr;
			v1->isFloat = 0;
			break;
		case 'f':
			v1->val.f = *(double*)ptr;
			v1->isFloat = 1;
			break;
		default: //l
			v1->val.i = *(int32_t*)ptr;
			v1->isFloat = 0;
	}
}

void _st(char** code, const char* end, const char* start)
{
	char type = **code;
	*code += 2;
	struct Var *v1 = getVar(code, end);
	struct Var *v2 = getVar(code, end);
	
	size_t tmp = v2->val.i;
	if(v2->isFloat)
		tmp = v2->val.f;
	void *ptr = (void*)tmp;
	
	union
	{
		uint64_t i;
		double f;
	} val;
	
	val.i = v1->val.i;
	if(v1->isFloat && type != 'f')
		val.i = val.f;
	else if(!v1->isFloat && type == 'f')
		val.f = val.i;
	
	switch(type)
	{
		case 'b': *(int8_t*)ptr = val.i; break;
		case 'w': *(int16_t*)ptr = val.i; break;
		case 'q': *(int64_t*)ptr = val.i; break;
		case 'f': *(double*)ptr = val.f; break;
		default: *(int32_t*)ptr = val.i; //l
	}
}

void _add(char** code, const char* end, const char* start)
{
	struct Var *v1 = getVar(code, end);
	struct Var *v2 = getVar(code, end);
	
	double a = v1->val.f;
	if(!v1->isFloat)
		a = v1->val.i;
	
	double b = v2->val.f;
	if(!v2->isFloat)
		b = v2->val.i;
	
	v1->isFloat = 1;
	v1->val.f = a+b;
}

void _sub(char** code, const char* end, const char* start)
{
	struct Var *v1 = getVar(code, end);
	struct Var *v2 = getVar(code, end);
	
	double a = v1->val.f;
	if(!v1->isFloat)
		a = v1->val.i;
	
	double b = v2->val.f;
	if(!v2->isFloat)
		b = v2->val.i;
	
	v1->isFloat = 1;
	v1->val.f = a-b;
}

void _mul(char** code, const char* end, const char* start)
{
	struct Var *v1 = getVar(code, end);
	struct Var *v2 = getVar(code, end);
	
	double a = v1->val.f;
	if(!v1->isFloat)
		a = v1->val.i;
	
	double b = v2->val.f;
	if(!v2->isFloat)
		b = v2->val.i;
	
	v1->isFloat = 1;
	v1->val.f = a*b;
}

void _div(char** code, const char* end, const char* start)
{
	struct Var *v1 = getVar(code, end);
	struct Var *v2 = getVar(code, end);
	
	double a = v1->val.f;
	if(!v1->isFloat)
		a = v1->val.i;
	
	double b = v2->val.f;
	if(!v2->isFloat)
		b = v2->val.i;
	
	v1->isFloat = 1;
	v1->val.f = a/b;
}

void _mod(char** code, const char* end, const char* start)
{
	struct Var *v1 = getVar(code, end);
	struct Var *v2 = getVar(code, end);
	
	int32_t a = v1->val.i;
	if(v1->isFloat)
		a = v1->val.f;
	
	int32_t b = v2->val.i;
	if(v2->isFloat)
		b = v2->val.f;
	
	v1->isFloat = 0;
	v1->val.i = a%b;
}

void _and(char** code, const char* end, const char* start)
{
	struct Var *v1 = getVar(code, end);
	struct Var *v2 = getVar(code, end);
	
	uint64_t a = v1->val.i;
	if(v1->isFloat)
		a = v1->val.f;
	
	uint64_t b = v2->val.i;
	if(v2->isFloat)
		b = v2->val.f;
	
	v1->isFloat = 0;
	v1->val.i = a&b;
}

void _or(char** code, const char* end, const char* start)
{
	struct Var *v1 = getVar(code, end);
	struct Var *v2 = getVar(code, end);
	
	uint64_t a = v1->val.i;
	if(v1->isFloat)
		a = v1->val.f;
	
	uint64_t b = v2->val.i;
	if(v2->isFloat)
		b = v2->val.f;
	
	v1->isFloat = 0;
	v1->val.i = a|b;
}

void _xor(char** code, const char* end, const char* start)
{
	struct Var *v1 = getVar(code, end);
	struct Var *v2 = getVar(code, end);
	
	uint64_t a = v1->val.i;
	if(v1->isFloat)
		a = v1->val.f;
	
	uint64_t b = v2->val.i;
	if(v2->isFloat)
		b = v2->val.f;
	
	v1->isFloat = 0;
	v1->val.i = a^b;
}

void _not(char** code, const char* end, const char* start)
{
	struct Var *v1 = getVar(code, end);
	
	uint64_t a = v1->val.i;
	if(v1->isFloat)
		a = v1->val.f;
	
	v1->isFloat = 0;
	v1->val.i = ~a;
}

void _eq(char** code, const char* end, const char* start)
{
	struct Var *v1 = getVar(code, end);
	struct Var *v2 = getVar(code, end);
	
	double a = v1->val.f;
	if(!v1->isFloat)
		a = v1->val.i;
	
	double b = v2->val.f;
	if(!v2->isFloat)
		b = v2->val.i;
	
	v1->isFloat = 0;
	v1->val.i = a == b;
}

void _lt(char** code, const char* end, const char* start)
{
	struct Var *v1 = getVar(code, end);
	struct Var *v2 = getVar(code, end);
	
	double a = v1->val.f;
	if(!v1->isFloat)
		a = v1->val.i;
	
	double b = v2->val.f;
	if(!v2->isFloat)
		b = v2->val.i;
	
	v1->isFloat = 0;
	v1->val.i = a < b;
}

void _jmp(char** code, const char* end, const char* start)
{
	char *str = *code;
	*code = (char*) start;
	for(; *code < end; (*code)++)
		if(**code == ':' && strcmp((*code)+1, end, str))
			return;
	
	*code = (char*)end;
}

void _jnz(char** code, const char* end, const char* start)
{
	struct Var *v1 = getVar(code, end);
	char *str = *code;
	if(v1->val.i)
	{
		*code = (char*)start;
		for(; *code < end; (*code)++)
			if(**code == ':' && strcmp((*code)+1, end, str))
				return;
		
		*code = (char*)end;
	}
}

void _int(char** code, const char* end, const char* start)
{
	struct Var id = parseNumber(code, end);
	struct Var *vr = getVar(code, end);
	struct Var *v1 = getVar(code, end);
	struct Var *v2 = getVar(code, end);
	struct Var *v3 = getVar(code, end);
	struct Var *v4 = getVar(code, end);
	
	vr->val.i = call(id.val.i, v1->val.i, v2->val.i, v3->val.i, v4->val.i);
}

void _run(char** code, const char* end, const char* start)
{
	int argc = 0;
	char **argv = NULL;
	for(; **code != EOL && **code != '\0' && *code < end; argc++)
	{
		argv = realloc(argv, (argc+1)*sizeof(char*));
		if(**code < '0' || **code > '9')
			argv[argc] = (char*)(size_t)getVar(code, end)->val.i;
		else
			argv[argc] = (char*)(size_t)parseNumber(code, end).val.i;
	}

	if(argc)
	{
		struct mount *mnt = getMountPoints();
		if(mnt)
		{
			struct node *node = findFile(mnt, argv[0]);
			if(node)
			{
				struct file *file = fopen(node);
				fseek(file, -1);
				size_t size = ftell(file);
				fseek(file, 0);
				char *elfCode = malloc(size);
				fread(file, size, (uint8_t*)elfCode);
				fclose(file);
				
				elfRun((uint8_t*)elfCode, argc, argv);
				
				free(elfCode);
			}
			else
			{
				print("File '");
				print(argv[1]);
				print("' not found.\n");
			}
		}
		else
			print("No mount points.\n");
	
		free(argv);
	}
}

const void (*FUNCS[FUNC_COUNT])(char**, const char*, const char*) = {
	_nop,
	_set, _mov, _ld, _st,
	_add, _sub, _mul, _div, _mod,
	_and, _or, _xor, _not,
	_eq, _lt,
	_jmp, _jnz,
	_int,
	_run
};
