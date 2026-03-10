;       hello2.asm
;       uses Microsoft DOSSEG ordering
;       makes INT 21h (DOS) calls
;       runs in 286|DOS-Extender

;       masm hello2;
;       link hello2,,,,protmode
;       run286 hello2

        .286
        dosseg
        .model small

stdout  equ     1		; changed from 286dosx Developer's Guide, p. 57

cr      equ     0dh
lf      equ     0ah

        .data

msg     db      cr, lf
        db      'Hello world!', cr, lf
msg_len equ     $-msg

wlen    dw      0

        .stack

        .code

print   proc    far

;       don't need to load DS in DOSSEG program

        mov     ah, 40h
        mov     bx, stdout
        mov     cx, msg_len
        mov     dx, offset msg
        int     21h

        mov     ax, 4c00h
        int     21h
print   endp

        end     print

