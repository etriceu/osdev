#ifndef MYFS_H
#define MYFS_H

#include <stdint.h>
#include <stddef.h>

#define EOF 1

struct node
{
	uint8_t address[3];
	void *mntPtr;
	struct node *next;
};

struct filerw
{
	uint32_t size, offset, pos, next;
	uint16_t bufpos;
	uint8_t buf[512];
};

struct file
{
	uint32_t lba;
	uint16_t lastSize;
	struct filerw read;
	struct filerw write;
	struct node *node;
};

struct mount
{
	uint32_t begin, end;
	uint8_t device;
	struct node *nodes;
	struct mount *next;
};

struct mount* getMountPoints();
struct mount* mount(uint8_t device, uint32_t begin, uint32_t end);
void umount(struct mount* mnt);

struct node* newFile(struct mount *mnt, const char *name);
void getFileName(struct node *nod, char *dest);
struct node* findFile(struct mount *mnt, const char* name);
void removeFile(struct node* nod);
void renameFile(struct node* nod, const char *name);

struct file* fopen(struct node* nod, uint64_t *sizeOut);
void fclose(struct file *file);
uint8_t fread(struct file *file, size_t size, uint8_t *dest);
void fwrite(struct file *file, size_t size, uint8_t *src);
void fflush(struct file *file);

#endif // MYFS_H_INCLUDED
