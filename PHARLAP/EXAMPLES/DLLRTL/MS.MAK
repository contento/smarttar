#
# RULES FOR MICROSOFT VISUAL C++
#

all: main.exe dll_1.dll dll_2.dll

main.exe: main.c main.def
	cl -Fm -ALw -G2 -Lp main.c main.def
	bind286 main

dll_1.obj: dll_1.c
	cl -c -ALw -Gs2 -Lp dll_1.c

dll_1.dll: dll_1.obj dll_1.def
	link /NOI /M /NOE /NOD dll_1.obj,dll_1.dll,dll_1.map,,dll_1.def

dll_2.obj: dll_2.c
	cl -c -ALw -Gs -Lp dll_2.c

dll_2.dll: dll_2.obj dll_2.def
	link /NOI /M /NOD /NOE dll_2.obj,dll_2.dll,dll_2.map,,dll_2.def


