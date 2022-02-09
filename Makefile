GCCARG = -m32 -nostdlib -fno-builtin -fno-exceptions -fno-leading-underscore -O1 -fno-pic -fno-stack-protector
GCCARG_USER = -m32 -nostdlib -nostartfiles -fno-builtin -fno-stack-protector -O3 -e main

ASMARG = --32
LDARG = -melf_i386

objects = src/boot.o src/kernel.o src/drivers/screen.o src/drivers/keyboard.o src/system/system.o src/drivers/ata.o src/system/malloc.o src/fs/myfs.o src/cpu/gdt.o src/cpu/gdtFlush.o src/cpu/idt.o src/cpu/isr.o src/cpu/interrupts.o src/system/systemCall.o src/system/exec.o src/cpu/irq.o src/cpu/timer.o

userSoftware = helloworld

%.o: %.c
	gcc $(GCCARG) -o $@ -c $<

%.o: %.s
	as $(ASMARG) -o $@ $<
	
myos.bin: $(objects)
	ld -T src/linker.ld --oformat=binary $^ -o myos.bin $(LDARG)

sysimg:
	g++ -O3 src/tools/sysimg.cpp -o sysimg

helloworld:
	gcc $(GCCARG_USER) src/system/systemCalls.c src/userSoftware/$@.c -o $@
	
userSoft: $(userObj) $(userSoftware)

myos.img: myos.bin sysimg userSoft
	./sysimg myos.bin myos.img $(userSoftware)
	
all: myos.img

.PHONY: clean

clean:
	rm -f src/*.o
	rm -f src/*/*.o
	rm -f *.img
	rm -f *.bin
	rm -f $(userSoftware)
