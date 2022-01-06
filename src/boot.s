.code16
.globl _start
_start:
	mov $0, %ax
	mov %ax, %es
	mov $0x7e00, %bx
	mov $0x02, %ah
	mov $64, %al # is 32KB enough?
	mov $0x02, %cl
	mov $0x00, %ch
	mov $0x00, %dh
	
	int $0x13

	mov $0x2401, %ax
	int $0x15
	
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
	hlt
	
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
end:
## myfs structure
.byte 64 # 32KB
.byte 0
.byte 0
#next
.byte 0
.byte 0
.byte 0
#last sector size
.byte 0
.byte 0
#name
.ascii "kernel.bin\0"
. = end + 512
