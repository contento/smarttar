	.286
;
;	Entry point module for trace.dll
;
	extrn	_trace_init:far

_DATA	segment public 'DATA'
	
public __acrtused
__acrtused dw	0

_DATA	ends

 	end	_trace_init


