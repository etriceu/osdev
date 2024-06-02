.code16
.globl _start
_start:
	#load kernel info
	xor %ax, %ax
	mov %ax, %es
	mov $0x500, %bx # desc addr
	mov $0x02, %ah # read sectors from drive
	mov $0x01, %al # 1 sector (512B) to read
	mov $0x02, %cl # sector
	mov $0x00, %ch # head 0
	mov $0x00, %dh # drive 0
	int $0x13
	
	#load kernel
	mov $0x7e00, %bx
	mov $0x02, %ah
	mov 0x500, %al # load kernel size in sectors
	mov 0x501, %cl # load address
	mov 0x502, %ch
	mov 0x503, %dh
	int $0x13
	
	cli
	lgdt gdtReg
	mov %cr0, %eax
	or $0x1, %al
	mov %eax, %cr0
	jmp $0x08, $protMode
	
.code32
.extern kernelMain
protMode:
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %ss
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	call kernelMain
halt:
	hlt
	jmp halt
	
.p2align 2
gdt:
	.word 0, 0
	.byte 0, 0, 0, 0
	.word 0xffff, 0
	.byte 0, 0x9a, 0xcf, 0
	.word 0xffff, 0
	.byte 0, 0x92, 0xcf, 0
	.word 0xffff, 0
	.byte 0, 0x9e, 0, 0
	.word 0xffff, 0
	.byte 0, 0x92, 0, 0
	
gdtReg:
	.word 0x27
	.long gdt

. = _start + 510
.word 0xaa55
