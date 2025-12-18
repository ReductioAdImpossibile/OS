BIN_DIR := ../../bin

NASM := nasm
NASM_FLAGS := -f obj

CC16 := /usr/bin/watcom/binl64/wcc
CFLAGS16 := -4 -d3 -s -wx -ms -zl -zq
LD16 := /usr/bin/watcom/binl64/wlink

SOURCES_C := $(wildcard *.c)
SOURCES_ASM := $(wildcard *.asm)

OBJECTS_C := $(patsubst %.c, $(BIN_DIR)/stage1/c/%.obj, $(SOURCES_C))
OBJECTS_ASM := $(patsubst %.asm, $(BIN_DIR)/stage1/asm/%.obj, $(SOURCES_ASM))

.PHONY: all
all: $(BIN_DIR)/stage1.bin

$(BIN_DIR)/stage1.bin: $(OBJECTS_C) $(OBJECTS_ASM)
	$(LD16) NAME $@ FILE { $(OBJECTS_ASM) $(OBJECTS_C) } OPTION MAP=$(BIN_DIR)/stage1.map @linker.lnk


$(BIN_DIR)/stage1/c/%.obj: %.c | $(BIN_DIR)/stage1/c
	$(CC16) $(CFLAGS16) -fo=$@ $<

$(BIN_DIR)/stage1/asm/%.obj: %.asm | $(BIN_DIR)/stage1/asm
	$(NASM) $(NASM_FLAGS) -o $@ $<