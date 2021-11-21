.extern isrHandler

isr_common:
	cli
	pusha
	mov %ds, %ax
	push %eax
	mov $16, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	
	call isrHandler
	
	pop %eax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	popa
	
	add $8, %esp
	sti
iret

.global isr0
.global isr1
.global isr2
.global isr3
.global isr4
.global isr5
.global isr6
.global isr7
.global isr8
.global isr9
.global isr10
.global isr11
.global isr12
.global isr13
.global isr14
.global isr15
.global isr16
.global isr17
.global isr18
.global isr19
.global isr20
.global isr21
.global isr22
.global isr23
.global isr24
.global isr25
.global isr26
.global isr27
.global isr28
.global isr29
.global isr30
.global isr31

isr0:
	push $0
	push $0
	jmp isr_common

isr1:
	push $0
	push $1
	jmp isr_common

isr2:
	push $0
	push $2
	jmp isr_common

isr3:
	push $0
	push $3
	jmp isr_common

isr4:
	push $0
	push $4
	jmp isr_common

isr5:
	push $0
	push $5
	jmp isr_common

isr6:
	push $0
	push $6
	jmp isr_common

isr7:
	push $0
	push $7
	jmp isr_common

isr8:
	push $8
	jmp isr_common

isr9:
	push $0
	push $9
	jmp isr_common

isr10:
	push $10
	jmp isr_common

isr11:
	push $11
	jmp isr_common

isr12:
	push $12
	jmp isr_common

isr13:
	push $13
	jmp isr_common

isr14:
	push $14
	jmp isr_common

isr15:
	push $0
	push $15
	jmp isr_common

isr16:
	push $0
	push $16
	jmp isr_common

isr17:
	push $17
	jmp isr_common

isr18:
	push $0
	push $18
	jmp isr_common

isr19:
	push $0
	push $19
	jmp isr_common

isr20:
	push $0
	push $20
	jmp isr_common

isr21:
	push $0
	push $21
	jmp isr_common

isr22:
	push $0
	push $22
	jmp isr_common

isr23:
	push $0
	push $23
	jmp isr_common

isr24:
	push $0
	push $24
	jmp isr_common

isr25:
	push $0
	push $25
	jmp isr_common

isr26:
	push $0
	push $26
	jmp isr_common

isr27:
	push $0
	push $27
	jmp isr_common

isr28:
	push $0
	push $28
	jmp isr_common

isr29:
	push $0
	push $29
	jmp isr_common

isr30:
	push $30
	jmp isr_common

isr31:
	push $0
	push $31
	jmp isr_common
