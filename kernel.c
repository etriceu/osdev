#include "include/screen.h"

extern void kernelMain(void* multiboot_structure, unsigned int magicnumber)
{
	setStyle(FG_PURPLE);
	print("MyOS :)\n\n");
	setStyle(FG_GREY);
}
