CCFLAGS=-w -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -m32

ifdef OSX_TOOLSET
LDFLAGS=-preload -arch i386 -segaddr __TEXT 0 -U start
else
LDFLAGS=-Ttext 0 --oformat binary
endif

TARGET=$(BUILD_PATH)/shell.bin
OBJS=$(addprefix $(BUILD_PATH)/shell/, shell.o common.o screen.o keyboard.o)

all: $(TARGET)

$(TARGET) : $(OBJS)
	ld $(LDFLAGS) -o $@ $^

$(BUILD_PATH)/shell/shell.o : shell.c
	gcc $(CCFLAGS) -c -o $@ shell.c

$(BUILD_PATH)/shell/common.o : common.c
	gcc $(CCFLAGS) -c -o $@ common.c

$(BUILD_PATH)/shell/screen.o : screen.c
	gcc $(CCFLAGS) -c -o $@ screen.c

$(BUILD_PATH)/shell/keyboard.o : keyboard.c
	gcc $(CCFLAGS) -c -o $@ keyboard.c

