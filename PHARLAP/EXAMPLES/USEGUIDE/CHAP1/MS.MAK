all: mem1.exe mem2.exe

mem1.exe: mem1.c
	cl -AL -Lp mem1.c
	bind286 mem1

mem2.exe: mem2.c
	cl -AL -Ox -Gs2 -Lp -DDOSX286 mem2.c wprintf.c 
	bind286 mem2
