#include "../include/myfs.h"
#include "../include/ata.h"
#include "../include/malloc.h"

/*
first sector	|	N sectors	|	last sectors
empty secotr	|	saved files	|	file table

saved files:
	first file sector:
		size of the data segment (3 bytes)
		address of the next data segment (3 bytes)
		file name (506 bytes) (only first data segment)
	next N sectors:
		data

file table - array of 3B to the beginnings of files 
*/

struct node *nodes = 0;

uint8_t mount()
{
	struct node **nod = &nodes;
	uint8_t *buf = malloc(512);
	uint32_t end;
	for(end = ataGetSize(); end > 0; end--)
	{
		ataRead(end, 1, buf);
		int sum = 0;
		for(int n = 0; n < 512; n++)
			sum += buf[n];
		
		if(!sum)
			break;
		
		for(int n = 0; n < 510; n += 3)
			if((buf[n] | buf[n+1] | buf[n+2]) != 0)
			{
				*nod = (struct node*)malloc(sizeof(struct node));
				((struct node*)(*nod))->next = 0;
				((struct node*)(*nod))->address[0] = buf[n];
				((struct node*)(*nod))->address[1] = buf[n+1];
				((struct node*)(*nod))->address[2] = buf[n+2];
				nod = &((struct node*)(*nod))->next;
			}
	}
	
	free(buf);
	return end ? 1 : 0;
}

void umount()
{
	for(struct node *nod = nodes; nod != 0;)
	{
		void *ptr = nod;
		nod = nod->next;
		free(ptr);
	}
}

void newFile(const char *name)
{
	uint32_t pos = 1;
	uint8_t *buf = malloc(512);
	
	while(1)
	{
		ataRead(pos, 1, buf);
		uint32_t size = *(uint32_t*)buf & 0x00ffffff;
		if(size == 0)
		{
			int n = 0;
			for(; n < 6; n++)
				buf[n] = 0;
			
			buf[0] = 1;
			
			for(int s = 0; name[s] != 0; s++, n++)
				buf[n] = name[s];
			
			for(; n < 512; n++)
				buf[n] = 0;
			
			ataWrite(pos, 1, buf);
			
			break;
		}
		else
			pos += size;
	}
	
	struct node **nod = &nodes;
	for(; *nod != 0; nod = &((struct node*)(*nod))->next);
	*nod = (struct node*)malloc(sizeof(struct node));
	((struct node*)(*nod))->next = 0;
	((struct node*)(*nod))->address[0] = (uint8_t)pos;
	((struct node*)(*nod))->address[1] = (uint8_t)(pos>>8);
	((struct node*)(*nod))->address[2] = (uint8_t)(pos>>16);
	
	for(uint32_t end = ataGetSize(); end > 0; end--)
	{
		ataRead(end, 1, buf);
		for(int n = 0; n < 512; n += 3)
			if((buf[n] | buf[n+1] | buf[n+2]) == 0)
			{
				buf[n] = (uint8_t)pos;
				buf[n+1] = (uint8_t)(pos>>8);
				buf[n+2] = (uint8_t)(pos>>16);
				
				ataWrite(end, 1, buf);
				free(buf);
				return;
			}
	}
	free(buf);
}

void getFileName(struct node *nod, char *dest)
{
	uint8_t *buf = malloc(512);
	ataRead(*(uint32_t*)nod->address & 0x00ffffff, 1, buf);
	for(int n = 6; buf[n] != 0 && n < 512; n++, dest++)
		*dest = buf[n];
	
	*dest = 0;
	
	free(buf);
}

struct node* findFile(const char* name)
{
	uint8_t *buf = malloc(512);
	for(struct node *nod = nodes; nod != 0; nod = nod->next)
	{
		ataRead(*(uint32_t*)nod->address & 0x00ffffff, 1, buf);
		if(strcmp(name, &buf[6]))
		{
			free(buf);
			return nod;
		}
	}
	free(buf);
	return 0;
}

void removeFile(struct node* nod)
{
	uint8_t *buf = malloc(512);
	uint8_t *zero = malloc(512);
	int sum = 0;
	uint32_t end;
	uint8_t found = 0;
	for(end = ataGetSize(); end > 0 && !found; end--)
	{
		ataRead(end, 1, buf);
		for(int n = 0; n < 512; n += 3)
		{
			if( buf[n] == nod->address[0] &&
				buf[n+1] == nod->address[1] &&
				buf[n+2] == nod->address[2])
			{
				buf[n] = 0;
				buf[n+1] = 0;
				buf[n+2] = 0;
				found = 1;
				ataWrite(end, 1, buf);
			}
			sum += buf[n];
		}
		if(!sum)
			break;
	}
	if(!sum && found)
		while(1)
		{
			ataRead(end-1, 1, buf);
			sum = 0;
			for(int n = 0; n < 512; n += 3)
				sum += buf[n];
			
			if(sum)
			{
				ataWrite(end, 1, buf);
				ataWrite(end-1, 1, zero);
				end--;
			}
			else
				break;
		}
	
	uint32_t size, next, lba = *(uint32_t*)nod->address & 0x00ffffff;
	while(1)
	{
		ataRead(lba, 1, buf);
		size = *(uint32_t*)buf & 0x00ffffff;
		next = *(uint32_t*)(buf+3) & 0x00ffffff;
		
		for(uint32_t n = 0; n < size; n++)
			ataWrite(lba+n, 1, zero);
		
		if(next)
			lba = next;
		else
			break;
	}

	if(nod == nodes)
	{
		nodes = nod->next;
		free(nod);
	}
	else
		for(struct node *n = nodes; n != 0; n = n->next)
			if(n->next == nod)
			{
				n->next = nod->next;
				free(nod);
				break;
			}

	free(buf);
	free(zero);
}

void renameFile(struct node* nod, const char *name)
{
	uint8_t *buf = malloc(512);
	uint32_t lba = *(uint32_t*)nod->address & 0x00ffffff;
	ataRead(lba, 1, buf);
	
	int n = 0;
	for(; name[n] != 0; n++)
		buf[n+6] = name[n];
	buf[n+6] = 0;
	
	ataWrite(lba, 1, buf);
	free(buf);
}
