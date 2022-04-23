#include "../include/systemCall.h"

uint32_t call(uint32_t id, uint32_t a, uint32_t b, uint32_t c, uint32_t d)
{
	uint32_t ret = 0;
	uint32_t args[] = {id, (uint32_t)&ret, a, b, c, d};
	asm volatile("mov %0, %%ebx;" :: "a" (args) : "%ebx");
	asm volatile("int $128");
	return ret;
}

//ata
void ataRead(uint8_t device, uint32_t lba, uint8_t n, uint8_t *dest){
	call(0, device, lba, n, (uint32_t)dest);
}
void ataWrite(uint8_t device, uint32_t lba, uint8_t n, uint8_t *src){
	call(1, device, lba, n, (uint32_t)src);
}
uint32_t ataGetSize(uint8_t device){
	return call(2, device, 0, 0, 0);
}

//keyboard
uint8_t pullKeys(){
	return call(3, 0, 0, 0, 0);
}
uint8_t keyID(uint8_t key){
	return call(4, key, 0, 0, 0);
}
int8_t getKeyStatus(uint8_t key){
	return call(5, key, 0, 0, 0);
}

//malloc
void* malloc(size_t size){
	return (uint8_t*)call(6, size, 0, 0, 0);
}
void free(void *ptr){
	call(7, (uint32_t)ptr, 0, 0, 0);
}

//myfs
struct mount* getMountPoints(){
	return (struct mount*)call(8, 0, 0, 0, 0);
}
struct mount* mount(uint8_t device, uint32_t begin, uint32_t end){
	return (struct mount*)call(9, device, begin, end, 0);
}
void umount(struct mount* mnt){
	call(10, (uint32_t)mnt, 0, 0, 0);
}

struct node* newFile(struct mount *mnt, const char *name){
	return (struct node*)call(11, (uint32_t)mnt, (uint32_t)name, 0, 0);
}
void getFileName(struct node *nod, char *dest){
	call(12, (uint32_t)nod, (uint32_t)dest, 0, 0);
}
struct node* findFile(struct mount *mnt, const char* name){
	return (struct node*)call(13, (uint32_t)mnt, (uint32_t)name, 0, 0);
}
void removeFile(struct node* nod){
	call(14, (uint32_t)nod, 0, 0, 0);
}
void renameFile(struct node* nod, const char *name){
	call(15, (uint32_t)nod, (uint32_t)name, 0, 0);
}

struct file* fopen(struct node* nod, uint64_t *sizeOut, uint8_t mode){
	return (struct file*)call(16, (uint32_t)nod, (uint32_t)sizeOut, (uint32_t)mode, 0);
}
void fclose(struct file *file){
	call(17, (uint32_t)file, 0, 0, 0);
}
uint8_t fread(struct file *file, size_t size, uint8_t *dest){
	return call(18, (uint32_t)file, size, (uint32_t)dest, 0);
}
void fwrite(struct file *file, size_t size, uint8_t *src){
	call(19, (uint32_t)file, size, (uint32_t)src, 0);
}
void fflush(struct file *file){
	call(20, (uint32_t)file, 0, 0, 0);
}

//screen
void setStyle(uint16_t s){
	call(21, s, 0, 0, 0);
}
uint16_t getStyle(){
	return call(22, 0, 0, 0, 0);
}

int8_t isCursor(){
	return call(23, 0, 0, 0, 0);
}
void disableCursor(){
	call(24, 0, 0, 0, 0);
}
void enableCursor(uint8_t start, uint8_t end){
	call(25, start, end, 0, 0);
}
void moveCursor(int pos){
	call(26, pos, 0, 0, 0);
}
int getCursorPos(){
	return call(27, 0, 0, 0, 0);
}

void clear(){
	call(28, 0, 0, 0, 0);
}
void print(const char *str){
	call(29, (uint32_t)str, 0, 0, 0);
}
void printn(const char *str, size_t size){
	call(30, (uint32_t)str, size, 0, 0);
}
void printRaw(const uint16_t *str, size_t size){
	call(31, (uint32_t)str, size, 0, 0);
}
void setChar(int pos, char c, uint8_t s){
	call(32, pos, c, s, 0);
}

//system
void system(const char *cmd){
	call(33, (uint32_t)cmd, 0, 0, 0);
}

//timer
uint32_t getTicks(){
	return call(34, 0, 0, 0, 0);
}
void sleep(uint32_t ms){
	call(35, ms, 0, 0, 0);
}
