#include "../include/systemCall.h"

//ata
void f0(Registers reg){
	ataRead(reg.esi, reg.ebp, reg.ebx, (uint8_t*)reg.edx);
}
void f1(Registers reg){
	ataWrite(reg.esi, reg.ebp, reg.ebx, (uint8_t*)reg.edx);
}
void f2(Registers reg){
	*((uint32_t*)reg.ecx) = ataGetSize(reg.esi);
}

//keyboard
void f3(Registers reg){
	*((uint8_t*)reg.ecx) = isKeyEvent();
}
void f4(Registers reg){
	*((uint8_t*)reg.ecx) = getKey();
}
void f5(Registers reg){
	*((int8_t*)reg.ecx) = getKeyStatus(reg.esi);
}

//malloc
void f6(Registers reg){
	*((uint8_t**)reg.ecx) = malloc(reg.esi);
}
void f7(Registers reg){
	free((uint8_t*)reg.esi);
}

//myfs
void f8(Registers reg){
	*((struct mount**)reg.ecx) = getMountPoints();
}
void f9(Registers reg){
	*((struct mount**)reg.ecx) = mount(reg.esi, reg.ebp, reg.ebx);
}
void f10(Registers reg){
	umount((struct mount*)reg.esi);
}

void f11(Registers reg){
	*((struct node**)reg.ecx) = newFile((struct mount*)reg.esi, (const char*)reg.ebp);
}
void f12(Registers reg){
	getFileName((struct node*)reg.esi, (char*)reg.ebp);
}
void f13(Registers reg){
	*((struct node**)reg.ecx) = findFile((struct mount*)reg.esi, (const char*)reg.ebp);
}
void f14(Registers reg){
	removeFile((struct node*)reg.esi);
}
void f15(Registers reg){
	renameFile((struct node*)reg.esi, (const char*)reg.ebp);
}

void f16(Registers reg){
	*((struct file**)reg.ecx) = fopen((struct node*)reg.esi, (uint64_t*)reg.ebp);
}
void f17(Registers reg){
	fclose((struct file*)reg.esi);
}
void f18(Registers reg){
	*((uint8_t*)reg.ecx) = fread((struct file*)reg.esi, reg.ebp, (uint8_t*)reg.ebx);
}
void f19(Registers reg){
	fwrite((struct file*)reg.esi, reg.ebp, (uint8_t*)reg.ebx);
}
void f20(Registers reg){
	fflush((struct file*)reg.esi);
}

//screen
void f21(Registers reg){
	setStyle(reg.esi);
}
void f22(Registers reg){
	*((uint8_t*)reg.ecx) = getStyle();
}
void f23(Registers reg){
	*((unsigned int*)reg.ecx) = getWidth();
}
void f24(Registers reg){
	*((unsigned int*)reg.ecx) = getHeight();
}

void f25(Registers reg){
	*((uint8_t*)reg.ecx) = isCursor();
}
void f26(Registers reg){
	disableCursor();
}
void f27(Registers reg){
	enableCursor(reg.esi, reg.ebp);
}
void f28(Registers reg){
	moveCursor(reg.esi);
}
void f29(Registers reg){
	*((int*)reg.ecx) = getCursorPos();
}

void f30(Registers reg){
	clear();
}
void f31(Registers reg){
	print((const char*)reg.esi);
}
void f32(Registers reg){
	printn((const char*)reg.esi, reg.ebp);
}
void f33(Registers reg){
	printRaw((const uint16_t*)reg.esi, reg.ebp);
}
void f34(Registers reg){
	setChar(reg.esi, reg.ebp, reg.ebx);
}

//system
void f35(Registers reg){
	system((const char*)reg.esi);
}

typedef void (*func)();
func calls[36] = {	f0, f1, f2, f3, f4, f5, f6, f7, f8, f9,
					f10, f11, f12, f13, f14, f15, f16, f17, f18, f19,
					f20, f21, f22, f23, f24, f25, f26, f27, f28, f29,
					f30, f31, f32, f33, f34, f35};

void systemCall(Registers reg)
{
	if(reg.edi < 0 || reg.edi >= sizeof(calls)/sizeof(calls[0]))
	{
		print("Invalid system call.\n");
		return;
	}
	calls[reg.edi](reg);
}
