/*
ENUMMOD.C -- enumerate all loaded modules in 286|DOS-Extender

Loops over all possible module handles (0..FFFFh), printing out
module handle and name whenever DosGetModName() succeeds. If
DosGetModName() fails, we just continue.

cl -Lp enummod.c

sample output:
    327 C:\WILBUR\DOC\NEW\enummod.EXE
    351 DOSCALLS
    383 WILSYS
    415 VIOCALLS
*/

#include <stdio.h>
#include <phapi.h>

main()
{
    char buf[128];
    register USHORT i;
    for (i=0; i<0xFFFF; i++)
        if (DosGetModName(i, 128, buf) == 0)
            printf("%u\t%s\n", i, buf);
        else if (i && (! (i % 0x1000)))
            putchar('.');   // visual indication that we're still looking...
}
