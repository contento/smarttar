/* 
VIDBUF.C
cl -Lp -DDOSX286 vidbuf.c
bcc286 vidbuf.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>

#ifdef DOSX286
#include <phapi.h>
#endif

char far *vidbuf(unsigned short vidseg)
{
#ifdef DOSX286
    REGS16 r;
    SEL sel;
    memset(&r, 0, sizeof r);
    r.es = vidseg;  // ES:DI -> assumed video buffer
    r.di = 0;
    r.ax = 0xfe00;  // AH=FEh
    if (DosRealIntr(0x10, &r, 0, 0) != 0)  // INT 10h
        return 0;
    // ES:DI now -> real-mode address of actual video buffer 
    if (DosMapRealSeg(r.es, (long) r.di + 25*80*2, &sel) != 0) 
        return 0;
    return MAKEP(sel, r.di);    
#else
    union REGS r;
    struct SREGS s;
    char far *vid;
    segread(&s);
    s.es = vidseg;
    r.x.di = 0;
    r.h.ah = 0xfe;
    int86x(0x10, &r, &r, &s);
    FP_SEG(vid) = s.es;
    FP_OFF(vid) = r.x.di;
    return vid;
#endif      
}

main()
{
    char far *screen = vidbuf(0xb800);
    char far *fp;
    int i, j;
    for (j=0, fp=screen; j<25; j++) // fill the screen with 'x'
        for (i=0; i<160; i+=2, fp+=2)
            *fp = 'x';
    return 0;
}

