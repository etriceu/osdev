#include "../include/system.h"
#include "../include/functions.h"
#include "../include/malloc.h"
#include "../include/myfs.h"
#include "../include/ata.h"
#include "../include/exec.h"
#include "../include/irq.h"

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
	char **args;
	int argc = 0;
	int size = 0;
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
				newFile(getMountPoints(), args[0]); // get first mnt
			else
				print("newfile [fileName]\n");print("3\n");
		}
	}
	else if(strcmp(cmd2, "rename"))
	{
		if(argc == 2)
		{
			if(checkMnt())
			{
				struct node *nod = findFile(getMountPoints(), args[0]);
				if(nod)
					renameFile(nod, args[1]);
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
			char *name = malloc(512);
			for(struct node *nod = getMountPoints()->nodes; nod != 0; nod = nod->next)
			{
				getFileName(nod, name);
				print(name);
				print("\n");
			}
			free(name);
		}
	}
	else if(strcmp(cmd, "format")) // format first device
	{
		char *zero = malloc(512*256);
		uint32_t n = 0;
		
		print("formating... ");
		uint32_t diskSize = ataGetSize(PRIMARY_MASTER);
		for(; n <= diskSize; n += 255)
			ataWrite(PRIMARY_MASTER, n, 255, zero);
		
		ataWrite(PRIMARY_MASTER, n, diskSize-n*255, zero);
		
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
					uint64_t size;
					struct file* file = fopen(nod, &size, MYFS_READ);
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
