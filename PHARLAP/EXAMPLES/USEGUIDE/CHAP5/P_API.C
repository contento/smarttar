/* 
P_API.C -- see also RTESTAPI.C
cl -Asnu -Gs -Lp p_api.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <phapi.h>

#ifndef MK_FP
#define MK_FP(seg,ofs)  ((char far *) \
    (((unsigned long)(seg) << 16) | (ofs)))
#endif

/* procaddr() accepts real-mode pointer to ASCIIZ module name,
   real-mode pointer to ASCIIZ function name, and returns function
   pointer. Mode of the function pointer depends on the function */
PFN procaddr(char far *mod, char far *func)
{
    HMODULE h;
    SEL sel;
    PFN f = 0;
    if (DosMapRealSeg(FP_SEG(mod), 1024, &sel) != 0)     
        return 0;
    // see if this module is already loaded
    if (DosGetModHandle(MK_FP(sel, FP_OFF(mod)), &h) != 0)
    {
        // not already loaded: load it now
        if (DosLoadModule(0, 0, MK_FP(sel, FP_OFF(mod)), &h) != 0)   
        {
            // couldn't load
            DosFreeSeg(sel); 
            return 0;
        }
    }
    // now have module handle
    if ((DosFreeSeg(sel) != 0) ||
        (DosMapRealSeg(FP_SEG(func), 1024, &sel) != 0))
        return 0;
    if (DosGetProcAddr(h, MK_FP(sel, FP_OFF(func)), &f) != 0)     
        f = (PFN) 0;
    DosFreeSeg(sel); // must free even if DosGetProcAddr failed
    return f;
}

#ifdef _MSC_VER
void interrupt far api(REGS16 r)    // our INT 63h handler
#else
void interrupt far api(REGS_BINT r)    // our INT 63h handler
#endif
{
    unsigned ah = (r.ax & 0xFF00) >> 8;
    PFN f;
    r.flags |= 1;   // assume error: set carry flag
    switch (ah)
    {
        case 0x01:  // PROCADDR function
            // DX:DI -> module name, CX:SI -> function name
            if (f = procaddr(MK_FP(r.dx, r.di), MK_FP(r.cx, r.si)))
            {
                r.dx = FP_SEG(f);
                r.ax = FP_OFF(f);
                r.flags &= ~1;  // clear carry
                // on return, DX:AX -> function pointer
            }
            break;
        case 0x02:
            // DX = real seg
            // CX = count of bytes
            if (DosMapRealSeg(r.dx, (ULONG) r.cx, &r.ax))
                r.ax = 0;
            else
                r.flags &= ~1;  // clear carry
            // on return, AX = selector or zero
            break;
    }
}

void fail(char *s) { puts(s); exit(1); }

main()
{
    PIHANDLER old_63_prot;
    REALPTR old_63_real;
    
    if (DosSetPassToProtVec(0x63, (PIHANDLER)api, &old_63_prot, &old_63_real) != 0)
        fail("DosSetPasstoProtVec fail");
    putenv("PROMPT=[P_API] $p$g"); 
    system(getenv("COMSPEC"));
    if (DosSetRealProtVec(0x63, old_63_prot, old_63_real, NULL, NULL) != 0)
        fail("DosSetRealProtVec fail");
    puts("bye");
    return 0;
}

