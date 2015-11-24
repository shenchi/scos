export CC=cc
export MAKE=make
export BUILD_PATH=$(abspath $(shell pwd))/build

TARGETS=boot kernel shell edit demo
BUILD_PATHS=$(addprefix $(BUILD_PATH)/, $(TARGETS))

all: $(TARGETS)

$(TARGETS): $(BUILD_PATHS)
	$(MAKE) -C src/$@

$(BUILD_PATHS):
	mkdir -p $@

clean:
	rm -rf $(BUILD_PATH)