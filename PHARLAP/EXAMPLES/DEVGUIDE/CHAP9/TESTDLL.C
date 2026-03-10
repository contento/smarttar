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

#ifdef _MSC_VER
#if _MSC_VER >= 700
/*
 * Calling most of the C run-time library is not yet
 * supported in a DLL under MSC/C++ 7.0.  For this
 * reason, we must declare "main" here, link against
 * the regular library (not llibcdll.lib), and
 * avoid calling printf/puts.
 */
void main(void){}
#define	puts(x) /* nothing */
#endif
#endif

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
    
    switch (reason)
    {
        case TC_EXIT:        puts("Normal exit"); break;
        case TC_HARDERROR:   puts("Critical error"); break;
        case TC_TRAP:        puts("Exception [GP Fault?]"); break;
        case TC_KILLPROCESS: puts("Process killed"); break;
    }
    
    DosExitList(EXLST_EXIT, NULL);  /* bye! */
}
unsigned far pascal Start(void) /* autoinit entry point */
{
    DosExitList(EXLST_ADD, (PFNEXITLIST)Finish); /* install exit handler */
    return 1;                   /* okay to load DLL */
}

#if 1
void P_EXPORT put(char far *s, unsigned y)
#else
void far pascal put(char far *s, unsigned y)
#endif
{
    strcpy(buf, s);
    x = y;
}

#if 1
unsigned C_EXPORT get_num(void)
#else
unsigned far get_num(void)
#endif
{
    return x;
}

#if 1
char far * C_EXPORT get_str(char far *s)
#else
char far * far get_str(char far *s)
#endif
{
    strcpy(s, buf);
    return s;
}

