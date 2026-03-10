#
# Make file to build the real mode DLL example.
#

all: rmdlllib.dll mainapp.exe

# Building the real mode library

real.lib: real.c
	bcc -c -ml -N- -I\rel30\inc real.c
	del real.lib
	tlib real.lib +real.obj

# Build the real mode dll

rmdlllib.dll: rmdlllib.c real.lib
	bcc -ml! -f -N- -Oi -c -I\rel30\inc rmdlllib.c
	bcc286 -realdll rmdlllib.obj real.lib

# Build the import library for the real mode DLL

rmdlllib.lib: rmdlllib.dll
	implib rmdlllib.lib rmdlllib.dll

pmlibdll.obj: pmlibdll.c
	bcc -c -ml -N- -I\rel30\inc pmlibdll.c

# Build the main protected mode application

mainapp.exe: mainapp.c pmlibdll.obj rmdlllib.lib mainapp.def
	bcc -c -ml -N- -I\rel30\inc mainapp.c
	bcc286 mainapp.obj pmlibdll.obj rmdlllib.lib mainapp.def
