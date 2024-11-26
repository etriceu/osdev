#include "../include/myfs.h"
#include "../include/ata.h"
#include "../include/malloc.h"
#include "../include/functions.h"

struct mount *mountPoints = NULL;

void readNextBlockHeader(struct file *file)
{
	struct mount *mnt = file->node->mnt;
	ataRead(mnt->device, file->nextBlock, 1, file->buf);
	file->blockAdr = file->nextBlock;
	file->offset = MY_FS_BLOCK_HEADER_SIZE;
	file->blockSize = *(uint32_t*)file->buf;
	file->nextBlock = *(uint32_t*)(file->buf+sizeof(file->blockSize));
	file->sector = 0;
}

uint8_t fseek(struct file *file, size_t pos)
{
	fflush(file);
	
	file->virtPos = 0;
	file->nextBlock = file->node->firstBlock;
	readNextBlockHeader(file);

	do
	{
		size_t bSize = file->blockSize*MY_FS_SECTOR_SIZE-MY_FS_BLOCK_HEADER_SIZE;
		if(file->nextBlock == 0)
		{
			if(file->blockSize == 1)
				bSize += MY_FS_BLOCK_HEADER_SIZE;
			
			bSize -= MY_FS_SECTOR_SIZE - file->node->lastSectorSize;
				
			if(file->virtPos+bSize <= pos)
			{
				file->sector = file->blockSize-1;
				file->offset = file->node->lastSectorSize;
				file->virtPos += bSize;
				return MY_EOF;
			}
		}
		
		if(file->virtPos+bSize < pos)
		{
			file->virtPos += bSize;
			readNextBlockHeader(file);
		}
		else
		{
			size_t diff = pos-file->virtPos+MY_FS_BLOCK_HEADER_SIZE;
			file->sector = diff/MY_FS_SECTOR_SIZE;
			if(file->sector == 0)
				file->offset = diff;
			else
				file->offset = diff%MY_FS_SECTOR_SIZE;
				
			file->virtPos = pos;
		}
	} while(file->virtPos < pos);
	
	return 0;
}

size_t ftell(struct file *file)
{
	return file->virtPos;
}

struct mount* getMountPoints()
{
	return mountPoints;
}

void loadSuperNode(struct mount *mnt, struct node *superNode)
{
	superNode->firstBlock = mnt->begin;
	superNode->nameSize = 0;
	superNode->name = NULL;
	superNode->mnt = mnt;
	superNode->next = NULL;

	uint8_t buf[MY_FS_SECTOR_SIZE];
	ataRead(mnt->device, mnt->begin, 1, buf);
	superNode->lastSectorSize = *((uint16_t*)(buf+MY_FS_BLOCK_HEADER_SIZE));
}

struct mount* mount(uint8_t device, uint32_t begin, uint32_t end)
{
	if(end == 0)
		end = ataGetSize(device);
	
	if(!end)
		return NULL;
	
	struct mount *lastMnt = mountPoints;
	if(lastMnt != NULL)
		do {
			if(lastMnt->begin == begin && lastMnt->device == device
				&& lastMnt->end == end)
				return NULL;
			
			if(lastMnt->next != NULL)
				lastMnt = lastMnt->next;
		} while(1);
	
	struct mount *mnt = malloc(sizeof(struct mount));
	mnt->begin = begin;
	mnt->device = device;
	mnt->next = mountPoints;
	mnt->end = end;
	
	mountPoints = mnt;

	struct node firstNode;
	loadSuperNode(mnt, &firstNode);

	struct file *nodeTable = fopen(&firstNode);
	if(fseek(nodeTable, MY_FS_NODE_SIZE) != MY_EOF)
	{
		uint8_t isEOF;
		do
		{
			isEOF = 0;
			struct node *lastNode = mnt->nodes;
			struct node *newNode = malloc(sizeof(struct node));
			mnt->nodes = newNode;
			isEOF += fread(nodeTable, 2, (uint8_t*)&newNode->lastSectorSize);
			isEOF += fread(nodeTable, 4, (uint8_t*)&newNode->firstBlock);
			isEOF += fread(nodeTable, 4, (uint8_t*)&newNode->nameSize);
			
			if(isEOF)
			{
				free(newNode);
				mnt->nodes = lastNode;
				break;
			}
			
			if(newNode->nameSize)
			{
				newNode->name = malloc(newNode->nameSize+1);
				newNode->name[newNode->nameSize] = '\0';
				isEOF = fread(nodeTable, newNode->nameSize, (uint8_t*)newNode->name);
			}
			
			newNode->mnt = mnt;
			newNode->next = lastNode;
		} while(!isEOF);
	}
	fclose(nodeTable);
	return mnt;
}

