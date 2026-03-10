# Compile all of the source files of trace.exe

all: mem.exe trace.dll

mem.exe: mem.c
	cl -AL -Lp mem.c
	bind286 mem

trace.obj: trace.c
	cl -c -G2 -Gs -Alfu $*.c 

traceasm.obj: traceasm.asm
	ml -c traceasm.asm

trace.dll: trace.obj traceasm.obj
	link trace+traceasm,trace.dll,,/nod:llibce llibcep,trace.def;
