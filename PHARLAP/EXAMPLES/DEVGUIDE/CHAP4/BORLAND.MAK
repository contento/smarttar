CC=bcc286
CFLAGS=-v
LDFLAGS=
LIBS=
ASM=tasm
ASMFLAGS=-p -DDOSX286 -mx
LINK=tlink

all: grmem.exe crt.exe cursor.exe bitplane.exe vidbuf.exe vidtest.exe

grmem.exe: grmem.obj fullscrn.obj
	$(CC) $(CFLAGS) $(LDFLAGS) grmem.obj fullscrn.obj $(LIBS)

vidtest.exe: vidtest.obj fullscrn.obj
	$(CC) $(CFLAGS) $(LDFLAGS) vidtest.obj fullscrn.obj $(LIBS)

bitplane.exe: bitplane.asm
	
	$(ASM) -DNEW -DDOSX286 bitplane;
	$(LINK) bitplane,,,phapi,protmode;

.c.exe:
	$(CC) $(CFLAGS) $(LDFLAGS) $*.c $(LIBS)
.c.obj:
	$(CC) -c $(CFLAGS) $*.c
.asm.obj:
	$(ASM) $(ASMFLAGS) $*.asm
