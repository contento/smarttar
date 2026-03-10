CC=bcc286
CC=bcc286
CFLAGS=
LDFLAGS=
LIBS=

all: file.exe truename.exe whereami.exe

.c.exe:
	$(CC) $(CFLAGS) $(LDFLAGS) $*.c $(LIBS)
