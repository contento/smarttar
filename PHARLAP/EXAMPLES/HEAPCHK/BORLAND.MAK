all: heap_chk.obj heaptest.exe

heap_chk.obj: heap_chk.c
	bcc286 -c heap_chk.c

heaptest.exe: heaptest.c heap_chk.obj
	bcc286 heaptest.c heap_chk.obj

