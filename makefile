
BIN_DIR := bin
OBJ_DIR := object

# FILES
BOOT0_BIN := $(BIN_DIR)/boot0.bin
BOOT1_BIN := $(BIN_DIR)/boot1.bin

BOOT0 := bootloader/stage0/boot0.asm
BOOT1 := bootloader/stage1/boot1.asm

OS_IMAGE := $(BIN_DIR)/disk.img


# TOOLS
NASM := nasm
GCC := gcc
LD := ld
QEMU := qemu-system-i386
MOUNT := ./mountfat12


# DEBUG COMMANDS
# xp /32bx 0x1000



NASM_FLAGS_BIN := -f bin
NASM_FLAGS_ELF := -f elf32
GCC_FLAGS := -fno-pie -m32 -ffreestanding -c
LD_FLAGS := -m elf_i386 -Ttext 0x1000 --oformat binary





exports:
	export PATH=/usr/bin/watcom/binl64:$PATH

dirs:
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)

create: exports dirs

	$(MAKE) -C bootloader
	$(MOUNT) --bootstage0 $(BOOT0_BIN) --bootstage1 $(BOOT1_BIN) --kernel $(BOOT0_BIN) --location $(BIN_DIR)/disk.img
	
all: create
	$(QEMU) -fda $(OS_IMAGE)

debug: create
	$(QEMU) -fda $(OS_IMAGE) -monitor stdio


img:
	$(GCC) -o mountfat12 imgwriter/src/mountfat12.c imgwriter/src/file.c  