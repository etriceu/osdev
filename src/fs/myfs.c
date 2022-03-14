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
		last sector size (2 bytes) (only first data segment)
		file name (504 bytes) (only first data segment)
	next N sectors:
		data

file table - array of 3B to the beginnings of files 
*/

#define SIZE_OFFSET 0
#define NEXT_OFFSET 3
#define LSIZE_OFFSET 6
#define NAME_OFFSET 8
#define LBA_SIZE 3
#define TABLE_SIZE  512/LBA_SIZE*LBA_SIZE

uint64_t appendMode(struct file *file);

struct mount *mountPoints = 0;

struct mount* getMountPoints()
{
	return mountPoints;
}

struct mount* mount(uint8_t device, uint32_t begin, uint32_t end)
{
	struct mount *mnt = malloc(sizeof(struct mount));
	mnt->begin = begin;
	mnt->device = device;
	mnt->next = 0;
	if(end == 0)
		end = ataGetSize(device);
	mnt->end = end;
	
	if(!end)
	{
		free(mnt);
		return NULL;
	}

	struct node **nod = &mnt->nodes;
	uint8_t *buf = malloc(512);
	for(; end >= begin; end--)
	{
		ataRead(device, end, 1, buf);
		uint8_t sum = 0;
		for(int n = 0; n < 512 && !sum; n++)
			sum |= buf[n];
		
		if(!sum)
			break;
		
		for(int n = 0; n < TABLE_SIZE; n += LBA_SIZE)
			if((buf[n] | buf[n+1] | buf[n+2]) != 0)
			{
				*nod = (struct node*)malloc(sizeof(struct node));
				((struct node*)(*nod))->next = 0;
				((struct node*)(*nod))->address[0] = buf[n];
				((struct node*)(*nod))->address[1] = buf[n+1];
				((struct node*)(*nod))->address[2] = buf[n+2];
				((struct node*)(*nod))->mntPtr = mnt;
				nod = &((struct node*)(*nod))->next;
			}
	}

	free(buf);

	if(mountPoints == 0)
		mountPoints = mnt;
	else
		for(struct mount *m = mountPoints; m->next != 0 || !(m->next = mnt); m = m->next);
	
	return mnt;
}

void umount(struct mount *mnt)
{
	for(struct node *nod = mnt->nodes; nod != 0;)
	{
		void *ptr = nod;
		nod = nod->next;
		free(ptr);
	}
	
	if(mountPoints == mnt)
		mountPoints = mnt->next;
	else
	{
		struct mount *m = mountPoints;
		for(; m->next != mnt && m->next != 0; m = m->next);
		if(m != 0)
			m->next = mnt->next;
	}
	free(mnt);
}

struct node* newFile(struct mount *mnt, const char *name)
{
	uint32_t pos = mnt->begin;
	uint8_t *buf = malloc(512);
	
	while(1)
	{
		ataRead(mnt->device, pos, 1, buf);
		uint32_t size = *(uint32_t*)(buf+SIZE_OFFSET) & 0x00ffffff;
		if(size == 0)
		{
			int n = 0;
			for(; n < NAME_OFFSET; n++)
				buf[n] = 0;
			
			buf[SIZE_OFFSET] = 1;
			
			for(int s = 0; name[s] != 0; s++, n++)
				buf[n] = name[s];
			
			for(; n < 512; n++)
				buf[n] = 0;
			
			ataWrite(mnt->device, pos, 1, buf);
			
			break;
		}
		else
			pos += size;
	}
	
	struct node **nod = &mnt->nodes;
	for(; *nod != 0; nod = &((struct node*)(*nod))->next);
	*nod = (struct node*)malloc(sizeof(struct node));
	((struct node*)(*nod))->next = 0;
	((struct node*)(*nod))->address[0] = (uint8_t)pos;
	((struct node*)(*nod))->address[1] = (uint8_t)(pos>>8);
	((struct node*)(*nod))->address[2] = (uint8_t)(pos>>16);
	((struct node*)(*nod))->mntPtr = mnt;
	
	for(uint32_t end = mnt->end; end >= mnt->begin; end--)
	{
		ataRead(mnt->device, end, 1, buf);
		for(int n = 0; n < TABLE_SIZE; n += LBA_SIZE)
			if((buf[n] | buf[n+1] | buf[n+2]) == 0)
			{
				buf[n] = (uint8_t)pos;
				buf[n+1] = (uint8_t)(pos>>8);
				buf[n+2] = (uint8_t)(pos>>16);
				
				ataWrite(mnt->device, end, 1, buf);
				free(buf);
				return *nod;
			}
	}
	free(buf);
	return 0;
}

