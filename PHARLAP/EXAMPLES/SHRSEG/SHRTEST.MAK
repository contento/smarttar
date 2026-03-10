all: shrtest1.exe shrtest2.exe

shrtest1.exe: shrtest1.c
	cl -Lp -AL shrtest1.c

shrtest2.exe: shrtest2.c
	cl -Lp -AL shrtest2.c

