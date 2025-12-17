
[org 0x7C3E]            ; So we need to use this instead! (+62)

STAGE1_OFFSET equ 0x1000 

mov [BOOT_DRIVE], dl

mov bp , 0x9000
mov sp , bp

call load_next_stage
jmp $

%include "print_rm.asm"
%include "disk.asm"

[BITS 16]
load_next_stage:
    
    mov dl , [BOOT_DRIVE]       
    call disk_load

    mov ax , MSG_NEXT_STAGE
    call print_string_rm

    jmp 0x100:0x0000

BOOT_DRIVE:
    db 0
MSG_NEXT_STAGE:
    db "booting next stage!",0


times 510 - ($-$$) db 0
dw 0xAA55