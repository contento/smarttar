;       hello.asm 
;       Produces LINK warning: no automatic data segment
;       but still runs in 286|DOS-Extender

;	Note that C7 LINK warning 4036 produces invalid an
;	EXE file, so this file no longer produces that warning.

;       masm hello;
;       link hello,,,,protmode
;       run286 hello

        .286

stdout  equ     1	; corrected from 286dosx Developer's Guide, p. 56

cr      equ     0dh
lf      equ     0ah

_DATA   segment word public 'DATA'
msg     db      cr, lf
        db      'Hello world!', cr, lf
msg_len equ     $-msg

wlen    dw      0
_DATA   ends

STACK   segment para stack 'STACK'
        db      128 dup (?)
STACK   ends

_TEXT   segment word public 'CODE'
        assume CS:_TEXT, DS:_DATA, SS:STACK

print   proc    far

        mov     ax, _DATA
        mov     ds, ax

        mov     ah, 40h
        mov     bx, stdout
        mov     cx, msg_len
        mov     dx, offset msg
        int     21h

        mov     ax, 4c00h
        int     21h
print   endp

_TEXT   ends

;;
;; A bug in certain MS linkers (including the one that comes with
;; MS C/C++ 7.0) requires that all program have DGROUP's.
dgroups segment
dgroup group dgroups
	dw	?
dgroups ends

        end     print

