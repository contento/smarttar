all: testdll.dll testdll.lib trydll.exe \
     handler.dll mouse.exe p_api.exe rtestapi.exe \
     realdll.dll realdll.lib testrdll.exe 

testdll.dll: start.asm testdll.c testdll.def
	ml -c start.asm
	cl -ALu -Gs -Lp -c testdll.c
	link /noi start testdll,testdll.dll,,,testdll.def

testdll.lib: testdll.dll testdll.def
	implib /noi testdll.lib testdll.def

trydll.exe: trydll.c testdll.lib
	cl -Lp trydll.c -link testdll.lib
	bind286 trydll

realdll.dll: realdll.c realdll.def
	cl -ALw -FPa -Gs2 -c realdll.c 
	j:\msc.6\bin\link /nod realdll, realdll.dll,,,realdll.def
	markreal realdll.dll

realdll.lib: realdll.dll realdll.def
	implib realdll.lib realdll.def

testrdll.exe: testrdll.c realdll.lib
	cl -Lp testrdll.c -link realdll.lib
	bind286 testrdll
	
handler.dll: handler.asm handler.def
	ml -c -DDOSX286 handler.asm
	j:\msc.6\bin\link handler,handler.dll,,,handler.def
	markreal handler.dll

mouse.exe: mouse.c
	cl -Lp -DDOSX286 -c mouse.c
	link mouse, mouse /noi,, /nod:slibce slibcep,mouse.def
	bind286 mouse

p_api.exe: p_api.c
	cl -Asnu -Gs -Lp p_api.c
	bind286 p_api

rtestapi.exe: rtestapi.c
	cl -Gs2 rtestapi.c
