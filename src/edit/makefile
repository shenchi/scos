edit.bin : edit.o common.o screen.o keyboard.o
	ld -Tt 0 --oformat binary -o edit.bin edit.o common.o screen.o keyboard.o

edit.o : edit.c
	gcc -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c -o edit.o edit.c

common.o : common.c
	gcc -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c -o common.o common.c

screen.o : screen.c
	gcc -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c -o screen.o screen.c

keyboard.o : keyboard.c
	gcc -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c -o keyboard.o keyboard.c
