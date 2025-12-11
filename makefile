
BIN_DIR := bin
OBJ_DIR := object

BOOT0 := bootloader/stage0/boot0.asm
BOOT0_BIN := $(BIN_DIR)/boot0.bin



# TOOLS
NASM := nasm
GCC := gcc
LD := ld
QEMU := qemu-system-i386
MOUNT := ./mountfat12


NASM_FLAGS_BIN := -f bin
NASM_FLAGS_ELF := -f elf32
GCC_FLAGS := -fno-pie -m32 -ffreestanding -c
LD_FLAGS := -m elf_i386 -Ttext 0x1000 --oformat binary

all:
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)
	$(NASM) $(NASM_FLAGS_BIN) $(BOOT0) -o $(BOOT0_BIN)





	$(MOUNT) --bootstage0 $(BOOT0_BIN) --bootstage1 $(BOOT0_BIN) --kernel $(BOOT0_BIN) --location $(BIN_DIR)/disk.img

img:
	$(GCC) -o mountfat12 imgwriter/src/mountfat12.c imgwriter/src/file.c  