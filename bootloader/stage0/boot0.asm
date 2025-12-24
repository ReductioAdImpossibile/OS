
; Due to the extended bios parameter block at the start of our file system we need to shift by 0x7c00 (start of bootloader) + 62 (bios parameter block) 
[org 0x7C3E]            ; So we need to use this instead! (+62)

STAGE1_OFFSET equ 0x7E00 

mov [BOOT_DRIVE], dl

cli                 
xor ax, ax
mov ss, ax          
mov bp, 0x7C00
mov sp, bp
sti

call load_next_stage
jmp $

%include "print_rm.asm"
%include "disk.asm"

[BITS 16]
load_next_stage:
    
    ; write to STAGE1_OFFSET
    mov ax, 0x7E0               
    mov es, ax
    xor bx, bx

    mov dl , [BOOT_DRIVE]       
    call disk_load

    mov ax , MSG_NEXT_STAGE
    call print_string_rm

    ; jmp to STAGE1_OFFSET
    jmp 0x7E0:0x0000            

BOOT_DRIVE:
    db 0
MSG_NEXT_STAGE:
    db "booting next stage!",0


times 510 - ($-$$) db 0
dw 0xAA55