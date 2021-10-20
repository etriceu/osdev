#include "../include/system.h"
#include "../include/functions.h"

void system(const char *cmd)
{
	if(strcmp(cmd, "exit"))
	{
		print("Bye\n");
		asm volatile("hlt");
	}
	else if(strcmp(cmd, "clear"))
		clear();
	else
		print("commands:\nexit, clear\n");
}
