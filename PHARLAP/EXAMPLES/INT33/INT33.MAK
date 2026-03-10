all: int33.dll

# Compile and assemble all of the source files of int33.dll

int33.obj: int33.c
	cl -c -G2 -Gs -Alfu $(copt) $*.c 

int33asm.obj: int33asm.asm
	ml -c int33asm.asm

# Link int33.dll

int33.dll: int33.obj int33asm.obj
	link int33+int33asm,int33.dll,int33/m,/nod:llibce llibcep,int33.def;

