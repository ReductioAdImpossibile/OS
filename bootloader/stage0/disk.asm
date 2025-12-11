
; This program function loads /boot/bootst1 out of our FAT filesystem into the RAM.
; In this OS the file bootst1 is special, because it always has the size of 10 sectors and is placed at sector 35.
; I really dont wanna write a disk driver in assembly. Im sorry.

; To get a variable file size, its necessary to read the FAT at the third entry.


disk_load:

    pusha 
    mov ah , 0x02   ; BIOS read sector function
    mov al , 10     ; read 10 sectors

    mov ch , 0x00   ; cylinder 0
    mov dh , 0x00   ; head 0
    mov cl , 35     ; Sector 35
    int 0x13

    jc disk_error   ; Falls die Carry Flag aktiv ist -> Fehler

    pop dx
    cmp dh , al
    jne disk_error ; Falls die tatsächlich gelesenen Sektoren ungleich der gewollten sind -> Fehler

    popa
    ret 

    disk_error :
        mov bx , DISK_ERROR_MSG
        call print_string_rm
        jmp $










DISK_ERROR_MSG:
    db "DISK ERROR. ", 0







