

[org 0x7c00]

; Position des Kernels im RAM:
KERNEL_OFFSET equ 0x1000 

; Anfangs übergibt das Bios das zum booten genutzte Laufwerk dem register dl.
; (also 0x00 oder 0x80 etc)
; Dieses Speichern wir uns ab.
mov [BOOT_DRIVE], dl

mov bp , 0x9000
mov sp , bp

call load_next_stage
jmp $

; Hier wird der Code aus den anderen files direkt eingefügt.
; Die Makefile wird im oberen Verzeichnis ausgeführt also müssen wir hier boot/* hinzufügen.
%include "bootloader/stage0/print_rm.asm"
%include "bootloader/stage0/disk.asm"


; Hier wird eine Nachricht ausgegeben, danach laden wir sektoren 2 bis 15 in den RAM
[BITS 16]
load_next_stage:
    
    mov bx , KERNEL_OFFSET      ; Ort zum Laden
    mov dl , [BOOT_DRIVE]       ; Nehme das gewählte Laufwerk, auf welchem das OS installiert ist. (meistens 0x00)
    ; Aus disk.asm
    call disk_load


    mov ax , MSG_NEXT_STAGE
    call print_string_rm

    ret


BOOT_DRIVE:
    db 0
MSG_NEXT_STAGE:
    db "booting next stage!",0


times 510 - ($-$$) db 0
dw 0xAA55