/*
PTESTAPI.C -- calls stack-based rmode API in R_API.C
Improved version
cl -Lp ptestapi.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <time.h>
#include <string.h>
#include <phapi.h>

#pragma pack(1)

typedef struct {
    unsigned x, y, z;
    } PARAM_BLOCK;
    
void fail(char *s) { puts(s); exit(1); }

unsigned far entry(unsigned handle, PARAM_BLOCK far *pb)
{
    static unsigned (far *entry_func)(unsigned handle, PARAM_BLOCK far *pb);
    static int first_time = 1;
    static PARAM_BLOCK far *pp, far *pr;
    REGS16 r;

    if (first_time) /* one-time initialization */
    {
        /* allocate the low-memory copy buffer */
        char *s;
        unsigned short seg, sel;
        if (DosAllocRealSeg(sizeof(PARAM_BLOCK), &seg, &sel) != 0)
            fail("DosAllocRealSeg fail");
        pp = MAKEP(sel, 0);
        pr = MAKEP(seg, 0);
        
        if (((s = getenv("PROMPT")) != NULL) && (! strstr(s, "[R_API]")))
            fail("This program requires R_API");

        /* get function pointer to rmode entry() */
        memset(&r, 0, sizeof(r));
        r.ax = 0x00;
        if (DosRealIntr(0x63, &r, 0L, 0) != 0)
            fail("DosRealIntr fail");
        if (r.flags & 1)
            fail("INT 63h AH=0 failed");
        entry_func = MAKEP(r.es, r.bx);

        first_time--;
    }
    
    memset(&r, 0, sizeof(r));
    
    /* structure copy, using pmode ptr to conventional memory... */
    *pp = *pb;

    /* ... and use rmode equivalent to pass down to rmode function */
    if (DosRealFarCall((REALPTR)entry_func, &r, 0L, 3, handle, pr) != 0)
        fail("DosRealFarCall fail");
    return r.ax;
}

main()
{
    PARAM_BLOCK pb;
    int i, j, k;
    unsigned long iter;
    time_t t1, t2;
    
    fputs("Testing R_API/PTESTAPI communication... ", stdout);
    
    time(&t1);
    for (iter=0;;)
        for (i=10; i--; )
            for (j=11; j--; )
                for (k=12; k--; )
                {
                    pb.x = i;
                    pb.y = j;
                    pb.z = k;
                    if (entry(0, &pb) != (unsigned)(i * j * k))
                        fail("call to entry failed");
                    if (time(&t2) - t1 > 5)
                        goto fini;
                    iter++;
                }
fini:
    printf("%lu intermode calls/second\n", iter/5L);
    return 0;
}

