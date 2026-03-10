	.286
;
;	Entry point module for int33.dll
;
	extrn	_int33_init:far

_DATA	segment public 'DATA'
	
	public __acrtused
__acrtused dw	0

_DATA	ends

	extrn	_DosProtFarCall:far

	assume	cs:REALSEG

REALSEG	segment public byte 'RMCODE'

k0	dw	0

	public 	_passup_real
_passup_real proc	far

	push	ds 		; Save ES and DS.
	push	es		;

	pushf			; Create a REG16 structure on the
	push	k0		;    stack to call into protected mode
	push	k0		;    with.
	pusha			;
	push	k0		;
	push	k0		;

	mov	ax,sp		; Call the protected mode handler.
	push	k0		;
	push	k0		;
	push	k0		;
	push	ss		;
	push	ax		;
	push	cs:word ptr _passup_protp+2 ;
	push	cs:word ptr _passup_protp   ;
	call	_DosProtFarCall	;
	add	sp,14		;

	add	sp,4		; Restore the saved registers.
	popa			;
	add	sp,4		;
	popf			;

	pop	es		; Restore the real mode DS and ES.
	pop	ds		;

	ret			; Return back to the real mode mouse
				;    driver.

	public	_passup_protp
_passup_protp dd 0

	public 	_real_size
_real_size dw	offset $ + 2

_passup_real endp

REALSEG	ends

 	end	_int33_init


