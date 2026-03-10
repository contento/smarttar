/* 
TESTDLL.C -- Sample DLL for 286|DOS-Extender

Microsoft C 5.1:
masm start;
cl -ALu -Gs -Lp -c testdll.c
link /noi/noe start testdll,testdll.dll,,/nod:llibce llibcdll.lib,testdll.def
implib testdll.lib testdll.def
cl -Lp trydll.c -link testdll.lib
run286 trydll

Microsoft C 6.0:
masm start;
cl -ALu -Gs -Lp -c testdll.c
link /noi start testdll,testdll.dll,,llibcdll.lib,testdll.def
implib testdll.lib testdll.dll
cl -Lp trydll.c -link testdll.lib
run286 trydll

Microsoft C/C++ 7.0:
masm start;
cl -ALu -Gs -Lp -c testdll.c
link /noi start testdll,testdll.dll,,,testdll.def
implib testdll.lib testdll.dll
cl -Lp trydll.c -link testdll.lib
run286 trydll
*/



#include <stdlib.h>
#include <stdio.h>
#include <phapi.h>
#include <string.h>
#include "testdll.h"

extern void r286printf(char *, ...);

void main(void){}

/* these buffers are managed by the DLL */
static unsigned x = 0;
static char buf[80];

#ifndef TC_EXIT
#define TC_EXIT         0
#define TC_HARDERROR    1
#define TC_TRAP         2
#define TC_KILLPROCESS  3
#endif

void far pascal Finish(unsigned short reason)
{
    /* do cleanup (put back interrupt vectors, etc.) here */
    
    switch (reason)
    {
        case TC_EXIT:        r286printf("Normal exit\n"); break;
        case TC_HARDERROR:   r286printf("Critical error\n"); break;
        case TC_TRAP:        r286printf("Exception [GP Fault?]\n"); break;
        case TC_KILLPROCESS: r286printf("Process killed\n"); break;
    }
    
    DosExitList(EXLST_EXIT, NULL);  /* bye! */
}
unsigned far pascal Start(void) /* autoinit entry point */
{
    DosExitList(EXLST_ADD, Finish); /* install exit handler */
    return 1;                   /* okay to load DLL */
}

void P_EXPORT put(char far *s, unsigned y)
{
    r286printf("DLL put() called s=\"%Fs\" y=%u\n", s, y);
    strcpy(buf, s);
    x = y;
}

unsigned C_EXPORT get_num(void)
{
    r286printf("DLL get_num() returning %u\n", x);
    return x;
}

char far * C_EXPORT get_str(char far *s)
{
    r286printf("DLL get_str() returning \"%Fs\"\n", buf);
    strcpy(s, buf);
    return s;
}

