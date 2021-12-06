GCCARG = -m32 -nostdlib -fno-builtin -fno-exceptions -fno-leading-underscore -O1 -fno-pic
ASMARG = --32
LDARG = -melf_i386

objects = src/boot.o src/kernel.o src/drivers/screen.o src/drivers/keyboard.o src/system/system.o src/drivers/ata.o src/system/malloc.o src/fs/myfs.o src/cpu/gdt.o src/cpu/gdtFlush.o src/cpu/idt.o src/cpu/isr.o  src/cpu/interrupts.o
%.o: %.c
	gcc $(GCCARG) -o $@ -c $<

%.o: %.s
	as $(ASMARG) -o $@ $<
	
myos.bin: $(objects)
	ld -T src/linker.ld --oformat=binary $^ -o myos.bin $(LDARG)
	
myos.iso: myos.bin
	mkdir iso
	cp $^ iso/$^
	mkisofs -no-emul-boot -o $@ -b $^ iso/
	rm -R iso

all: myos.iso

.PHONY: clean

clean:
	rm -f src/*.o
	rm -f src/drivers/*.o
	rm -f src/cpu/*.o
	rm -f src/system/*.o
	rm -f src/fs/*.o
	rm -f *.iso
	rm -f *.bin
	rm -f src/*.bin
