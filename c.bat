main %1.d
nasm -f elf32 %1.asm -o %1.o
gcc print.o %1.o -o %1 -m32
%1