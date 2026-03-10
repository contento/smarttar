/* 
CRT.C
cl -Lp -DDOSX286 crt.c
bcc286 crt.c
*/

#include <stdlib.h>
#include <stdio.h>

#ifdef DOSX286
#include <phapi.h>
#endif

#ifndef MK_FP
#define MK_FP(seg,ofs) \
    ((void far *)(((unsigned long)(seg) << 16) | (ofs)))
#endif
        
main()
{
    unsigned far *fp;
    unsigned crt_base;
    
#ifdef DOSX286
    SEL bios;
    DosGetBIOSSeg(&bios);
    fp = (unsigned far *) MK_FP(bios, 0x63);
#else
    /* the following are all equivalent */
    fp = (unsigned far *) 0x463L;            /* 0000:0463 */
    fp = (unsigned far *) 0x00400063L;       /* 0040:0063 */
    fp = (unsigned far *) MK_FP(0, 0x463);   /* 0000:0463 */
    fp = (unsigned far *) MK_FP(0x40, 0x63); /* 0040:0063 */
#endif  
    
    crt_base = *fp;
    printf("CRT controller base = %04X\n", crt_base);
    return 0;
}

