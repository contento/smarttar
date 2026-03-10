# Makefile for Chapter 2 of the 286|DOS-Extender Developer's Guide
CC=cl
CFLAGS=-DDOSX286 -W4
LDFLAGS=-Lp
LIBS=protmode.def

all: file.exe truename.exe whereami.exe

.c.exe:
	$(CC) $(CFLAGS) $(LDFLAGS) $*.c $(LIBS)
