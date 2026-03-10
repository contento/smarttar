
all: testdll.dll testdll.lib trydll.exe memtrace.dll

testdll.dll: testdll.c start.asm
	ml -c start.asm
	cl -ALu -Gs -Lp -c testdll.c
	link /noi start testdll,testdll.dll,,,testdll.def

testdll.lib: testdll.dll
	implib /noi testdll.lib testdll.def

trydll.exe: trydll.c testdll.lib
	cl -Lp trydll.c -link testdll.lib
	bind286 trydll

#For C7, crtlib.dll is not provided
#cl -c -AL -Gs2 -Zpe -Lp -D_MT callfunc.c
#link c:\c600\lib\crtexe callfunc,callfunc,,/nod crtlib.lib os2.lib;

memtrace.dll: memtrace.c start.asm
	ml -c start.asm
	cl -ALu -Lp -Gs2 -c memtrace.c
	link memtrace start,memtrace.dll,,/nod:llibce llibcep phapi,memtrace.def
