# Compile and assemble all of the source files of shrseg.dll

all: shrseg.dll

shrseg.obj: shrseg.c
	cl -c -G2 -Gs -Alfu $*.c 

shrasm.obj: shrasm.asm
	ml -c shrasm.asm

# Link shrseg.dll

shrseg.dll: shrseg.obj shrasm.obj
	link shrseg+shrasm,shrseg.dll,/m,/nod:llibce llibcep,shrseg.def;


