#include "../include/system.h"
#include "../include/functions.h"
#include "../include/malloc.h"
#include "../include/myfs.h"
#include "../include/ata.h"
#include "../include/exec.h"
#include "../include/irq.h"
#include "../include/screen.h"

uint8_t checkMnt()
{
	if(!getMountPoints())
	{
		setStyle(FG_RED);
		print("No available mount point\n");
		setStyle(FG_GREEN);
		return 0;
	}
	return 1;
}

void system(const char *cmd)
{
	setStyle(FG_GREEN);
	char **args = NULL;
	int argc = 0;
	volatile int size = 0;
	for(; cmd[size] != 0; size++);
	size++;
	char *cmd2 = malloc(size);

	for(int n = 0; n < size; n++)
	{
		cmd2[n] = cmd[n];
		if(cmd2[n] == ' ')
		{
			argc++;
			cmd2[n] = 0;
		}
	}

	if(argc)
	{
		args = malloc(sizeof(char*)*argc);
		for(int n = 0, a = 0; n < size-1; n++)
			if(cmd2[n] == 0)
			{
				args[a] = &cmd2[n+1];
				a++;
			}
	}

	if(strcmp(cmd, "exit"))
	{
		print("Bye\n");
		
		irqDisable();
		asm volatile("hlt");
	}
	else if(strcmp(cmd, "clear"))
		clear();
	else if(strcmp(cmd2, "newfile"))
	{
		if(checkMnt())
		{
			if(argc == 1)
			{
				size_t s = 0;
				for(char *it = args[0]; *it != '\0'; it++, s++);
				newFile(getMountPoints(), args[0], s); // get first mnt
			}
			else
				print("newfile [fileName]\n");
		}
	}
	else if(strcmp(cmd2, "rename"))
	{
		if(argc == 2)
		{
			if(checkMnt())
			{
				size_t s = 0;
				for(char *it = args[1]; *it != '\0'; it++, s++);
				struct node *nod = findFile(getMountPoints(), args[0]);
				if(nod)
					renameFile(nod, args[1], s);
				else
					print("File not found.\n");
			}
		}
		else
			print("rename [fileName] [newName]\n");
	}
	else if(strcmp(cmd2, "rm"))
	{
		if(argc == 1)
		{
			if(checkMnt())
			{
				struct node *nod = findFile(getMountPoints(), args[0]);
				if(nod)
					removeFile(nod);
				else
					print("File not found.\n");
			}
		}
		else
			print("rm [fileName]\n");
	}
	else if(strcmp(cmd, "ls"))
	{
		if(checkMnt())
		{
			for(struct node *nod = getMountPoints()->nodes; nod != 0; nod = nod->next)
			{
				char *name = getFileName(nod, NULL);
				print(name);
				print("\n");
			}
		}
	}
	else if(strcmp(cmd, "format")) // format first device
	{
		char *zero = malloc(512*256);
		uint32_t n = 0;
		
		print("formating... ");
		uint32_t diskSize = ataGetSize(PRIMARY_MASTER);
		for(; n <= diskSize; n += 255)
			ataWrite(PRIMARY_MASTER, n, 255, (uint8_t*)zero);
		
		ataWrite(PRIMARY_MASTER, n, diskSize-n*255, (uint8_t*)zero);
		
		print("done\n");
		free(zero);
	}
	else if(strcmp(cmd, "mount")) // default mount
		mount(PRIMARY_MASTER, 1, ataGetSize(PRIMARY_MASTER));
	else if(strcmp(cmd, "umount"))
	{
		if(checkMnt())
			umount(getMountPoints());
	}
	else if(strcmp(cmd2, "run"))
	{
		if(argc >= 1)
		{
			if(checkMnt())
			{
				struct node *nod = findFile(getMountPoints(), args[0]);
				if(nod)
				{
					struct file* file = fopen(nod);
					fseek(file, -1);
					uint64_t size = ftell(file);
					fseek(file, 0);
					uint8_t *code = malloc(size);
					fread(file, size, code);
					fclose(file);
					elfRun(code, argc, args);
					free(code);
				}
				else
					print("File not found.\n");
			}
		}
		else
			print("run [fileName] [args...]\n");
	}
	else
		print("commands:\nexit, clear, newfile, rm, ls, format, mount, umount, rename, run\n");
	
	if(argc)
		free(args);
	free(cmd2);
	setStyle(FG_GREY);
}
