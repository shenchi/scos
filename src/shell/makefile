shell.bin : shell.o common.o screen.o keyboard.o
	ld -Ttext 0 --oformat binary -o shell.bin shell.o common.o screen.o keyboard.o

shell.o	: shell.c
	gcc -w -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c -o shell.o shell.c

common.o : common.c
	gcc -w -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c -o common.o common.c

screen.o : screen.c
	gcc -w -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c -o screen.o screen.c

keyboard.o : keyboard.c
	gcc -w -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c -o keyboard.o keyboard.c

