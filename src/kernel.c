#include "include/keyboard.h"
#include "include/system.h"
#include "include/malloc.h"
#include "include/myfs.h"
#include "include/gdt.h"
#include "include/idt.h"
#include "include/systemCall.h"
#include "include/irq.h"
#include "include/timer.h"

extern void kernelMain()
{
	memInit();
	gdtInit();
	idtInit();
	irqInit();
	timerInit();
	irqEnable();
	
	translateKeyCodes();
	
	clear();
	
	setStyle(FG_PURPLE);
	print("MyOS :)\n\n");
	setStyle(FG_GREY);
	
	char *buf = malloc(512);
	ataRead(PRIMARY_MASTER, 1, 1, buf);
	uint32_t end = *((uint32_t*)(buf+509)) & 0x00ffffff;//read the end of myfs
	free(buf);
	
	mount(PRIMARY_MASTER, 1, end);
	
	char cmd[VIDEO_WIDTH];
	int cmdi = 0;

	while(1)
	{
		for(uint8_t key = pollKeys(); key != KEY_NONE; key = pollKeys())
		{
			if(key < KEY_SPECIAL)
				key = KEYS[key];

			if(key == KEY_ENTER)
			{
				print("\n");
				cmd[cmdi] = '\0';
				cmdi = 0;
				system(cmd);
			}
			else if(key == KEY_BACKSPACE && cmdi > 0)
			{
				cmdi--;
				moveCursor(getCursorPos()-1);
				setChar(getCursorPos(), 0, getStyle());
				cmd[cmdi] = 0;
			}
			else if(key <= '~')
			{
				if((getKeyStatus(keyID(KEY_LSHIFT)) ||
					getKeyStatus(keyID(KEY_RSHIFT))) &&
					key >= 'a' && key <= 'z')
					key -= 32;
				
				if(cmdi < VIDEO_WIDTH)
				{
					printn(&key, 1);
					cmd[cmdi] = key;
					cmdi++;
				}
			}
		}
		sleep(10);
	}
}
