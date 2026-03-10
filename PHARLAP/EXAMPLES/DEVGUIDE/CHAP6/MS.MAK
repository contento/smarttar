CC=cl
CFLAGS=-AL -DDOSX286 -W3 -Zi 
LDFLAGS=-Lp
LIBS=llibpe.lib protmode.def
ASM=ml
ASMFLAGS=-c -DDOSX286 -Cx
LINK=link

all: twinclip.exe pdev.exe r_api.exe ptestapi.exe 

twinclip.exe: twinclip.c pwinclip.c
	$(CC) $(CFLAGS) $(LDFLAGS) twinclip.c pwinclip.c $(LIBS)

r_api.exe: r_api.c
	cl -Asnu -Gs r_api.c

.c.exe:
	$(CC) $(CFLAGS) $(LDFLAGS) $*.c $(LIBS)
.c.obj:
	$(CC) -c $(CFLAGS) $*.c
.asm.obj:
	$(ASM) $(ASMFLAGS) $*.asm
