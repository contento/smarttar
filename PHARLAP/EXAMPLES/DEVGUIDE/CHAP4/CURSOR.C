/* 
CURSOR.C
cl -Lp -DDOSX286 -Oi cursor.c
bcc286 -Oi cursor.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>      /* inp, outp */
#include <dos.h>        /* _disable, _enable */

#ifdef DOSX286
#include <phapi.h>
#endif

#ifndef MK_FP
#define MK_FP(seg,ofs) \
    ((void far *)(((unsigned long)(seg) << 16) | (ofs)))
#endif
        
main()
{
    unsigned crt_base;
    unsigned cursaddr_h, cursaddr_l, cursaddr;
    unsigned columns;
    unsigned x, y;
    unsigned short bios;
    
#ifdef DOSX286
    DosGetBIOSSeg(&bios);
#else
    bios = 0x40;
#endif

#define CRT_COLS        0x4A
#define ADDR_6845       0x63

    crt_base = *((unsigned far *) MK_FP(bios, ADDR_6845));
    columns = *((unsigned far *) MK_FP(bios, CRT_COLS));
    
#define INDEX_REG       crt_base
#define DATA_REG        crt_base+1

#define CURSADDR_H      0x0E
#define CURSADDR_L      0x0F

    _disable();         /* CLI */
    outp(INDEX_REG, CURSADDR_H);
    cursaddr_h = inp(DATA_REG);
    
    outp(INDEX_REG, CURSADDR_L);
    cursaddr_l = inp(DATA_REG);
    _enable();          /* STI */
    
#define MAKEWORD(l,h)   (((unsigned)(l)) | ((unsigned)(h)) << 8)

    cursaddr = MAKEWORD(cursaddr_l, cursaddr_h);

    y = (cursaddr / columns);
    x = (cursaddr % columns);

    printf("x=%u y=%u\n", x, y);
    return 0;
}

