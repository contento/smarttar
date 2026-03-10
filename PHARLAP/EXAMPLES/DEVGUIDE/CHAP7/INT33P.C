/*
INT33P.C
Toy protected mode INT 33h (MOUSE) handler
cl -Lp -Ox int33p.c -link slibpe.lib
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <process.h>
#include <dos.h>
#include <phapi.h>

unsigned calls = 0;
unsigned lowmem_sel;
unsigned lowmem_seg;

void interrupt far int33(REGS16 r)
{
    static REGS16 r2;       // ss != ds
    memset(&r2, 0, sizeof(r2));
    calls++;
    r2.ax = r.ax;
    r2.bx = r.bx;
    r2.cx = r.cx;
    r2.dx = r.dx;
    if (r.ax == 9)      // Function 9: set graphics pointer shape
    {                   // takes 64-byte buffer in ES:DX
        movedata(r.es, r.dx, lowmem_sel, 0, 64);
        r2.es = lowmem_seg;
        r2.dx = 0;
    }
    DosRealIntr(0x33, &r2, 0L, 0);
    r.dx = r2.dx;
    r.cx = r2.cx;
    r.bx = r2.bx;
    r.ax = r2.ax;
}

main(int argc, char *argv[])
{
    void (interrupt far *old_int33)();

    if (argc < 2)
        return puts("usage: int33p program [args...]");

    /* just take over protected-mode interrupt */
    DosSetProtVec(0x33, (PIHANDLER)int33, &old_int33);

    /* allocate copy buffer in conventional memory */
    DosAllocRealSeg(64, &lowmem_seg, &lowmem_sel);

    /* spawn program named on commandline */
    printf("%s running %s\n", argv[0], argv[1]);
    spawnvp(P_WAIT, argv[1], &argv[1]);

    /* cleanup */
    printf("%u calls to INT 33h\n", calls);
    DosFreeSeg(lowmem_sel);
    DosSetProtVec(0x33, old_int33, NULL);
    return 0;
}

