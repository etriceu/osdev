#include "include/keyboard.h"
#include "include/system.h"
#include "include/ata.h"

extern void kernelMain(void* multiboot_structure, unsigned int magicnumber)
{
	setStyle(FG_PURPLE);
	print("MyOS :)\n\n");
	setStyle(FG_GREY);
	
	const int cmdSize = 16;
	char cmd[cmdSize];
	int cmdi = 0;
	
	
	uint8_t tab[512];
	
	const char* text = "test123";
	
	for(int n = 0; text[n] != '\0'; n++)
		tab[n] = text[n];
	
	ataWrite(0, 1, tab);
	
	for(int n = 0; n < 512; n++)
		tab[n] = 0;
	

	ataRead(0, 1, tab);
	print(tab);
	
	
	while(1)
	{
		if(isKeyEvent())
		{
			uint8_t key = getKey();
			if(getKeyStatus(key))
			{
				key = keys[key];
				if(key == KEY_ENTER)
				{
					print("\n");
					cmd[cmdi] = '\0';
					cmdi = 0;
					system(cmd);
				}
				else if(key <= '~')
				{
					printn(&key, 1);
					if(cmdi < cmdSize)
					{
						cmd[cmdi] = key;
						cmdi++;
					}
				}
			}
		}
	}
}
