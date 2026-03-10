;       a.asm
        .286
        .model large
        public _init, _subr, _fini

ifdef DOSX286
ifndef ??version
        includelib PHAPI                ; -link PHAPI.LIB
endif
        extrn DOSCREATEDSALIAS : far
        extrn DOSFREESEG : far
endif

A_DATA  segment word public 'DATA'
dalias  dw  ?           ; temporary alias
A_DATA  ends

A_TEXT  segment word public 'CODE'
        assume CS:A_TEXT

foo     dw  ?           ; the data we're interested in
dsalias   dw  ?         ; the alias for code segment

_init   proc far
ifdef DOSX286
        assume DS:A_DATA
        ; call DosCreateDSAlias(unsigned sel, unsigned far *alias)
        push A_TEXT                 ; PUSH immediate with .286
        push seg dalias
        push offset dalias
        call far ptr DOSCREATEDSALIAS
        or ax, ax
        jnz done                    ; check return value
        push ds
        mov dx, A_DATA
        mov ds, dx                  ; get data segment
        mov dx, dalias
        mov ds, dx                  ; set DS to code seg alias
        assume DS:NOTHING
        mov ds:dsalias, dx          ; move alias into code seg
        pop ds
else
        push ds
        mov ax, A_TEXT              ; for portability, pretend we need
        mov ds, ax                  ; an alias in real mode too
        mov dsalias, ax
        pop ds
        mov ax, 0                   ; always succeeds
endif
done:   not ax                      ; turn around error code in AX
        ret                          
_init   endp

_subr   proc far
        assume DS:A_TEXT
        push ds
        mov ax, cs:dsalias          ; okay to read from CS
        mov ds, ax                  ; put alias for CS into DS
        mov foo, 1234h              ; do the work!
        pop ds
        ret
_subr   endp

_fini   proc far
        assume DS:A_TEXT
ifdef DOSX286
        ; call DosFreeSeg(unsigned sel)
        push dsalias
        call far ptr DOSFREESEG     ; cancel the alias
endif
        ret
_fini   endp

A_TEXT  ends

        end

