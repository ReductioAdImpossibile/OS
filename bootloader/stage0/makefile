NASM := nasm
NASM_FLAGS_BIN := -f bin

BIN_DIR := ../../bin

BOOT0 := boot0.asm
STAGE0_BIN := $(BIN_DIR)/stage0.bin

.PHONY: all
all: $(STAGE0_BIN)

$(OUT_DIR):
	mkdir -p $@

$(STAGE0_BIN): $(BOOT0) | $(OUT_DIR)
	$(NASM) $(NASM_FLAGS_BIN) $< -o $@
