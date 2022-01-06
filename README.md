# operating-system
Simple operating system for x86 architecture.

# Supports
Video: 80x25 color text mode

Disk: ATA PIO 24bit

Keyboard: PS2 US QWERTY

# Features
32bit, ISR, memory manager, own file system

# Build & run
```
#build
make all

#run
qemu-system-x86_64 -hda myos.img -m 1024

#.img
#./sysimg -h #manual
./sysimg myos.bin myos.img #[additional files/programs]

#.iso (only bootable kernel)
mkdir iso
cp myos.bin iso/
mkisofs -no-emul-boot -o myos.iso -b myos.bin iso/

#installation
dd if=myos.img of=/dev/sdx
#if you want the system to automatically mount the filesystem then put it on primary master disk.
```
