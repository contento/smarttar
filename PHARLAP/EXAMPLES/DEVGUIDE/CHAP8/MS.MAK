all: div.exe b800.exe math.exe sqrt.exe

div.exe: div.c
	cl -Lp div.c
	bind286 div

b800.exe: b800.c
	cl -Lp b800.c
	bind286 b800

math.exe: math.c
	cl -Lp math.c
	bind286 math

sqrt.exe: sqrt.c
	cl -Lp sqrt.c
	bind286 sqrt

