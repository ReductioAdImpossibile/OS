[org  0x1000]


mov ax, MSG_STAGE1
call print_string_rm

jmp $

MSG_STAGE1:
    db "stage 1 loaded !",0


%include "stage0/print_rm.asm"