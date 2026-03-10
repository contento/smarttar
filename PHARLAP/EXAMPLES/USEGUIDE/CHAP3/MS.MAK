all: d.obj hello.exe

d.obj: d.c
	cl -Od -Zi -Gs2 -c -Lp d.c

hello.exe: hello.c d.obj	
	cl -Od -Zi -Lp hello.c d.obj

#
#cvp7 /Cp;p;?segdesc(ds),s hello
#
