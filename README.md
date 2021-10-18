# operating-system
Simple operating system for x86 architecture.

# Why?
I used to joke about creating a kernel but in the end I decided to really make a operating system.
Also once I had a similar project for avr but I lost all files.It made me miss having my own operating system a bit.
Besides, creating gives me fun and new experiences.

# Supports
Video: 80x25 color text mode

Disk: ATA PIO 24bit, only primary master

Keyboard: PS2 US QWERTY

# Build
```make all```

# Boot
MyOS uses the grub system. When creating an iso file, make sure you have the appropriate grub tools (grub-mkrescue).
