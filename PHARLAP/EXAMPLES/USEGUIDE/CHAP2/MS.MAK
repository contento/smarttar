all: list.exe testdll.dll testdll.lib trydll.exe

list.exe: list.c msheapst.c
	cl -AL -Lp list.c msheapst.c
	bind286 list

hello.exe: hello.asm
	ml -c hello.asm
	link hello,,,,protmode;
	bind286 hello

hello2.exe: hello2.asm
	ml -c hello2.asm
	link hello2,,,,protmode
	bind286 hello2

testdll.dll: testdll.c start.asm
	ml -c start.asm
	cl -ALu -Gs -Lp -c testdll.c
	link /noi start testdll,testdll.dll,,,testdll.def

testdll.lib: testdll.dll testdll.def
	implib testdll.lib testdll.def

trydll.exe: trydll.c
	cl -Lp trydll.c -link testdll.lib
	bind286 trydll

hello3.exe: hello3.asm
	ml -c hello3.asm
	link /stack:1024 hello3,,,phapi,protmode
	bind286 hello3
