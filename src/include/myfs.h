#ifndef MYFS_H
#define MYFS_H

#include <stdint.h>
#include <stddef.h>

#ifndef KERNEL
#include "userCall.h"
#endif

#define EOF 1

#define MYFS_READ 0
#define MYFS_WRITE 1

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

struct mount* getMountPoints()
#ifndef KERNEL
{return (struct mount*)call(8, 0, 0, 0, 0);}
#endif
;
struct mount* mount(uint8_t device, uint32_t begin, uint32_t end)
#ifndef KERNEL
{return (struct mount*)call(9, device, begin, end, 0);}
#endif
;
void umount(struct mount* mnt)
#ifndef KERNEL
{call(10, (uint32_t)mnt, 0, 0, 0);}
#endif
;

struct node* newFile(struct mount *mnt, const char *name)
#ifndef KERNEL
{return (struct node*)call(11, (uint32_t)mnt, (uint32_t)name, 0, 0);}
#endif
;
void getFileName(struct node *nod, char *dest)
#ifndef KERNEL
{call(12, (uint32_t)nod, (uint32_t)dest, 0, 0);}
#endif
;
struct node* findFile(struct mount *mnt, const char* name)
#ifndef KERNEL
{return (struct node*)call(13, (uint32_t)mnt, (uint32_t)name, 0, 0);}
#endif
;
void removeFile(struct node* nod)
#ifndef KERNEL
{call(14, (uint32_t)nod, 0, 0, 0);}
#endif
;
void renameFile(struct node* nod, const char *name)
#ifndef KERNEL
{call(15, (uint32_t)nod, (uint32_t)name, 0, 0);}
#endif
;

struct file* fopen(struct node* nod, uint64_t *sizeOut, uint8_t mode)
#ifndef KERNEL
{return (struct file*)call(16, (uint32_t)nod, (uint32_t)sizeOut, (uint32_t)mode, 0);}
#endif
;
void fclose(struct file *file)
#ifndef KERNEL
{call(17, (uint32_t)file, 0, 0, 0);}
#endif
;
uint8_t fread(struct file *file, size_t size, uint8_t *dest)
#ifndef KERNEL
{return call(18, (uint32_t)file, size, (uint32_t)dest, 0);}
#endif
;
void fwrite(struct file *file, size_t size, uint8_t *src)
#ifndef KERNEL
{call(19, (uint32_t)file, size, (uint32_t)src, 0);}
#endif
;
void fflush(struct file *file)
#ifndef KERNEL
{call(20, (uint32_t)file, 0, 0, 0);}
#endif
;

#endif // MYFS_H_INCLUDED
