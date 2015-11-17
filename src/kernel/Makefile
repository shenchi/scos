CCFLAGS=-w -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -fno-stack-protector -m32 -c

#for GNU-ld
#LDFLAGS=-T link.ld
#for OSX-ld
LDFLAGS=-preload -arch i386 -segaddr __TEXT 0x10000 -U start

#for GNU-ld
#ASFLAGS=-f coff
#for OSX-ld
ASFLAGS=-f macho32


TARGET=$(BUILD_PATH)/kernel.bin
OBJS=$(addprefix $(BUILD_PATH)/, entry.o kernel.o gdt.o idt.o interrupts.o screen.o mem.o timer.o fdc.o dma.o task.o fs.o tm.o)

all: $(TARGET)

$(TARGET) : $(OBJS)
	ld $(LDFLAGS) -o $@ $^ 

$(BUILD_PATH)/entry.o : entry.asm
	nasm $(ASFLAGS) -o $@ entry.asm

$(BUILD_PATH)/kernel.o : kernel.c kernel.h gdt.h idt.h mem.h timer.h fdc.h dma.h task.h fs.h
	gcc $(CCFLAGS) -o $@ kernel.c

$(BUILD_PATH)/gdt.o : gdt.c gdt.h
	gcc $(CCFLAGS) -o $@ gdt.c

$(BUILD_PATH)/idt.o : idt.c idt.h
	gcc $(CCFLAGS) -o $@ idt.c

$(BUILD_PATH)/interrupts.o : interrupts.asm
	nasm $(ASFLAGS) -o $@ interrupts.asm

$(BUILD_PATH)/screen.o : screen.c screen.h kernel.h
	gcc $(CCFLAGS) -o $@ screen.c

$(BUILD_PATH)/mem.o : mem.c mem.h screen.h kernel.h
	gcc $(CCFLAGS) -o $@ mem.c

$(BUILD_PATH)/timer.o : timer.c timer.h screen.h idt.h task.h
	gcc $(CCFLAGS) -o $@ timer.c

$(BUILD_PATH)/fdc.o : fdc.c fdc.h screen.h timer.h dma.h
	gcc $(CCFLAGS) -o $@ fdc.c

$(BUILD_PATH)/dma.o : dma.c dma.h kernel.h
	gcc $(CCFLAGS) -o $@ dma.c

$(BUILD_PATH)/task.o : task.c task.h kernel.h mem.c gdt.h timer.h
	gcc $(CCFLAGS) -o $@ task.c

$(BUILD_PATH)/fs.o : fs.c fs.h kernel.h mem.c fdc.h
	gcc $(CCFLAGS) -o $@ fs.c

$(BUILD_PATH)/tm.o : tm.c tm.h
	gcc $(CCFLAGS) -o $@ tm.c
