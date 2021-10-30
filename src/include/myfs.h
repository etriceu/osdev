#ifndef MYFS_H
#define MYFS_H

#include <stdint.h>

struct node
{
	uint8_t address[3];
	struct node *next;
};

struct file
{
	uint32_t start;
	uint32_t rstart, rsize, rpos;
	uint32_t wstart, wsize, wpos;
};

extern struct node *nodes;

uint8_t mount();
void umount();
void newFile(const char *name);
void getFileName(struct node *nod, char *dest);
struct node* findFile(const char* name);
void removeFile(struct node* nod);
void renameFile(struct node* nod, const char *name);

#endif // MYFS_H_INCLUDED
