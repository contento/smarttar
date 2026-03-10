/*
LINPHYS.C -- Do linear address equal physical?
    Demonstrates DosGetPhysAddr(), DosLockSegPages(),
    and DosUnlockSegPages() in 286|DOS-Extender

cl -Lp linphys.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <phapi.h>

void fail(char *s) { puts(s); exit(1); }

main(int argc, char *argv[])
{
    ULONG phys;
    ULONG count;
    USHORT sel;
    
    fputs("Testing... ", stdout);
    for (sel=0; sel<0xFFFF; sel++)
    {
#if 1       
        USHORT flags;
        if ((DosVerifyAccess(sel, &flags) != 0) ||
            (! (flags & IS_SEL)))
                continue;
#else   
        DESC desc;
        if (DosGetSegDesc(sel, &desc) != 0) // could use DosVerifyAccess
            continue;                       // not a valid selector
#endif              
        if (DosLockSegPages(sel) != 0)      // DosGetPhysAddr needs lock
            continue;                       // can't lock
        if (DosGetPhysAddr(MAKEP(sel, 0), &phys, &count) != 0)
        {   // if DosGetPhysAddr fails, then linear != physical
            puts("linear != physical");
            if (DosUnlockSegPages(sel) != 0)
                fail("DosUnlockSegPages - failure");
            return 0;
        }
        if (DosUnlockSegPages(sel) != 0)
            fail("DosUnlockSegPages - failure");
    }
    /* still here */
    puts("linear == physical");
    return 0;
}

