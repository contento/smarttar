/*
REALVEC.C -- demonstrates real-mode interrupt vectors
    in 286|DOS-Extender
        
cl -Lp realvec.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <dos.h>
#include <phapi.h>

#define IRET        0xCF

void fail(char *s)  { puts(s); exit(1); }

main(int argc, char *argv[])
{
    REGS16 r;
    void (interrupt far *rhandler)();
    SEL sel;
    int intno;
    
    if (argc < 2)
        fail("usage: realvec [hex intno]");
    else
        sscanf(argv[1], "%02X", &intno);

    if (DosGetRealVec(intno, (REALPTR *) &rhandler) != 0)
        fail("DosGetRealVec - failure");

    /*  show that DosGetRealVec is equivalent to
        real-mode INT 21h Function 35h (DOS Get Vector)
        by calling INT 21h Function 35h from real mode */
    memset(&r, 0, sizeof(REGS16));  /* important: clear to zero! */
    r.ax = 0x3500 + intno;
    if (DosRealIntr(0x21, &r, 0L, 0) != 0)
        fail("DosRealIntr - failure");
    else
        assert(MAKEP(r.es, r.bx) == (PVOID) rhandler);
    
    if (! rhandler)
        printf("INT %02Xh - no real-mode handler\n", intno);
    else
    {
        BYTE far *code = (BYTE far *) rhandler;
        /* if we tried to dereference this pointer, it would
           cause a GP fault: it's a real-mode pointer */
        
        /* Since the returned pointer is a real-mode address,
           if we want to examine the code itself, we must first
           map the address into our protected-mode address
           space, then create a protected-mode pointer to the
           real-mode code */
        if (DosMapRealSeg(FP_SEG(rhandler), 0xFFFF, &sel) != 0)
            fail("DosMapRealSeg - failure");
        code = MAKEP(sel, FP_OFF(rhandler));
        
        /* now we can dereference the pointer... */
        if ((*code == IRET) ||                
            (*((ULONG far *) code) == 0L) ||  /* first 4 bytes are 0 */
            (*((ULONG far *) code) == -1L))   /* first 4 bytes are FFh */
            printf("INT %02Xh - no real-mode handler\n", intno);
        else
            printf("INT %02Xh RealVec = %Fp\n", intno, rhandler);

        /* of course we free the selector when done with it... */
        if (DosFreeSeg(sel) != 0)
            fail("DosFreeSeg - failure");
    }

    return 0;
}
