	.286
;
;	Entry point module for shrseg.dll
;
	extrn	_shrseg_init:far

_DATA	segment public 'DATA'
	
public __acrtused
__acrtused dw	0

_DATA	ends

 	end	_shrseg_init


