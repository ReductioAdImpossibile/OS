[org  0x1000]

%include "bootloader/stage1/print_rm.asm"


mov ah, 0x0e
mov al, "x"
int 0x10

MSG_STAGE1:
    db "STAGE1",0