void ftrim(struct file *file)
{
	struct node *node = file->node;
	struct mount *mnt = node->mnt;
	
	struct file fileTrim = *file;
	
	node->lastSectorSize = file->offset;
	if(file->sector == 0)
		 node->lastSectorSize -= MY_FS_BLOCK_HEADER_SIZE;
	
	if(file->virtPos != 0)
	{
		for(uint8_t *it = file->buf+file->offset, *end = file->buf+MY_FS_SECTOR_SIZE;
			it < end; it++)
			*it = 0;
		file->nextBlock = 0;
		file->doFlush = 1;
		fflush(file);
		if(file->sector+1 < file->blockSize)
			ataFill(mnt->device, file->sector+file->blockAdr+1,
					file->blockSize-file->sector-1, 0);
		if(fileTrim.nextBlock != 0)
			readNextBlockHeader(&fileTrim);
		else
			return;
	}
	
	do
	{
		uint32_t blockAdr = fileTrim.blockAdr;
		uint32_t blockSize = fileTrim.blockSize;
		
		ataFill(mnt->device, blockAdr, blockSize, 0);
		
		if(fileTrim.nextBlock != 0)
			readNextBlockHeader(&fileTrim);
		else
			break;
	} while(1);
	
	file->doFlush = 0;
}

void saveNodes(struct mount *mnt)
{
	struct node superNode;
	loadSuperNode(mnt, &superNode);
	struct file *nodeTable = fopen(&superNode);
	
	fwrite(nodeTable, 2, (uint8_t*)&superNode.lastSectorSize);
	fwrite(nodeTable, 4, (uint8_t*)&superNode.firstBlock);
	fwrite(nodeTable, 4, (uint8_t*)&superNode.nameSize);
	fwrite(nodeTable, superNode.nameSize, (uint8_t*)superNode.name);
	
	for(struct node *node = mnt->nodes; node != NULL; node = node->next)
	{
		fwrite(nodeTable, 2, (uint8_t*)&node->lastSectorSize);
		fwrite(nodeTable, 4, (uint8_t*)&node->firstBlock);
		fwrite(nodeTable, 4, (uint8_t*)&node->nameSize);
		fwrite(nodeTable, node->nameSize, (uint8_t*)node->name);
	}
	
	ftrim(nodeTable);
	
	fseek(nodeTable, 0);
	fwrite(nodeTable, 2, (uint8_t*)&superNode.lastSectorSize);

	fclose(nodeTable);
}

void umount(struct mount *mnt)
{
	saveNodes(mnt);
	
	for(struct node *node = mnt->nodes; node != NULL;)
	{
		void *ptr = node;
		if(node->name != NULL)
			free(node->name);
		node = node->next;
		free(ptr);
	}
	
	if(mountPoints == mnt)
		mountPoints = mnt->next;
	else
	{
		struct mount *m = mountPoints;
		for(; m->next != mnt && m->next != NULL; m = m->next);
		if(m != NULL)
			m->next = mnt->next;
	}
	free(mnt);
}

void saveBlockInfo(struct file *file)
{
	struct mount *mnt = file->node->mnt;
	uint8_t *buf = malloc(MY_FS_SECTOR_SIZE);
	ataRead(mnt->device, file->blockAdr, 1, buf);
	*(uint32_t*)buf = file->blockSize;
	*(uint32_t*)(buf+sizeof(file->blockSize)) = file->nextBlock;
	ataWrite(mnt->device, file->blockAdr, 1, buf);
	free(buf);
}

uint8_t createOrExtendBlock(struct file *file, uint32_t from)
{
	struct node *node = file->node;
	struct mount *mnt = node->mnt;
	uint8_t *buf = malloc(MY_FS_SECTOR_SIZE);
	uint8_t newBlock = 0;
	uint8_t end = 0;
	uint32_t i = from;
	
	do
	{
		ataRead(mnt->device, i, 1, buf);
		
		uint32_t blockSize = *(uint32_t*)buf;
		if(blockSize > 0) {
			i += blockSize;
			newBlock = 1;
		}
		else
		{
			if(file->blockAdr == 0) //first new block
			{
				node->firstBlock = i;
				file->blockAdr = i;
				file->blockSize = 1;
				file->nextBlock = 0;
				file->offset = MY_FS_BLOCK_HEADER_SIZE;
			}
			else if(!newBlock) //extend
				file->blockSize++;
			else // extend with new block
			{
				file->nextBlock = i;
				saveBlockInfo(file);
				file->blockAdr = i;
				file->blockSize = 1;
				file->sector = 0;
				file->nextBlock = 0;
				file->offset = MY_FS_BLOCK_HEADER_SIZE;
			}
			
			saveBlockInfo(file);
			free(buf);
			return 0;
		}
		
		if(i >= mnt->end)
		{
			i = mnt->begin;
			end = 1;
		}
	} while(end ^ (i >= from));
	free(buf);
	return MY_EOF;
}