void getFileName(struct node *nod, char *dest)
{
	uint8_t *buf = malloc(512);
	struct mount *mnt = nod->mntPtr;
	ataRead(mnt->device, *(uint32_t*)nod->address & 0x00ffffff, 1, buf);
	for(int n = NAME_OFFSET; buf[n] != 0 && n < 512; n++, dest++)
		*dest = buf[n];
	
	*dest = 0;
	
	free(buf);
}

struct node* findFile(struct mount *mnt, const char* name)
{
	uint8_t *buf = malloc(512);
	for(struct node *nod = mnt->nodes; nod != 0; nod = nod->next)
	{
		ataRead(mnt->device, *(uint32_t*)nod->address & 0x00ffffff, 1, buf);
		if(strcmp(name, &buf[NAME_OFFSET]))
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
	struct mount *mnt = nod->mntPtr;
	uint8_t *buf = malloc(512);
	uint8_t *zero = malloc(512);
	uint8_t sum = 0;
	uint32_t end;
	uint8_t found = 0;
	for(end = mnt->end; end >= mnt->begin && !found; end--)
	{
		sum = 0;
		ataRead(mnt->device, end, 1, buf);
		for(int n = 0; n < TABLE_SIZE; n += LBA_SIZE)
		{
			if( buf[n] == nod->address[0] &&
				buf[n+1] == nod->address[1] &&
				buf[n+2] == nod->address[2])
			{
				buf[n] = 0;
				buf[n+1] = 0;
				buf[n+2] = 0;
				found = 1;
				ataWrite(mnt->device, end, 1, buf);
			}
			sum |= buf[n];
			sum |= buf[n+1];
			sum |= buf[n+2];
		}
		if(!sum)
			break;
	}
	if(!sum && found)
		while(1)
		{
			ataRead(mnt->device, end-1, 1, buf);
			sum = 0;
			for(int n = 0; n < 512 && !sum; n++)
				sum |= buf[n];
			
			if(sum)
			{
				ataWrite(mnt->device, end, 1, buf);
				ataWrite(mnt->device, end-1, 1, zero);
				end--;
			}
			else
				break;
		}
	
	uint32_t size, next, lba = *(uint32_t*)nod->address & 0x00ffffff;
	while(1)
	{
		ataRead(mnt->device, lba, 1, buf);
		size = *(uint32_t*)(buf+SIZE_OFFSET) & 0x00ffffff;
		next = *(uint32_t*)(buf+NEXT_OFFSET) & 0x00ffffff;
		
		for(uint32_t n = 0; n < size; n++)
			ataWrite(mnt->device, lba+n, 1, zero);
		
		if(next)
			lba = next;
		else
			break;
	}

	if(nod == mnt->nodes)
	{
		mnt->nodes = nod->next;
		free(nod);
	}
	else
		for(struct node *n = mnt->nodes; n != 0; n = n->next)
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
	struct mount *mnt = nod->mntPtr;
	uint8_t *buf = malloc(512);
	uint32_t lba = *(uint32_t*)nod->address & 0x00ffffff;
	ataRead(mnt->device, lba, 1, buf);
	
	int n = NAME_OFFSET;
	for(; name[n] != 0; n++)
		buf[n] = name[n];
	buf[n] = 0;
	
	ataWrite(mnt->device, lba, 1, buf);
	free(buf);
}

struct file* fopen(struct node* nod, uint64_t *sizeOut, uint8_t mode)
{
	struct mount *mnt = nod->mntPtr;
	struct file *file = malloc(sizeof(struct file));
	
	file->lba = *(uint32_t*)nod->address & 0x00ffffff;
	file->read.offset = file->lba+1;
	file->read.pos = 0;
	file->read.bufpos = 0;
	file->node = nod;
	
	ataRead(mnt->device, file->lba, 1, file->read.buf);
	file->read.size = *(uint32_t*)(file->read.buf+SIZE_OFFSET) & 0x00ffffff;
	file->read.next = *(uint32_t*)(file->read.buf+NEXT_OFFSET) & 0x00ffffff;
	file->lastSize = *(uint16_t*)(file->read.buf+LSIZE_OFFSET);
	
	file->write = file->read;
	
	
	uint8_t *buf = malloc(512);
	uint32_t size = 0, pos = file->lba;
	do
	{
		ataRead(mnt->device, pos, 1, buf);
		file->write.offset = pos+1;
		uint32_t s = *(uint32_t*)(buf+SIZE_OFFSET) & 0x00ffffff;
		if(s != 0)
			size += s-1;
		pos = *(uint32_t*)(buf+NEXT_OFFSET) & 0x00ffffff;
	} while(pos);
	
	if(size != 0)
		size--;
	
	if(sizeOut)
		*sizeOut = 512*(size)+file->lastSize;
	
