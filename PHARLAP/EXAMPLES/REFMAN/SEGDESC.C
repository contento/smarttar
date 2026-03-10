/*
SEGDESC.C -- illustrates DosGetSegDesc() in 286|DOS-Extender

cl -Lp segdesc.c

usage:  segdesc [- or selector number]

segdesc with - on command line displays entire memory map
segdesc with selector on command line displays that selector (if valid)
segdesc with no command-line arguments displays current segregs:
    CS  039F  size=001ED2  base=0018D400  CODE
    DS  03B7  size=002000  base=00112C00  DATA
    ES  03B7  size=002000  base=00112C00  DATA
    SS  03B7  size=002000  base=00112C00  DATA
        
under Microsoft Windows 32-bit DPMI, note very large base addresses:
    CS  01CD  size=001EB2  base=806B5978  CODE
    DS  01E5  size=002000  base=806C51E8  DATA
    ES  01E5  size=002000  base=806C51E8  DATA
    SS  01E5  size=002000  base=806C51E8  DATA
*/


#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <phapi.h>

#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

USHORT dummy;

void fail(char *s) { puts(s); exit(1); }

BOOL display_seg(char *name, USHORT reg)
{
    DESC desc;
    if (DosGetSegDesc(reg, &desc) != 0)
        return FALSE;   // not a valid selector
    if (*name)
        printf("%s  ", name);
    printf("%04X  size=%06lX  base=%08lX  ",
        reg, desc.size, desc.base);
    switch (desc.attrib)
    {
        case CODE16:         puts("CODE"); break;
        case DATA16:         puts("DATA"); break;
        case CODE16_NOREAD:  puts("EXEC-ONLY CODE"); break;
        case DATA16_NOWRITE: puts("READ-ONLY DATA"); break;
        default:             fail("DosGetSegDesc error!");
    }
    return TRUE;
}

main(int argc, char *argv[])
{
    if (argc < 2)
    {
        /*
            If no command-line arguments, just print out descriptors
            corresponding to program's current segment registers. Use
            Microsoft C segread() function to get segregs.
        */
        struct SREGS sregs;
        segread(&sregs);

        display_seg("CS", sregs.cs);
        display_seg("DS", sregs.ds);
        display_seg("ES", sregs.es);
        display_seg("SS", sregs.ss);
    }
    else if (argv[1][0] == '-')
    {
        /*
            If dash on command line, print out entire memory map,
            by looping over all possible selector values, and printing
            out those which are valid, as indicated by the return
            value from DosGetSegDesc() (tested in display_seg).
        */
        USHORT sel;
        for (sel=0; sel<0xFFFF; sel++)
            display_seg("", sel);
    }
    else
    {
        /* Print descriptor for selector number given on command line */
        USHORT sel;
        sscanf(argv[1], "%04X", &sel);
        if (! display_seg("", sel))
            printf("%04X not a valid selector\n", sel);
    }

    return 0;
}
