00000000  FA                cli
00000001  8CD8              mov ax,ds
00000003  8ED0              mov ss,ax
00000005  BC0000            mov sp,0x0
00000008  89E5              mov bp,sp
0000000A  FB                sti
0000000B  30F6              xor dh,dh
0000000D  52                push dx
0000000E  E86F00            call 0x80
00000011  FA                cli
00000012  F4                hlt
00000013  55                push bp
00000014  89E5              mov bp,sp
00000016  53                push bx
00000017  B40E              mov ah,0xe
00000019  8A4604            mov al,[bp+0x4]
0000001C  8A7E06            mov bh,[bp+0x6]
0000001F  CD10              int 0x10
00000021  5B                pop bx
00000022  89EC              mov sp,bp
00000024  5D                pop bp
00000025  C3                ret
00000026  53                push bx
00000027  51                push cx
00000028  52                push dx
00000029  56                push si
0000002A  57                push di
0000002B  55                push bp
0000002C  89E5              mov bp,sp
0000002E  81EC0400          sub sp,0x4
00000032  8846FE            mov [bp-0x2],al
00000035  31C0              xor ax,ax
00000037  50                push ax
00000038  31C0              xor ax,ax
0000003A  8A46FE            mov al,[bp-0x2]
0000003D  50                push ax
0000003E  E8D2FF            call 0x13
00000041  83C404            add sp,byte +0x4
00000044  89EC              mov sp,bp
00000046  5D                pop bp
00000047  5F                pop di
00000048  5E                pop si
00000049  5A                pop dx
0000004A  59                pop cx
0000004B  5B                pop bx
0000004C  C3                ret
0000004D  53                push bx
0000004E  51                push cx
0000004F  52                push dx
00000050  56                push si
00000051  57                push di
00000052  55                push bp
00000053  89E5              mov bp,sp
00000055  81EC0400          sub sp,0x4
00000059  8946FC            mov [bp-0x4],ax
0000005C  E90000            jmp 0x5f
0000005F  8B5EFC            mov bx,[bp-0x4]
00000062  803F00            cmp byte [bx],0x0
00000065  0F840E00          jz near 0x77
00000069  B87900            mov ax,0x79
0000006C  E8B7FF            call 0x26
0000006F  8B5EFC            mov bx,[bp-0x4]
00000072  FF46FC            inc word [bp-0x4]
00000075  EBE8              jmp short 0x5f
00000077  89EC              mov sp,bp
00000079  5D                pop bp
0000007A  5F                pop di
0000007B  5E                pop si
0000007C  5A                pop dx
0000007D  59                pop cx
0000007E  5B                pop bx
0000007F  C3                ret
00000080  56                push si
00000081  57                push di
00000082  55                push bp
00000083  89E5              mov bp,sp
00000085  81EC0400          sub sp,0x4
00000089  C746FE7800        mov word [bp-0x2],0x78
0000008E  8B5EFE            mov bx,[bp-0x2]
00000091  31C0              xor ax,ax
00000093  8A07              mov al,[bx]
00000095  E88EFF            call 0x26
00000098  E90000            jmp 0x9b
0000009B  EBFE              jmp short 0x9b
0000009D  89EC              mov sp,bp
0000009F  5D                pop bp
000000A0  5F                pop di
000000A1  5E                pop si
000000A2  C3                ret
