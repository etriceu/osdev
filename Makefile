GCCARG = -m32 -nostdlib -fno-builtin -fno-exceptions -fno-leading-underscore -O1
ASMARG = --32
LDARG = -melf_i386

objects = src/loader.o src/kernel.o src/drivers/screen.o src/drivers/keyboard.o src/system/system.o src/drivers/ata.o src/system/malloc.o src/fs/myfs.o src/cpu/gdt.o src/cpu/gdtFlush.o
%.o: %.c
	gcc $(GCCARG) -o $@ -c $<

%.o: %.s
	as $(ASMARG) -o $@ $<
	
kernel.bin: src/linker.ld $(objects)
	ld $(LDARG) -T $< -o $@ $(objects)
	
myos.iso: kernel.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp $< iso/boot/
	echo 'set timeout=5' >> iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo 'menuentry "myos"{' >> iso/boot/grub/grub.cfg
	echo '  multiboot /boot/kernel.bin' >> iso/boot/grub/grub.cfg
	echo ' boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=myos.iso iso
	rm -rf iso

all: myos.iso

.PHONY: clean

clean:
	rm -f src/*.o
	rm -f src/drivers/*.o
	rm -f src/cpu/*.o
	rm -f src/system/*.o
	rm -f src/fs/*.o
	rm -f myos.iso
	rm -f kernel.bin
