; d_asm.asm -- Phar Lap CVP extensions for use with MSC 5.1
; masm -Ml -Zi d_asm.asm;
; see d.c

        .286p
        dosseg
        .model small
        .code
        
;       unsigned lsl(unsigned sel)
        public __lsl
__lsl   proc far
        enter 0,0
        sub ax, ax
        lsl ax, word ptr [bp+6]
        leave
        ret
__lsl   endp

;       unsigned lar(unsigned sel)
        public __lar
__lar   proc far
        enter 0,0
        sub ax, ax
        lar ax, word ptr [bp+6]
        shr ax, 8
        leave
        ret
__lar   endp

;       unsigned verr(unsigned sel)
        public __verr
__verr  proc far
        enter 0,0
        mov ax, 1
        verr word ptr [bp+6]
        je short r_ok
        dec ax
r_ok:   leave
        ret
__verr  endp

;       unsigned verw(unsigned sel)
        public __verw
__verw  proc far
        enter 0,0
        mov ax, 1
        verw word ptr [bp+6]
        je short w_ok
        dec ax
w_ok:   leave
        ret
__verw  endp

        end
