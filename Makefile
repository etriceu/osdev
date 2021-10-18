GCCARG = -m32 -nostdlib -fno-builtin -fno-exceptions -fno-leading-underscore
ASMARG = --32
LDARG = -melf_i386

objects = loader.o kernel.o screen.o keyboard.o system.o ata.o
%.o: %.c
	gcc $(GCCARG) -o $@ -c $<

%.o: %.s
	as $(ASMARG) -o $@ $<
	
kernel.bin: linker.ld $(objects)
	ld $(LDARG) -T $< -o $@ $(objects)
	
kernel.iso: kernel.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp $< iso/boot/
	echo 'set timeout-0' >> iso/boot/grub/grub.cfg
	echo 'set default-0' >> iso/boot/grub/grub.cfg
	echo 'menuentry "myos"{' >> iso/boot/grub/grub.cfg
	echo '  multiboot /boot/kernel.bin' >> iso/boot/grub/grub.cfg
	echo ' boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso
	rm -rf iso
