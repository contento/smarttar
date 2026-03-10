all:  rmdlllib.dll mainapp.exe

real.lib:	real.obj
	del real.lib
	lib real.lib +real.obj;

real.obj:	real.c
	cl -c -AL -Gs real.c

rmdlllib.lib:	rmdlllib.dll
	implib rmdlllib.lib rmdlllib.dll

rmdlllib.dll:	rmdlllib.obj real.lib rmdlllib.def
	link /nod rmdlllib.obj,rmdlllib.dll,,real.lib phapi.lib llibca.lib,rmdlllib.def;
	markreal rmdlllib.dll

rmdlllib.obj:	rmdlllib.c
	cl -ALu -FPa -Gs2 -Oi -c rmdlllib.c

mainapp.exe:	mainapp.obj pmlibdll.obj rmdlllib.lib mainapp.def
	link mainapp.obj pmlibdll.obj/co/noe,mainapp.exe,,rmdlllib.lib phapi.lib llibcep.lib/nod,mainapp.def;

pmlibdll.obj:	pmlibdll.c
	cl -Zi -c -AL -Gs -FPi pmlibdll.c

mainapp.obj:	mainapp.c
	cl -Zi -c -AL -Gs -FPi mainapp.c
