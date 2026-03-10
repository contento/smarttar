; crit.asm -- for criterr.c in MSC 5.1
; masm -Ml crit;

            dosseg
            .model small,c
            .code

            public put_str
put_str     proc s:far ptr
            push ds
            mov ah, 9
            lds dx, s
            int 21h
            pop ds
            ret
put_str     endp

            public get_char
get_char    proc
            mov ah, 1
            int 21h
            xor ah, ah
            ret
get_char    endp            
            end
