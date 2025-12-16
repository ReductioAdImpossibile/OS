


NASM := nasm
NASM_FLAGS_BIN := -f bin

#FILES
BIN_DIR := ../bin
BOOT0_BIN := $(BIN_DIR)/boot0.bin
BOOT1_BIN := $(BIN_DIR)/boot1.bin

BOOT0 := stage0/boot0.asm
BOOT1 := stage1/boot1.asm


STAGE0:
	$(NASM) $(NASM_FLAGS_BIN) $(BOOT0) -o $(BOOT0_BIN)

STAGE1:
	$(NASM) $(NASM_FLAGS_BIN) $(BOOT1) -o $(BOOT1_BIN)

all: STAGE0 STAGE1
	
	