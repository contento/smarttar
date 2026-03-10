all: exectest.exe dpmitell.exe

exectest.exe: exectest.c
	cl -Lp exectest.c
	bind286 exectest

dpmitell.exe: dpmitell.c iswin3e.c
	cl -Lp -DDOSX286 dpmitell.c iswin3e.c
	bind286 dpmitell
