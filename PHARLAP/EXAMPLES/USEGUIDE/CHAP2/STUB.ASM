;       STUB.ASM

;       masm stub;
;       link stub;

_DATA   segment word public 'DATA'
msg     db      "This program requires Phar Lap's 286|DOS-Extender."
        db      0dh,0ah,'$'
_DATA   ends

_STACK  segment para stack 'STACK'
_STACK  ends

_TEXT   segment word public 'CODE'

        assume CS:_TEXT, ds:_DATA, ss:_STACK

main    proc near
        mov     ah, 9           ; Display String
        mov     dx, seg msg
        mov     ds, dx
        mov     dx, offset msg
        int     21h

        mov     ah, 4Ch         ; Return to DOS
        mov     al, 1
        int     21h
main    endp

_TEXT   ends

        end main
