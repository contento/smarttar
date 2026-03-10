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
#include <string.h>
#include <phapi.h>
#include "testdll.h"

/* these buffers are managed by the DLL */
static unsigned x = 0;
static char buf[80];

#ifndef TC_EXIT
#define TC_EXIT         0
#define TC_HARDERROR    1
#define TC_TRAP         2
#define TC_KILLPROCESS  3
#endif

void far pascal Finish(unsigned reason)
{
    /* do cleanup (put back interrupt vectors, etc.) here */
    
    DosExitList(EXLST_EXIT, NULL);  /* bye! */
}
unsigned far pascal Start(void) /* autoinit entry point */
{
    DosExitList(EXLST_ADD, (PFNEXITLIST)Finish); /* install exit handler */
    return 1;                   /* okay to load DLL */
}

void far pascal put(char far *s, unsigned y)
{
    strcpy(buf, s);
    x = y;
}

unsigned far get_num(void)
{
    return x;
}

char far * far get_str(char far *s)
{
    strcpy(s, buf);
    return s;
}

