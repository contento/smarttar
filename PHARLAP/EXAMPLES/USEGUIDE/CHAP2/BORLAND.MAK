all: list.exe testdll.dll trydll.exe

list.exe: list.c heapstat.c
	bcc286 list.c heapstat.c

testdll.dll: testdll.c
	bcc286 -dll testdll.c

trydll.exe: trydll.c testdll.lib
	bcc286 trydll.c testdll.lib


