#include "include/keyboard.h"
#include "include/system.h"
#include "include/malloc.h"
#include "include/myfs.h"
#include "include/gdt.h"
#include "include/idt.h"
#include "include/systemCall.h"
#include "include/irq.h"
#include "include/timer.h"
#include "include/exec.h"

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
	
	uint8_t *buf = malloc(MY_FS_SECTOR_SIZE);
	ataRead(PRIMARY_MASTER, 1, 1, buf);
	
	uint32_t myfsStart = *((uint32_t*)(buf+4));
	uint32_t myfsEnd = *((uint32_t*)(buf+8));
	free(buf);
	
	if(myfsStart && myfsEnd && myfsStart < myfsEnd &&
		mount(PRIMARY_MASTER, myfsStart, myfsEnd))
	{
		struct mount *mnt = getMountPoints();
		struct node *node = findFile(mnt, "init");
		if(node != NULL)
		{
			struct file *file = fopen(node);
			fseek(file, -1);
			size_t size = ftell(file);
			fseek(file, 0);
			char *name = malloc(size+1);
			fread(file, size, name);
			fclose(file);
			
			//rtrim
			for(size_t n = size; n > 0 && (name[n] < ' ' || name[n] > '~'); n--)
				name[n] = '\0';
			name[size] = '\0';
			
			node = findFile(mnt, name);
			if(node)
			{
				free(name);
				file = fopen(node);
				fseek(file, -1);
				size = ftell(file);
				fseek(file, 0);
				uint8_t *code = malloc(size);
				fread(file, size, code);
				fclose(file);
				elfRun(code, 0, NULL);
				free(code);
			}
			else
			{
				print("File '");
				print(name);
				print("' not found.\n");
				free(name);
			}
		}
		else
			print("File 'init' not found.\n");
	}
	else
		print("Partition not found.\n");

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
					printn((const char*)&key, 1);
					cmd[cmdi] = key;
					cmdi++;
				}
			}
		}
		sleep(10);
	}

	asm volatile("hlt");
}
