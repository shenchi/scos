CCFLAGS=-w -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -m32

ifdef OSX_TOOLSET
LDFLAGS=-preload -arch i386 -segaddr __TEXT 0 -U start
else
LDFLAGS=-Ttext 0 --oformat binary
endif

TARGET=$(BUILD_PATH)/edit.bin
OBJS=$(addprefix $(BUILD_PATH)/edit/, edit.o common.o screen.o keyboard.o)

all: $(TARGET)

$(TARGET) : $(OBJS)
	ld $(LDFLAGS) -o $@ $^

$(BUILD_PATH)/edit/edit.o : edit.c
	gcc $(CCFLAGS) -c -o $@ edit.c

$(BUILD_PATH)/edit/common.o : common.c
	gcc $(CCFLAGS) -c -o $@ common.c

$(BUILD_PATH)/edit/screen.o : screen.c
	gcc $(CCFLAGS) -c -o $@ screen.c

$(BUILD_PATH)/edit/keyboard.o : keyboard.c
	gcc $(CCFLAGS) -c -o $@ keyboard.c
