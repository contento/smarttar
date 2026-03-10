;       HELLO3.ASM

;       masm hello3;
;               -- can use other assemblers
;       link /stack:1024 hello3,,,os2,protmode
;               -- or use DOSCALLS.LIB instead of OS2.LIB
;       run286 hello3

        .286

stdout  equ     1

EXTRN   DosWrite:far
EXTRN   DosExit:far

_DATA   SEGMENT WORD PUBLIC 'DATA'
msg1    db      'INT 21h AH=40h: Hello world!', 0dh, 0ah
len1    equ     $-msg1

msg2    db      'DosWrite: Hello world!', 0dh, 0ah
len2    equ     $-msg2

wlen    dw      ?
_DATA   ENDS

DGROUP  GROUP   _DATA
ASSUME  CS:_TEXT, DS:DGROUP, SS:DGROUP

_TEXT   SEGMENT WORD PUBLIC 'CODE'
main    PROC    FAR

        ; make DOS call
ok:     mov     bx, stdout
        mov     cx, len1
        mov     dx, offset msg1
        mov     ah, 40h
        int     21h
        jc      error

        ; make OS/2 API call (under DOS!)
        push    stdout
        push    ds
        push    offset msg2
        push    len2
        push    ds
        push    offset wlen
        call    DosWrite
        or      ax, ax
        jnz     error

        push    1
        push    0
        call    DosExit

error:  push    1
        push    1
        call    DosExit
main    ENDP
_TEXT   ENDS

        END     main

