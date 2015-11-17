TARGET=$(BUILD_PATH)/boot.bin

all: $(TARGET)

$(TARGET) : boot.asm
	nasm -o $@ $^
