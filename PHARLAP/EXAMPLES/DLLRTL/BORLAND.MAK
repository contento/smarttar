FPDMY=..\..\bc3\lib\fpdmy.obj

all: main.exe dll_1.dll dll_2.dll

main.obj: main.c
	bcc286 -c main.c

main.exe: main.obj mainb.def
	bcc286 -M main.obj mainb.def

dll_1.obj: dll_1.c
	bcc286 -c dll_1.c

dll_1.dll: dll_1.obj dll_1b.def
	tlink dll_1+$(FPDMY),dll_1.dll/c/C/n/e/m,dll_1.map,,dll_1b.def;

dll_2.obj: dll_2.c
	bcc286 -c dll_2.c

dll_2.dll: dll_2.obj dll_2b.def
	tlink dll_2+$(FPDMY),dll_2.dll/c/C/n/e/m,dll_2.map,,dll_2b.def;