	if(mode)
	{
		file->write.pos = (*(uint32_t*)(buf+SIZE_OFFSET) & 0x00ffffff) - 1;
		file->write.bufpos = file->lastSize;
		free(buf);
		
		if(file->write.bufpos)
		{
			file->write.pos--;
			ataRead(mnt->device, file->write.offset+file->write.pos, 1, file->write.buf);
			buf = malloc(512);
			ataWrite(mnt->device, file->write.offset+file->write.pos, 1, buf);
			free(buf);
		}
	}
	else
		free(buf);
	
	return file;
}

void fclose(struct file *file)
{
	fflush(file);
	free(file);
}

uint8_t fread(struct file *file, size_t size, uint8_t *dest)
{
	struct node *nod = file->node;
	struct mount *mnt = nod->mntPtr;
	for(uint8_t *end = dest+size; dest < end;)
	{
		if(file->read.bufpos == 0)
		{
			ataRead(mnt->device, file->read.offset+file->read.pos, 1, file->read.buf);
			file->read.pos++;
			if(file->read.pos > file->read.size)
			{
				file->read.pos = 0;
				file->read.offset = file->read.next+1;
				ataRead(mnt->device, file->read.offset, 1, file->read.buf);
				file->read.size = *(uint32_t*)(file->read.buf+SIZE_OFFSET) & 0x00ffffff;
				file->read.next = *(uint32_t*)(file->read.buf+NEXT_OFFSET) & 0x00ffffff;
				continue;
			}
		}
		
		*dest = file->read.buf[file->read.bufpos];
		dest++;
		file->read.bufpos++;
		
		if(file->read.next == 0 && file->read.size-1 <= file->read.pos &&
			file->read.bufpos > file->lastSize)
			return EOF;
		
		file->read.bufpos %= 512;
	}
	return 0;
}

void write(struct file *file)
{
	struct node *nod = file->node;
	struct mount *mnt = nod->mntPtr;
	uint8_t *buf = malloc(1024);
	uint32_t size;
	ataRead(mnt->device, file->write.offset+file->write.pos, 1, buf);
	if(*(uint32_t*)(buf+SIZE_OFFSET))
	{
		uint32_t next = file->write.offset+file->write.pos;
		do
		{
			next += *(uint32_t*)(buf+SIZE_OFFSET) & 0x00ffffff;
			ataRead(mnt->device, next, 2, buf);
		} while(*(uint32_t*)(buf+SIZE_OFFSET) || *(uint32_t*)(buf+SIZE_OFFSET+512));

		ataRead(mnt->device, file->write.offset-1, 1, buf);
		size = file->write.pos+2;

		buf[SIZE_OFFSET] = (uint8_t)size;
		buf[SIZE_OFFSET+1] = (uint8_t)(size>>8);
		buf[SIZE_OFFSET+2] = (uint8_t)(size>>16);
		buf[NEXT_OFFSET] = (uint8_t)next;
		buf[NEXT_OFFSET+1] = (uint8_t)(next>>8);
		buf[NEXT_OFFSET+2] = (uint8_t)(next>>16);

		ataWrite(mnt->device, file->write.offset-1, 1, buf);
		file->write.pos = 0;
		file->write.offset = next+1;
		buf[SIZE_OFFSET+512] = 1;
		ataWrite(mnt->device, next, 1, buf+512);
	}

	ataWrite(mnt->device, file->write.offset+file->write.pos, 1, file->write.buf);
	file->write.pos++;
	
	if(file->write.bufpos % 512)
	{
		ataRead(mnt->device, file->lba, 1, buf);
		buf[LSIZE_OFFSET] = (uint8_t)file->write.bufpos;
		buf[LSIZE_OFFSET+1] = (uint8_t)(file->write.bufpos>>8);
		ataWrite(mnt->device, file->lba, 1, buf);
		file->lastSize = file->write.bufpos;
	}
	
	ataRead(mnt->device, file->write.offset-1, 1, buf);
	size = file->write.pos+1;
	buf[SIZE_OFFSET] = (uint8_t)size;
	buf[SIZE_OFFSET+1] = (uint8_t)(size>>8);
	buf[SIZE_OFFSET+2] = (uint8_t)(size>>16);
	ataWrite(mnt->device, file->write.offset-1, 1, buf);
	
	file->write.bufpos = 0;
	free(buf);
}

void fwrite(struct file *file, size_t size, uint8_t *src)
{
	for(uint8_t *end = src+size; src < end; src++)
	{
		file->write.buf[file->write.bufpos] = *src;
		file->write.bufpos++;
		if(file->write.bufpos == 512)
			write(file);
	}
}

void fflush(struct file *file)
{
	if(file->write.bufpos)
		write(file);
}
