#include "../include/screen.h"
#include "../include/malloc.h"
#include "../include/exec.h"

uint8_t isElf(uint8_t *buf)
{
	struct ElfHeader *head = (struct ElfHeader*)buf;
	
	for(int n = 0; n < 4; n++)
		if(head->ident[n] != ELF_MAGIC[n])
		{
			print("Invalid ELF format.\n");
			return 0;
		}
		
	return 1;
}

uint8_t elfCompatibility(uint8_t *buf)
{
	struct ElfHeader *head = (struct ElfHeader*)buf;
	if(head->ident[EI_CALSS] != ELFCLASS32 ||
		head->ident[EI_DATA] != ELFDATA2LSB ||
		head->ident[EI_VERSION] != EV_CURRENT ||
		(head->type != ET_EXEC && head->type != ET_DYN)
		|| head->machine != EM_386)
	{
		print("Unsupported ELF format.\n");
		return 0;
	}
	return 1;
}

int elfRun(uint8_t *buf, int argc, char** argv)
{
	if(!isElf(buf) | !elfCompatibility(buf))
		return -1;
	
	size_t size = 0;
	struct ElfHeader *head = (struct ElfHeader*)buf;
	struct ElfPhdr *phdr = (struct ElfPhdr*)(buf+head->phoff);
	
	for(int n = 0; n < head->phnum; n++)
		if(size < phdr[n].filesz + phdr[n].vaddr)
			size = phdr[n].filesz + phdr[n].vaddr;
	
	uint8_t *proc = malloc(size);
	
	for(int n = 0; n < head->phnum; n++)
		if(phdr[n].type == PT_LOAD || phdr[n].type == PT_DYNAMIC)
			for(size_t x = 0; x < phdr[n].filesz; x++)
				proc[x+phdr[n].vaddr] = buf[x+phdr[n].offset];
	
	int (*run)(int, char**) = (int (*)(int, char**))(proc+head->entry);
	int ret = (*run)(argc, argv);
	free(proc);
	return ret;
}
