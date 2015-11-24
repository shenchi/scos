CCFLAGS=-w -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -fno-stack-protector -m32 -c

ifdef OSX_TOOLSET
LDFLAGS=-preload -arch i386 -segaddr __TEXT 0x10000 -U start
ASFLAGS=-f macho32
else
LDFLAGS=-T link.ld
ASFLAGS=-f coff
endif

TARGET=$(BUILD_PATH)/kernel.bin
OBJS=$(addprefix $(BUILD_PATH)/kernel/, entry.o kernel.o gdt.o idt.o interrupts.o screen.o mem.o timer.o fdc.o dma.o task.o fs.o tm.o)

all: $(TARGET)

$(TARGET) : $(OBJS)
	ld $(LDFLAGS) -o $@ $^ 

$(BUILD_PATH)/kernel/entry.o : entry.asm
	nasm $(ASFLAGS) -o $@ entry.asm

$(BUILD_PATH)/kernel/kernel.o : kernel.c kernel.h gdt.h idt.h mem.h timer.h fdc.h dma.h task.h fs.h
	gcc $(CCFLAGS) -o $@ kernel.c

$(BUILD_PATH)/kernel/gdt.o : gdt.c gdt.h
	gcc $(CCFLAGS) -o $@ gdt.c

$(BUILD_PATH)/kernel/idt.o : idt.c idt.h
	gcc $(CCFLAGS) -o $@ idt.c

$(BUILD_PATH)/kernel/interrupts.o : interrupts.asm
	nasm $(ASFLAGS) -o $@ interrupts.asm

$(BUILD_PATH)/kernel/screen.o : screen.c screen.h kernel.h
	gcc $(CCFLAGS) -o $@ screen.c

$(BUILD_PATH)/kernel/mem.o : mem.c mem.h screen.h kernel.h
	gcc $(CCFLAGS) -o $@ mem.c

$(BUILD_PATH)/kernel/timer.o : timer.c timer.h screen.h idt.h task.h
	gcc $(CCFLAGS) -o $@ timer.c

$(BUILD_PATH)/kernel/fdc.o : fdc.c fdc.h screen.h timer.h dma.h
	gcc $(CCFLAGS) -o $@ fdc.c

$(BUILD_PATH)/kernel/dma.o : dma.c dma.h kernel.h
	gcc $(CCFLAGS) -o $@ dma.c

$(BUILD_PATH)/kernel/task.o : task.c task.h kernel.h mem.c gdt.h timer.h
	gcc $(CCFLAGS) -o $@ task.c

$(BUILD_PATH)/kernel/fs.o : fs.c fs.h kernel.h mem.c fdc.h
	gcc $(CCFLAGS) -o $@ fs.c

$(BUILD_PATH)/kernel/tm.o : tm.c tm.h
	gcc $(CCFLAGS) -o $@ tm.c
