/*
LASTDRV.C -- uses undocumented DOS to illustrate DosRealIntr()
    in 286|DOS-Extender

cl -Lp lastdrv.c
        
sample output:
    Real-mode List of Lists = 028E:0026
    Protected-mode List Of Lists = 012F:0026
    LASTDRIVE=E
*/

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <phapi.h>

void fail(char *s)  { puts(s); exit(1); }

main()
{
    REGS16 r;
    BYTE far *doslist;
    USHORT sel;
    
    /*
        Get DOS Lists of Lists (INT 21h Function 52h): returns
        pointer to DOS internal variable table in ES:BX
            
        Because this is an undocumented DOS call, not supported
        _transparently_ by 286|DOS-Extender, we must handle it
        ourselves, using DosRealIntr(), and then converted the
        real-mode pointer in ES:BX to a protected-mode address.
    */
    memset(&r, 0, sizeof(REGS16));  /* important: clear to zero! */
    r.ax = 0x5200;
    if (DosRealIntr(0x21, &r, 0L, 0) != 0)
        fail("DosRealIntr call failed");
    doslist = MAKEP(r.es, r.bx);

    if ((doslist == (BYTE far *) 0) || (doslist == (BYTE far *) -1))
        fail("INT 21h AH=52h not supported");
    printf("Real-mode List of Lists = %Fp\n", doslist);

    /*
        Now convert the real-mode pointer to protected mode
    */
    if (DosMapRealSeg(SELECTOROF(doslist), 1024L, &sel) != 0)
        fail("DosMapRealSeg failed");
    doslist = MAKEP(sel, OFFSETOF(doslist));
    printf("Protected-mode List Of Lists = %Fp\n", doslist);

    /*
        Now extract value of LASTDRIVE from protected-mode
    */
    printf("LASTDRIVE=%c\n", 'A' - 1 + 
        doslist[(_osmajor == 3 && _osminor == 0) ? 0x1b : 0x21]);

    /* free selector */
    if (DosFreeSeg(SELECTOROF(doslist)) != 0)
        fail("DosFreeSeg failed");

    return 0;
}
