all: memtrace.dll testdll.dll trydll.exe 

testdll.dll: testdll.c 
	bcc286 -dll testdll.c testdll.def

testdll.lib: testdll.def
	implib testdll.lib testdll.def

trydll.exe: trydll.c testdll.lib
	bcc286 trydll.c testdll.lib

memtrace.dll: memtrace.c
	bcc286 -dll -r- -start=START memtrace.c memtrace.def
