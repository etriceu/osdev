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

/**
 * Retrieves the list of mounted file systems.
 * 
 * @return A pointer to the first mount point in the list.
 */
_PREFIX struct mount* getMountPoints()
#ifndef KERNEL
{return (struct mount*)call(8, 0, 0, 0, 0);}
#endif
;

/**
 * Mounts a partition on a specified disk from a given sector range.
 * 
 * @param device The identifier for the disk (device) to mount.
 * @param begin The starting sector of the partition to mount.
 * @param end The ending sector of the partition to mount.
 * 
 * @return A pointer to the mounted file system structure.
 */
_PREFIX struct mount* mount(uint8_t device, uint32_t begin, uint32_t end)
#ifndef KERNEL
{return (struct mount*)call(9, device, begin, end, 0);}
#endif
;

/**
 * Unmounts a previously mounted file system.
 * 
 * @param mnt A pointer to the mount structure representing the file system to unmount.
 */
_PREFIX void umount(struct mount* mnt)
#ifndef KERNEL
{call(10, (size_t)mnt, 0, 0, 0);}
#endif
;

/**
 * Creates a new file in a mounted file system.
 * 
 * @param mnt A pointer to the mount structure where the file will be created.
 * @param name The name of the file to be created.
 * @param size The size of the file to be created.
 * 
 * @return A pointer to the newly created file node.
 */
_PREFIX struct node* newFile(struct mount *mnt, const char *name, const size_t size)
#ifndef KERNEL
{return (struct node*)call(11, (size_t)mnt, (size_t)name, size, 0);}
#endif
;

/**
 * Retrieves the name of a file node.
 * 
 * @param node A pointer to the file node.
 * @param size A pointer to a size_t variable where the file name size will be stored.
 * 
 * @return A pointer to the file name string.
 */
_PREFIX char* getFileName(struct node *node, size_t *size)
#ifndef KERNEL
{return (char*)call(12, (size_t)node, (size_t)size, 0, 0);}
#endif
;

/**
 * Finds a file by its name in the mounted file system.
 * 
 * @param mnt A pointer to the mount structure where the search will take place.
 * @param name The name of the file to search for.
 * 
 * @return A pointer to the file node if found, or NULL if the file doesn't exist.
 */
_PREFIX struct node* findFile(struct mount *mnt, const char* name)
#ifndef KERNEL
{return (struct node*)call(13, (size_t)mnt, (size_t)name, 0, 0);}
#endif
;

/**
 * Removes a file from the file system.
 * 
 * @param node A pointer to the file node to be removed.
 */
_PREFIX void removeFile(struct node* node)
#ifndef KERNEL
{call(14, (size_t)node, 0, 0, 0);}
#endif
;

/**
 * Renames an existing file.
 * 
 * @param node A pointer to the file node to be renamed.
 * @param name The new name for the file.
 * @param size The size of the new name.
 */
_PREFIX void renameFile(struct node* node, const char *name, size_t size)
#ifndef KERNEL
{call(15, (size_t)node, (size_t)name, size, 0);}
#endif
;

/**
 * Opens a file for reading and writing.
 * 
 * @param node A pointer to the file node to be opened.
 * 
 * @return A pointer to the opened file structure.
 */
_PREFIX struct file* fopen(struct node* node)
#ifndef KERNEL
{return (struct file*)call(16, (size_t)node, 0, 0, 0);}
#endif
;

/**
 * Closes an opened file.
 * 
 * @param file A pointer to the file structure to be closed.
 */
_PREFIX void fclose(struct file *file)
#ifndef KERNEL
{call(17, (size_t)file, 0, 0, 0);}
#endif
;

/**
 * Reads data from an opened file.
 * 
 * @param file A pointer to the file structure from which to read.
 * @param size The number of bytes to read from the file.
 * @param dest A pointer to the destination buffer where the read data will be stored.
 * 
 * @return `0` if the read operation is successful, or `EOF` if the end of the file is reached.
 */
_PREFIX uint8_t fread(struct file *file, size_t size, uint8_t *dest)
#ifndef KERNEL
{return call(18, (size_t)file, size, (size_t)dest, 0);}
#endif
;

/**
 * Writes data to an opened file.
 * 
 * @param file A pointer to the file structure to which data will be written.
 * @param size The number of bytes to write to the file.
 * @param src A pointer to the source buffer containing the data to be written.
 * 
 * @return `0` if the write operation is successful, or `EOF` if there is not enough free space to complete the write.
 */
_PREFIX uint8_t fwrite(struct file *file, size_t size, uint8_t *src)
#ifndef KERNEL
{return call(19, (size_t)file, size, (size_t)src, 0);}
#endif
;

/**
 * Flushes the file buffer to ensure all data is written to the disk.
 * 
 * @param file A pointer to the file structure to be flushed.
 */
_PREFIX void fflush(struct file *file)
#ifndef KERNEL
{call(20, (size_t)file, 0, 0, 0);}
#endif
;


/**
 * Sets the file pointer to a specified position in the file.
 * 
 * @param file A pointer to the file structure where the file pointer will be moved.
 * @param pos The new position of the file pointer.
 * 
 * @return 0 on success, or `EOF` if the end of the file is reached.
 */
_PREFIX uint8_t fseek(struct file* file, size_t pos)
#ifndef KERNEL
{return call(36, (size_t)file, pos, 0, 0);}
#endif
;

/**
 * Retrieves the current position of the file pointer.
 * 
 * @param file A pointer to the file structure whose position will be retrieved.
 * 
 * @return The current position of the file pointer in the file.
 */
_PREFIX size_t ftell(struct file* file)
#ifndef KERNEL
{return call(37, (size_t)file, 0, 0, 0);}
#endif
;

/**
 * Trims the file to its current position, removing any excess data.
 * 
 * @param file A pointer to the file structure to be trimmed.
 */
_PREFIX void ftrim(struct file* file)
#ifndef KERNEL
{call(38, (size_t)file, 0, 0, 0);}
#endif
;

/**
 * Stores the current state of the file system's nodes.
 * 
 * @param mnt A pointer to the mount structure representing the file system to save.
 */
void saveNodes(struct mount *mnt);
_PREFIX void saveNodes(struct mount *mnt)
#ifndef KERNEL
{call(41, (size_t)mnt, 0, 0, 0);}
#endif
;

#endif // MYFS_H_INCLUDED
