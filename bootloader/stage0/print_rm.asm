
print_string_rm:
    pusha

    mov bx,ax
    mov si, 0

    loop:

        mov ah, 0x0e
        mov al, [bx + si]
        int 0x10

        inc si
        cmp al, 0
        jne loop

    popa
    ret

print_hex_rm:
    pusha

    mov bx, ax          ; Kopie von AX
    mov cx, 4           ; 4 Hex-Ziffern
    print_hex_loop:

        
        ; nehme ersten 4 bits und platziere sie hinten.
        rol bx, 4       

        ; ===================================
        ; manuelle implementierung von rol
        ;mov dx, bx
        ;and dx, 0xF000
        ;shr dx, 12

        ;shl bx, 4
        ;or bx, dx
        ; ===================================

        mov al, bl
        and al, 0x0F    ; Nibble isolieren
        cmp al, 9
        jbe digit
        add al, 'A'-10
        jmp out_char

    digit:
        add al, '0'

    out_char:
        mov ah, 0x0E
        int 0x10
        loop print_hex_loop
    
    popa