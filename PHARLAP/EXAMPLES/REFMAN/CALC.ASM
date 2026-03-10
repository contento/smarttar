; calc.asm -- for hexcalc.c in MSC 5.1
; masm -Ml calc;

        dosseg
        .model small,c
        .code
        public calc
calc    proc op1:word, op2:word
        mov ax, op1
        mov cx, op2
        add ax, cx
        ret
        ; value returned in ax
calc    endp

        end
