#include "../include/system.h"
#include "../include/functions.h"
#include "../include/malloc.h"
#include "../include/myfs.h"

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
		asm volatile("hlt");
	}
	else if(strcmp(cmd, "clear"))
		clear();
	else if(strcmp(cmd2, "newfile"))
	{
		if(argc == 1)
			newFile(args[0]);
		else
			print("newfile [fileName]\n");
	}
	else if(strcmp(cmd2, "rename"))
	{
		if(argc == 2)
		{
			struct node *nod = findFile(args[0]);
			if(nod)
				renameFile(nod, args[1]);
			else
				print("File not found.\n");
		}
		else
			print("rename [fileName] [newName]\n");
	}
	else if(strcmp(cmd2, "rm"))
	{
		if(argc == 1)
		{
			struct node *nod = findFile(args[0]);
			if(nod)
				removeFile(nod);
			else
				print("File not found.\n");
		}
		else
			print("rm [fileName]\n");
	}
	else if(strcmp(cmd, "ls"))
	{
		char *name = malloc(512);
		for(struct node *nod = nodes; nod != 0; nod = nod->next)
		{
			getFileName(nod, name);
			print(name);
			print("\n");
		}
		free(name);
	}
	else if(strcmp(cmd, "format"))
	{
		char *zero = malloc(512*256);
		uint32_t n = 0;
		
		print("formating... ");
		
		for(; n <= ataGetSize(); n += 255)
			ataWrite(n, 255, zero);
		
		ataWrite(n, ataGetSize()-n*255, zero);
		
		print("done\n");
		free(zero);
	}
	else if(strcmp(cmd, "mount"))
		mount();
	else if(strcmp(cmd, "umount"))
		umount();
	else
		print("commands:\nexit, clear, newfile, rm, ls, format, mount, umount, rename\n");
	
	if(argc)
		free(args);
	free(cmd2);
	setStyle(FG_GREY);
}
