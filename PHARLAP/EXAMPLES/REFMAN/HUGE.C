/* 
HUGE.C -- allocate two megabytes in one fell swoop

cl -Lp huge.c
run286 huge

sample output:
01BF 01C7 01CF 01D7 01DF ...... 02A7 02AF 02B7
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

void fail(char *s) { puts(s); exit(1); }

#ifndef _MSC_VER
/* _fmemset() not provided in Microsoft C 5.1 - cheap version */
void far * _fmemset(void far *dest, int c, size_t count)
{
    size_t i;
    unsigned cc = MAKEUSHORT(c, c);
    unsigned far *p;
    for (i=count >> 1, p=dest; i--; p++)    // copy by words
        *p = cc;
    if (count % 2)                          // possible last byte
        ((unsigned char far *) dest)[count-1] = c;
    return dest;
}
#endif

main()
{
    USHORT sel, sel2, ret, shift, incr, seg;

    // allocate two-megabyte block (32 64k segments)
    if ((ret = DosAllocHuge(32, 0, &sel, 0, 0)) != 0)
    {
        fail((ret == 8) ?
            "Insufficient memory" : 
            "DosAllocHuge - failure");
    }

    // get jump from one segment number to next
    if (DosGetHugeShift(&shift) != 0)
        fail("DosGetHugeShift - failure");
    incr = 1 << shift;

    // do something with the memory block - set to all x's
    for (seg=0, sel2=sel; seg<32; seg++, sel2 += incr)
    {
        char far *fp = MAKEP(sel2, 0);
        printf("%04X ", sel2);
        _fmemset(fp, 'x', 0xFFFF);  
        fp[0xFFFF] = 'x'; // 65536th byte not accessible with _fmemset
    }
    printf("\n");

    // free the block
    if (DosFreeSeg(sel) != 0)
        fail("DosFreeSeg - failure");
    
    return 0;
}
