gcc -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c -o demo.o demo.c
pause
gcc -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c -o common.o common.c
gcc -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c -o screen.o screen.c
gcc -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c -o keyboard.o keyboard.c
ld -Ttext 0 --oformat binary -o demo.bin demo.o common.o screen.o keyboard.o
pause