struct node* newFile(struct mount *mnt, const char *name, const size_t size)
{
	struct node *newNode = malloc(sizeof(struct node));
	newNode->next = mnt->nodes;
	newNode->mnt = mnt;
	newNode->lastSectorSize = 0;
	struct file file;
	file.blockAdr = 0;
	file.node = newNode;
	file.offset = MY_FS_BLOCK_HEADER_SIZE;
	file.blockSize = 1;
	
	if(createOrExtendBlock(&file, mnt->begin))
	{
		free(newNode);
		return NULL;
	}
	renameFile(newNode, name, size);
	
	mnt->nodes = newNode;
	return newNode;
}

struct node* findFile(struct mount *mnt, const char* name)
{
	struct node *n = mnt->nodes;
	for(; n != NULL && !strcmp(n->name, name); n = n->next);
	return n;
}

void removeFile(struct node* node)
{
	struct mount *mnt = node->mnt;
	struct node *prev = NULL;
	struct node *n = mnt->nodes;
	for(; n != NULL && n != node; prev = n, n = n->next);

	if(n == node)
	{
		if(prev == NULL)
			mnt->nodes = n->next;
		else
			prev->next = n->next;
		
		struct file *file = fopen(n);
		ftrim(file);
		fclose(file);
		
		free(n->name);
		free(n);
	}
}

void renameFile(struct node* node, const char *name, size_t size)
{
	if(node->nameSize > 0)
		free(node->name);
	node->nameSize = size;
	node->name = malloc(size+1);
	
	for(size_t n = 0; n < size; n++)
		node->name[n] = name[n];
	node->name[size] = '\0';
}

struct file* fopen(struct node* node)
{
	struct file *file = malloc(sizeof(struct file));
	file->node = node;
	file->nextBlock = node->firstBlock;
	file->virtPos = 0;
	file->sector = 0;
	file->offset = MY_FS_BLOCK_HEADER_SIZE;
	file->doFlush = 0;
	
	readNextBlockHeader(file);
	
	return file;
}

void fclose(struct file *file)
{
	fflush(file);
	free(file);
}

uint8_t fread(struct file *file, size_t size, uint8_t *dest)
{
	struct node *node = file->node;
	struct mount *mnt = node->mnt;
	for(uint8_t *end = dest+size; dest < end; dest++, file->offset++, file->virtPos++)
	{
		if(file->nextBlock == 0 && file->sector >= file->blockSize-1
			&& file->offset-MY_FS_BLOCK_HEADER_SIZE >= node->lastSectorSize)
			return MY_EOF;
		
		if(file->offset >= MY_FS_SECTOR_SIZE) {
			if(file->sector >= file->blockSize-1)
				readNextBlockHeader(file);
			else
			{
				file->offset = 0;
				file->sector++;
				ataRead(mnt->device, file->sector+file->blockAdr, 1, file->buf);
			}
		}
		
		*dest = file->buf[file->offset];
	}
	return 0;
}

void write(struct file *file)
{
	struct node *node = file->node;
	struct mount *mnt = node->mnt;
	ataWrite(mnt->device, file->sector+file->blockAdr, 1, file->buf);
	if(file->nextBlock == 0 && file->sector+1 == file->blockSize &&
		node->lastSectorSize > file->offset)
		node->lastSectorSize = file->offset;
	
	file->doFlush = 0;
}

uint8_t fwrite(struct file *file, size_t size, uint8_t *src)
{
	for(uint8_t *end = src+size; src < end; src++)
	{
		file->buf[file->offset] = *src;
		file->offset++;
		file->virtPos++;
		file->doFlush = 1;
		
		if(file->offset >= MY_FS_SECTOR_SIZE)
		{
			write(file);
			
			if(file->sector+1 >= file->blockSize)
			{
				if(file->nextBlock == 0)
				{
					if(createOrExtendBlock(file, file->blockAdr+file->sector+1) == MY_EOF)
						return MY_EOF;
					
					if(file->blockSize > 1)
					{
						file->sector++;
						file->offset = 0;
					}
				}
			}
			else
			{
				file->sector++;
				file->offset = 0;
			}
		}
	}
	return 0;
}

void fflush(struct file *file)
{
	if(!file->doFlush)
		return;
	
	write(file);
}

char* getFileName(struct node *node, size_t *size)
{
	if(size != NULL)
		*size = node->nameSize;
	return node->name;
}
