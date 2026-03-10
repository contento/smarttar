;   HANDLER.ASM

comment $
Mouse callback routine
See also MOUSE.C
For 286|DOS-Extender, built into real-mode DLL:
    masm -ml -DDOSX286 handler;
    link handler,handler.dll,,,handler.def
HANDLER.DEF:
    LIBRARY HANDLER
    EXETYPE DOS4
    DESCRIPTION 'REAL MODE MOUSE EVENT HANDLER'
    EXPORTS
        _ev_handler
        _full
        _x
        _y
        _fini
        _left
$

    dosseg
    .model small

    PUBLIC  _full
    PUBLIC  _x
    PUBLIC  _y
    PUBLIC  _fini
    PUBLIC  _left
    PUBLIC  _ev_handler

;   shouldn't need this: workaround!
    .data
junk        dw      666

    .code   

;   data inside code segment -- okay for real mode code
_full       dw      1
_x          dw      0ffffh
_y          dw      0ffffh
_fini       dw      0
_left       dw      0

MOUSE_MOVE  EQU     1
LEFT_PRESS  EQU     2

_ev_handler PROC FAR
    cmp _full, 0
    jne fini
    push ds
    mov ax, cs
    mov ds, ax
    mov _x, cx
    mov _y, dx
    test bx, LEFT_PRESS
    je h2
    inc _fini
    jmp SHORT h3
h2:
    test bx, MOUSE_MOVE
    je h3
    inc _left
h3:
    inc _full
    pop ds
fini:
    ret 
_ev_handler ENDP

    end
