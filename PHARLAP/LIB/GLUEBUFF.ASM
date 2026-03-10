; $Id: gluebuff.asm 1.1 91/12/10 18:24:13 andreski Exp $

; To increase size of communication buffer, change
; BUFFSIZE equate to the new size, assemble and replace
; in DLL30.LIB. Note: You must run MKGRAPH.BAT again to
; produce another GRAPHICS.DLL with the new buffer size.
;
; To assemble, type:
;
; 	tasm /ml gluebuff.asm
;
; To replace, type:
;
;	tlib dll30.lib -+gluebuff.obj
;

	.286
	.model large
	dosseg

BUFFSIZE equ 4096

	.data

public ___plfifo_buffsize
___plfifo_buffsize dw BUFFSIZE

public ___plfifo
___plfifo db BUFFSIZE dup(0)

	end
