/*
REALDLL.C -- sample real-mode DLL for 286|DOS-Extender

cl -ALw -FPa -Gs2 -c realdll.c 
link /nod realdll, realdll.dll,,,realdll.def
implib realdll.lib realdll.def

see TESTRDLL.C:
    cl -Lp testrdll.c -link realdll.lib
    run286 testrdll

see also REALDLL.H, REALDLL.DEF
*/      

#include <dos.h>
#include "realdll.h"

#ifndef _MSC_VER
/* Microsoft C 5.1 */
int _acrtused = 0;
#endif

static unsigned x = 0;
static char buf[80];

void far pascal put(char far *s, unsigned y)
{
    char *p = buf;
    while (*s)
        *p++ = *s++;
    *p = '\0';
    
    x = y;
}

unsigned far get_num(void)
{
    return x;
}

void far get_str(char far *s)
{
    char *p = buf;
    while (*p)
        *s++ = *p++;
    *s = '\0';
}

unsigned long far ticks(void)
{
    unsigned long tick;
    /* note that this code is not possible in protected mode
       (we would need to use DosGetBIOSSeg() or
       DosMapRealSeg()), so we must really be in real mode! */
    _disable();
    tick = *((unsigned long far *) 0x46CL);
    _enable();
    return tick;
}
     
