# operating-system
Simple operating system for x86 architecture.
![image](https://github.com/user-attachments/assets/2aa6b6d1-652f-4309-a44a-5e3c8c5996da)
![image](https://github.com/user-attachments/assets/57376a2a-f21c-4edb-b2f1-73682d1f90b9)


# Supports
* Video: 80x25 color text mode
* Disk: ATA PIO 28bit
* Keyboard: PS2 US QWERTY

# Features
* 32 bit Protected Mode
* ISR
* Memory manager
* Own file system
* ELF exec
* System calls
* User software
* Shell with an assembler-like language

# Build & run
```
#build
make all

#run
qemu-system-x86_64 -hda myos.img -m 1024

#.img
#./sysimg -h #manual
./sysimg myos.bin myos.img 1000 #[additional files/programs]

#.iso (only bootable kernel)
mkdir iso
cp myos.bin iso/
mkisofs -no-emul-boot -o myos.iso -b myos.bin iso/

#installation
dd if=myos.img of=/dev/sdx
#if you want the system to automatically mount the filesystem then put it on primary master disk.
```
