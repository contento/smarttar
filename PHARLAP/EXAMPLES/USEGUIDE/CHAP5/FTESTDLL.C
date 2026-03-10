/* 
TESTDLL.C -- Sample DLL for 286|DOS-Extender
*/

#include <stdlib.h>
#include <stdio.h>
#include <phapi.h>
#include <math.h>
#include "ftestdll.h"

void main(void){}

/* these buffers are managed by the DLL */
static char buf[80];

static double x;
double z;

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
    return 1;                   /* okay to load DLL */
}

void P_EXPORT put(double y)
{
    x = y;
}

/* We must not return a double here, for fear of tickling a bug
 * in the Microsoft C/C++ 7.00 compiler.
 */
void C_EXPORT get_half(double *xp)
{
    z = x /2.0; 
    *xp = z;
}
