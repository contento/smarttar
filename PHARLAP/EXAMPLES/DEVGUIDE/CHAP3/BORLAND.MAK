CC=bcc286
CC=bcc286
CFLAGS=
LDFLAGS=
LIBS=
ASM=tasm
ASMFLAGS=-p -DDOSX286 -mx

all: desc.exe huge.exe c.exe

c.exe: c.obj a.obj
	$(CC) $(CFLAGS) $(LDFLAGS) c.obj a.obj $(LIBS)

.c.exe:
	$(CC) $(CFLAGS) $(LDFLAGS) $*.c $(LIBS)
.c.obj:
	$(CC) -c $(CFLAGS) $*.c
.asm.obj:
	$(ASM) $(ASMFLAGS) $*.asm
