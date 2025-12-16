
; This program function loads /boot/bootst1 out of our FAT filesystem into the RAM.
; In this OS the file bootst1 is special, because it always has the size of 10 sectors and is placed at sector 38.
; This due to my implementation of the disk img.
; I really dont wanna write a disk driver in assembly. Im sorry.

; To get a variable file size, its necessary to read the FAT at the third entry.


disk_load:

    pusha 

    mov ax, 0x100
    mov es, ax
    xor bx, bx

    mov ah , 0x02   ; BIOS read sector function
    mov al , 10     ; read 10 sectors

    ; Sector 37 from 2880 in CHS format : 
    mov ch , 1   ; cylinder 
    mov dh , 0   ; head 
    mov cl , 2   ; sector
    
    int 0x13

    jc disk_error   

    cmp al , 10
    jne disk_error 

    popa
    ret 

    disk_error :
        mov ax , DISK_ERROR_MSG
        call print_string_rm
        jmp $

DISK_ERROR_MSG:
    db "DISK ERROR. ", 0

