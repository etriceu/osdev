#include "../include/systemCall.h"

//ata
void f0(uint32_t *p){
	ataRead(p[1], p[2], p[3], (uint8_t*)p[4]);
}
void f1(uint32_t *p){
	ataWrite(p[1], p[2], p[3], (uint8_t*)p[4]);
}
void f2(uint32_t *p){
	*((uint32_t*)p[0]) = ataGetSize(p[1]);
}

//keyboard
void f3(uint32_t *p){
	*((uint8_t*)p[0]) = pollKeys();
}
void f4(uint32_t *p){
	*((uint8_t*)p[0]) = keyID(p[1]);
}
void f5(uint32_t *p){
	*((int8_t*)p[0]) = getKeyStatus(p[1]);
}

//malloc
void f6(uint32_t *p){
	*((uint8_t**)p[0]) = malloc(p[1]);
}
void f7(uint32_t *p){
	free((uint8_t*)p[1]);
}

//myfs
void f8(uint32_t *p){
	*((struct mount**)p[0]) = getMountPoints();
}
void f9(uint32_t *p){
	*((struct mount**)p[0]) = mount(p[1], p[2], p[3]);
}
void f10(uint32_t *p){
	umount((struct mount*)p[1]);
}

void f11(uint32_t *p){
	*((struct node**)p[0]) = newFile((struct mount*)p[1], (const char*)p[2], p[3]);
}
void f12(uint32_t *p){
	*((char**)p[0]) = getFileName((struct node*)p[1], (size_t*)p[2]);
}
void f13(uint32_t *p){
	*((struct node**)p[0]) = findFile((struct mount*)p[1], (const char*)p[2]);
}
void f14(uint32_t *p){
	removeFile((struct node*)p[1]);
}
void f15(uint32_t *p){
	renameFile((struct node*)p[1], (const char*)p[2], p[3]);
}

void f16(uint32_t *p){
	*((struct file**)p[0]) = fopen((struct node*)p[1]);
}
void f17(uint32_t *p){
	fclose((struct file*)p[1]);
}
void f18(uint32_t *p){
	*((uint8_t*)p[0]) = fread((struct file*)p[1], p[2], (uint8_t*)p[3]);
}
void f19(uint32_t *p){
	*((uint8_t*)p[0]) = fwrite((struct file*)p[1], p[2], (uint8_t*)p[3]);
}
void f20(uint32_t *p){
	fflush((struct file*)p[1]);
}

//screen
void f21(uint32_t *p){
	setStyle(p[1]);
}
void f22(uint32_t *p){
	*((uint16_t*)p[0]) = getStyle();
}

void f23(uint32_t *p){
	*((uint8_t*)p[0]) = isCursor();
}
void f24(uint32_t *p){
	disableCursor();
}
void f25(uint32_t *p){
	enableCursor(p[1], p[2]);
}
void f26(uint32_t *p){
	moveCursor(p[1]);
}
void f27(uint32_t *p){
	*((int*)p[0]) = getCursorPos();
}

void f28(uint32_t *p){
	clear();
}
void f29(uint32_t *p){
	print((const char*)p[1]);
}
void f30(uint32_t *p){
	printn((const char*)p[1], p[2]);
}
void f31(uint32_t *p){
	printRaw((const uint16_t*)p[1], p[2]);
}
void f32(uint32_t *p){
	setChar(p[1], p[2], p[3]);
}

//system
void f33(uint32_t *p){
	system((const char*)p[1]);
}

//timer
void f34(uint32_t *p){
	*((int*)p[0]) = getTicks();
}
void f35(uint32_t *p){
	sleep(p[1]);
}

//myfs
void f36(uint32_t *p){
	*((uint8_t*)p[0]) = fseek((struct file*)p[1], p[2]);
}
void f37(uint32_t *p){
	*((size_t*)p[0]) = ftell((struct file*)p[1]);
}
void f38(uint32_t *p){
	ftrim((struct file*)p[1]);
}

//ata
void f39(uint32_t *p){
	ataFill(p[1], p[2], p[3], p[4]);
}

typedef void (*func)();
func calls[40] = {	f0, f1, f2, f3, f4, f5, f6, f7, f8, f9,
					f10, f11, f12, f13, f14, f15, f16, f17, f18, f19,
					f20, f21, f22, f23, f24, f25, f26, f27,
					f28, f29, f30, f31, f32, f33, f34, f35,
					f36, f37, f38, f39};

uint8_t systemCall(Registers *reg)
{
	uint32_t *args = (uint32_t*)reg->ebx;
	uint32_t id = args[0];
	if(id < 0 || id >= sizeof(calls)/sizeof(calls[0]))
	{
		print("Invalid system call.\n");
		return 0;
	}
	calls[id](&args[1]);
	return 1;
}
