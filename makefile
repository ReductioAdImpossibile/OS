
# DEBUG COMMANDS
# xp /32bx 0x1000


BIN_DIR := bin
STAGE0_BIN := $(BIN_DIR)/stage0.bin
STAGE1_BIN := $(BIN_DIR)/stage1.bin
OS_IMAGE := $(BIN_DIR)/disk.img

QEMU := qemu-system-i386
MOUNT := ./mountfat12

.PHONY: dirs
dirs:
	mkdir -p $(BIN_DIR)/stage0/asm
	mkdir -p $(BIN_DIR)/stage1/c
	mkdir -p $(BIN_DIR)/stage1/asm


.PHONY: stage0
stage0: dirs
	$(MAKE) -C bootloader/stage0


.PHONY: stage1
stage1: dirs
	$(MAKE) -C bootloader/stage1

.PHONY: create
create: stage0 stage1
	$(MOUNT) --bootstage0 $(STAGE0_BIN) --bootstage1 $(STAGE1_BIN) --kernel $(STAGE1_BIN) --location $(OS_IMAGE)


.PHONY: all
all: create
	$(QEMU) -fda $(OS_IMAGE)

.PHONY: debug
debug: create
	$(QEMU) -fda $(OS_IMAGE) -monitor stdio



disasm_stage1:
	ndisasm -b 16 $(BIN_DIR)/stage1.bin > $(BIN_DIR)/debug/stage1.asm
	
img:
	$(GCC) -o mountfat12 imgwriter/src/mountfat12.c imgwriter/src/file.c  


