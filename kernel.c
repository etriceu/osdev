#include "include/screen.h"
#include "include/keyboard.h"

extern void kernelMain(void* multiboot_structure, unsigned int magicnumber)
{
	setStyle(FG_PURPLE);
	print("MyOS :)\n\n");
	setStyle(FG_GREY);
	
	while(1)
	{
		if(isKeyEvent())
		{
			uint8_t key = getKey();
			if(getKeyStatus(key))
			{
				if(keys[key] == KEY_ENTER)
					print("\n");
				else if(keys[key] <= '~')
					printn(&keys[key], 1);
			}
		}
	}
}
