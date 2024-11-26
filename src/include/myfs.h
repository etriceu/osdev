#ifndef MYFS_H
#define MYFS_H

#include <stdint.h>
#include <stddef.h>

#ifndef KERNEL
#include "userCall.h"
#define _PREFIX static inline
#else
#define _PREFIX
#endif

#include "myfsCommon.h"

/* node table structure
2B lastSectorSize
4B firstBlock
sizeof(size_t) nameSize
nameSize B name
*/

/* block structure
4B blockSize
4B nextBlock
*/

struct node
{
	uint32_t firstBlock;
	uint16_t lastSectorSize;
	size_t nameSize;
	char *name;
	void *mnt;
	struct node *next;
};

struct file
{
	size_t virtPos;
	uint32_t blockAdr;
	uint32_t blockSize;
	uint32_t nextBlock;
	uint32_t sector;
	uint16_t offset;
	uint8_t doFlush;
	uint8_t buf[MY_FS_SECTOR_SIZE];
	struct node *node;
};

struct mount
{
	uint32_t begin, end;
	uint8_t device;
	struct node *nodes;
	struct mount *next;
};

_PREFIX struct mount* getMountPoints()
#ifndef KERNEL
{return (struct mount*)call(8, 0, 0, 0, 0);}
#endif
;
_PREFIX struct mount* mount(uint8_t device, uint32_t begin, uint32_t end)
#ifndef KERNEL
{return (struct mount*)call(9, device, begin, end, 0);}
#endif
;
_PREFIX void umount(struct mount* mnt)
#ifndef KERNEL
{call(10, (size_t)mnt, 0, 0, 0);}
#endif
;

_PREFIX struct node* newFile(struct mount *mnt, const char *name, const size_t size)
#ifndef KERNEL
{return (struct node*)call(11, (size_t)mnt, (size_t)name, size, 0);}
#endif
;
_PREFIX char* getFileName(struct node *node, size_t *size)
#ifndef KERNEL
{return (char*)call(12, (size_t)node, (size_t)size, 0, 0);}
#endif
;
_PREFIX struct node* findFile(struct mount *mnt, const char* name)
#ifndef KERNEL
{return (struct node*)call(13, (size_t)mnt, (size_t)name, 0, 0);}
#endif
;
_PREFIX void removeFile(struct node* node)
#ifndef KERNEL
{call(14, (size_t)node, 0, 0, 0);}
#endif
;
_PREFIX void renameFile(struct node* node, const char *name, size_t size)
#ifndef KERNEL
{call(15, (size_t)node, (size_t)name, size, 0);}
#endif
;

_PREFIX struct file* fopen(struct node* node)
#ifndef KERNEL
{return (struct file*)call(16, (size_t)node, 0, 0, 0);}
#endif
;
_PREFIX void fclose(struct file *file)
#ifndef KERNEL
{call(17, (size_t)file, 0, 0, 0);}
#endif
;
_PREFIX uint8_t fread(struct file *file, size_t size, uint8_t *dest)
#ifndef KERNEL
{return call(18, (size_t)file, size, (size_t)dest, 0);}
#endif
;
_PREFIX uint8_t fwrite(struct file *file, size_t size, uint8_t *src)
#ifndef KERNEL
{return call(19, (size_t)file, size, (size_t)src, 0);}
#endif
;
_PREFIX void fflush(struct file *file)
#ifndef KERNEL
{call(20, (size_t)file, 0, 0, 0);}
#endif
;

_PREFIX uint8_t fseek(struct file* file, size_t pos)
#ifndef KERNEL
{return call(36, (size_t)file, pos, 0, 0);}
#endif
;

_PREFIX size_t ftell(struct file* file)
#ifndef KERNEL
{return call(37, (size_t)file, 0, 0, 0);}
#endif
;

_PREFIX void ftrim(struct file* file)
#ifndef KERNEL
{call(38, (size_t)file, 0, 0, 0);}
#endif
;

#endif // MYFS_H_INCLUDED
