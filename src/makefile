kernel.bin : boot.bin entry.o kernel.o gdt.o idt.o interrupts.o screen.o mem.o timer.o fdc.o dma.o task.o fs.o tm.o
	ld -T link.ld -o kernel.bin entry.o kernel.o gdt.o idt.o interrupts.o screen.o mem.o timer.o fdc.o dma.o task.o fs.o tm.o

entry.o : kernel\entry.asm
	nasm -f coff -o entry.o kernel\entry.asm

kernel.o : kernel\kernel.c kernel\kernel.h kernel\gdt.h kernel\idt.h kernel\mem.h kernel\timer.h kernel\fdc.h kernel\dma.h kernel\task.h kernel\fs.h
	gcc -w -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c -o kernel.o kernel\kernel.c

gdt.o : kernel\gdt.c kernel\gdt.h
	gcc -w -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c -o gdt.o kernel\gdt.c

idt.o : kernel\idt.c kernel\idt.h
	gcc -w -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c -o idt.o kernel\idt.c

interrupts.o : kernel\interrupts.asm
	nasm -f coff -o interrupts.o kernel\interrupts.asm

screen.o : kernel\screen.c kernel\screen.h kernel\kernel.h
	gcc -w -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c -o screen.o kernel\screen.c

mem.o : kernel\mem.c kernel\mem.h kernel\screen.h kernel\kernel.h
	gcc -w -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c -o mem.o kernel\mem.c

timer.o : kernel\timer.c kernel\timer.h kernel\screen.h kernel\idt.h kernel\task.h
	gcc -w -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c -o timer.o kernel\timer.c

fdc.o : kernel\fdc.c kernel\fdc.h kernel\screen.h kernel\timer.h kernel\dma.h
	gcc -w -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c -o fdc.o kernel\fdc.c

dma.o : kernel\dma.c kernel\dma.h kernel\kernel.h
	gcc -w -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c -o dma.o kernel\dma.c

task.o : kernel\task.c kernel\task.h kernel\kernel.h kernel\mem.c kernel\gdt.h kernel\timer.h
	gcc -w -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c -o task.o kernel\task.c

fs.o : kernel\fs.c kernel\fs.h kernel\kernel.h kernel\mem.c kernel\fdc.h
	gcc -w -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c -o fs.o kernel\fs.c

tm.o : kernel\tm.c kernel\tm.h
	gcc -w -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c -o tm.o kernel\tm.c

boot.bin : boot\boot.asm
	nasm -o boot.bin boot\boot.asm