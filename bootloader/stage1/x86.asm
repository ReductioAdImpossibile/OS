[bits 16] 
section _TEXT class=CODE 

global _x86_write_char 
_x86_write_char: 
    
    push bp
    mov bp, sp  
    
    
    mov ax, 0x00000025
    mov ds, ax
    
    mov ah, 0x0E 
    mov al, [bp+4]  
    xor bh, bh
    int 0x10

    mov sp, bp 
    pop bp 
    ret
