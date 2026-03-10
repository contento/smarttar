CC=cl
CFLAGS=-AL -DDOSX286 -W3 -Zi 
LDFLAGS=-Lp
LIBS=llibpe.lib protmode.def
ASM=ml
ASMFLAGS=-c -DDOSX286 -Cx
LINK=link

all: grmem.exe crt.exe cursor.exe bitplane.exe vidbuf.exe vidtest.exe

grmem.exe: grmem.obj fullscrn.obj
	$(CC) $(CFLAGS) $(LDFLAGS) grmem.obj fullscrn.obj $(LIBS)

vidtest.exe: vidtest.obj fullscrn.obj
	$(CC) $(CFLAGS) $(LDFLAGS) vidtest.obj fullscrn.obj $(LIBS)

bitplane.exe: bitplane.asm
	$(ASM) -c -DNEW -DDOSX286 -Zm bitplane.asm
	$(LINK) bitplane,,,phapi,protmode;

.c.exe:
	$(CC) $(CFLAGS) $(LDFLAGS) $*.c $(LIBS)
.c.obj:
	$(CC) -c $(CFLAGS) $*.c
.asm.obj:
	$(ASM) $(ASMFLAGS) $*.asm
