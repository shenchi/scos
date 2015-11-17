export CC=cc
export MAKE=make
export BUILD_PATH=$(abspath $(shell pwd))/build

TARGETS=boot kernel

all: $(TARGETS)

$(TARGETS): $(BUILD_PATH)
	$(MAKE) -C src/$@

$(BUILD_PATH):
	mkdir -p $(BUILD_PATH)

clean:
	rm -rf $(BUILD_PATH)