#test script;

set a "hello world!\n";

#load b - byte to register/variable b from address a;
ld b b a;

# 'h'-32 = 'H';
set c 32;
sub b c;

st b b a;

#syscall 29 - print, return to r, x - unused args;
int 29 r a x x x;
