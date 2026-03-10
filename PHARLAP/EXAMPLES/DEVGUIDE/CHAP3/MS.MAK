CC=cl
CFLAGS=-AL -DDOSX286 -W3 -Zi
LDFLAGS=-Lp
LIBS=llibpe.lib protmode.def
ASM=ml
ASMFLAGS=-c -DDOSX286 -Cx

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