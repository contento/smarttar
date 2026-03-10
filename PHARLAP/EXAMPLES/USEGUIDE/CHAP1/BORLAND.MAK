all: mem1.exe mem2.exe big.exe memtest.exe memtest2.exe dosmem.exe

mem1.exe: mem1.c
	bcc286 mem1.c

mem2.exe: mem2.c
	bcc286 mem2.c wprintf.c 

big.exe: BIG.C
	bcc286 -f- -G -O big.c

dosmem.exe: DOSMEM.C
	bcc286 -f- dosmem.c

memtest.exe: MEMTEST.C
	bcc286 -f- -G -O memtest.c

memtest2.exe: MEMTEST2.CPP
	bcc286 -f- -G -O memtest2.cpp


