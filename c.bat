rem gcc -c -o print.o -m32 print.c

gcc -o main main.c
main
nasm -f elf32 test.asm -o test.o
gcc print.o test.o -o test -m32
test