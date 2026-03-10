
all: testdll.dll testdll.lib trydll.exe \
     realdll.dll realdll.lib testrdll.exe p_api.exe rtestapi.exe \
     handler.dll mouse.exe 

testdll.dll: testdll.c testdll.def
	bcc286 -dll testdll.c testdll.def

testdll.lib: testdll.dll testdll.def
	implib testdll.lib testdll.def

trydll.exe: trydll.c testdll.lib
	bcc286 trydll.c testdll.lib

realdll.dll: realdll.c
	bcc -Ij:\bc31\include -c realdll.c
	tlink /nod realdll, realdll.dll,,,realdll.def;
	markreal realdll

realdll.lib: realdll.dll realdll.def
	implib realdll.lib realdll.def

testrdll.exe: testrdll.c
	bcc286 -f testrdll.c realdll.lib

p_api.exe: p_api.c
	bcc286 p_api.c

rtestapi.exe: rtestapi.c
	bcc -Ic:\rel30\inc rtestapi.c

handler.dll: handler.asm handler.def
	tasm -ml -DDOSX286 handler;
	tlink handler,handler.dll,,,handler.def;
	markreal handler

mouse.exe: mouse.c mouse.def
	bcc286 -v mouse.c mouse.def
	bind286 mouse
