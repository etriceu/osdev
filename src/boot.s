.code16
.globl _start
_start:
	#load kernel info
	xor %ax, %ax
	mov %ax, %es
	mov $0x7e00, %bx # desc addr
	mov $0x02, %ah # read sectors from drive
	mov $0x01, %al # 1 sector (512B) to read
	mov $0x02, %cl # sector 2
	mov $0x00, %ch # cylinder 0
	mov $0x00, %dh # head 0
	mov $0x80, %dl # first hard drive
	int $0x13
	
	#load kernel
	mov $0x8000, %bx
	mov $0x02, %ah
	mov 0x7e00, %al # load kernel size in sectors
	mov 0x7e01, %cl # load address
	mov 0x7e02, %ch
	mov 0x7e03, %dh
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
	#first always empty
	.word 0, 0
	.byte 0, 0, 0, 0
	#from 0 to 4GB, code, full access
	.word 0xffff, 0
	.byte 0, 0x9f, 0xcf, 0
	#from 0 to 4GB, data, full access
	.word 0xffff, 0
	.byte 0, 0x93, 0xcf, 0
	
gdtReg:
	.word 0x27
	.long gdt

. = _start + 510
.word 0xaa55
# default information about kernel and first partition
# may be changed by sysimg
.byte 0x7f # kernel size
.byte 0x03 # kernel hard drive address
.byte 0x00
.byte 0x00
.long 0x00 # LBA, myfs start address, 0 - none
.long 0x00 # myfs end address

. = _start + 1024
