/* 
RTESTAPI.C
bcc rtestapi.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <dos.h>

/* PHAPI.H included in real-mode program */
#include <phapi.h>

typedef int (pascal _far *FUNCP)();

#define NORMALIZE(fp)  \
    ((((ULONG) FP_SEG((void far *) (fp))) << 4) + FP_OFF(fp))

void fail(char *s) { puts(s); exit(1); }

PFN procaddr(char far *module, char far *funcname);

unsigned long sel_alloc = 0, sel_free = 0;

main()
{
    DESC desc;
    REALPTR rm;
    PVOID pm, pm1, fp;
    SEL sel, *psel;
    char *str;
    char far *s = "this is a test";
    time_t t1, t2;
    int i;

    /* make sure we are running under P_API */
    if (! (((str = getenv("PROMPT")) != NULL) && strstr(str, "[P_API]")))
        fail("This program requires P_API");

    /* make some simple protected-mode calls from real mode */
    if (DosMapRealSeg(FP_SEG(s), 1024L, &sel) != 0)
        fail("DosMapRealSeg - failure");
    pm1 = MAKEP(sel, FP_OFF(s));
    rm = DosProtToReal(pm1);
    if (NORMALIZE(rm) != NORMALIZE(s))
        fail("rm != s");
    pm = DosRealToProt(rm);
    if (pm != pm1)
        fail("pm != pm1");
    if (DosFreeSeg(sel) != 0)
        fail("DosFreeSeg - failure");

    /* time overhead of intermode call */
    if (DosMapRealSeg(FP_SEG(s), 1024L, &sel) != 0)
        fail("DosMapRealSeg - failure #2");
    printf("Timing intermode calls...");
    time(&t1);
    for (i=30000, fp=MAKEP(sel, FP_OFF(s)); i--; )
        rm = DosProtToReal(fp);
    if (NORMALIZE(rm) != NORMALIZE(s))
        fail("rm != s #2");
    time(&t2);
    if (DosFreeSeg(sel) != 0)
        fail("DosFreeSeg - failure #2");
    printf("\n%u calls per second\n", 30000 / (t2 - t1));

    /* test alloc, realloc, free, descript */
    if (DosAllocSeg(0, &sel, 0) != 0)
        fail("DosAllocSeg fail");
    if (DosGetSegDesc(sel, &desc) != 0)
        fail("DosGetSegDesc fail #1");
    else if (desc.size != 0x10000L)
        fail("seg size wrong #1");
    else
        printf("sel=%04X base=%08lX size=%08lX acc=%04X\n",
            sel, desc.base, desc.size, desc.attrib);
    if (DosReallocSeg(0x1234, sel) != 0)
        fail("DosReallocSeg fail");
    if (DosGetSegDesc(sel, &desc) != 0)
        fail("DosGetSegDesc fail #2");
    else if (desc.size != 0x1234L)
        fail("seg size wrong #2");
    else
        printf("sel=%04X base=%08lX size=%08lX acc=%04X\n",
            sel, desc.base, desc.size, desc.attrib);
    if (DosFreeSeg(sel) != 0)
        fail("DosFreeSeg fail");

    /* allocation loop */
    if (! (psel = calloc(256, sizeof(SEL))))
        fail("insufficient memory");
    for (i=0; ; i++)
        if (DosAllocSeg(0, &psel[i], 0) != 0)
            break;
    printf("Allocated %lu bytes\n", (long) i * 0xFFFFL);
    for ( ; i--; )
        if (DosFreeSeg(psel[i]) != 0)
            fail("DosFreeSeg loop fail");
    free(psel);
    
    printf("allocated %lu selectors\n", sel_alloc);
    if (sel_free != sel_alloc)
        printf("but freed %lu selectors!\n", sel_free);
    else
        puts("ok");
    return 0;
}

PFN procaddr(char far *module, char far *funcname)
{
    printf("procaddr %Fs.%Fs\n", module, funcname);
    {
        _asm	mov ah, 1
        _asm	mov dx, word ptr module+2
        _asm	mov di, word ptr module
        _asm	mov cx, word ptr funcname+2
        _asm	mov si, word ptr funcname
        _asm	int 63h
        _asm	jnc bye
    }
    fail("procaddr fail");
bye:;
}

USHORT APIENTRY DosMapRealSeg(USHORT rm_seg, ULONG ulsize, PSEL selp)
{
    USHORT sel;
    {
        _asm	mov ah, 2
        _asm	mov dx, rm_seg
        _asm	mov cx, word ptr ulsize
        _asm	int 63h
        _asm	jc error
        _asm	mov sel, ax
    }
    *selp = sel;
    sel_alloc++;
    return 0;   // success
error:
    return -1;  // error
}

#define RMODE_PHAPI(f, mod, funcname)       \
    static PFN f = (PFN) 0;                 \
    REGS16 r;                               \
    if (! f)                                \
        f = procaddr(mod, funcname);        \
    memset(&r, 0, sizeof(r));

PVOID APIENTRY DosRealToProt(REALPTR ptr)
{
    RMODE_PHAPI(fDosRealToProt, "PHAPI", "DOSREALTOPROT");
    return (DosVProtFarCall(fDosRealToProt, &r, 0L, -2, 
        (PUSHORT) &ptr)) ? 0 : (PVOID) MAKEP(r.dx, r.ax);
}

REALPTR APIENTRY DosProtToReal(PVOID ptr)
{
    RMODE_PHAPI(fDosProtToReal, "PHAPI", "DOSPROTTOREAL");
    return (DosVProtFarCall(fDosProtToReal, &r, 0L, -2, 
        (PUSHORT) &ptr)) ? 0 : (REALPTR) MAKEP(r.dx, r.ax);
}

USHORT APIENTRY DosAllocSeg(USHORT cbSize, PSEL pSel, USHORT fsAlloc)
{
    USHORT ret, sel;
    RMODE_PHAPI(fDosAllocSeg, "DOSCALLS", "DOSALLOCSEG");
    
    // have to get protected-mode addr for pSel!
    if (DosMapRealSeg(FP_SEG(pSel), sizeof(SEL) + FP_OFF(pSel), &sel) != 0)
        return 0xFFFF;
    pSel = (PSEL) MAKEP(sel, FP_OFF(pSel));
    ret = DosVProtFarCall(fDosAllocSeg, &r, 0L, -4, (PUSHORT) &fsAlloc);
    if ((! ret) && (! r.ax))
        sel_alloc++;
    return (DosFreeSeg(sel) || ret) ? 0xFFFF : r.ax;
}

USHORT APIENTRY DosReallocSeg(USHORT cbNewSize, SEL sel)
{
    RMODE_PHAPI(fDosReallocSeg, "DOSCALLS", "DOSREALLOCSEG");
    return (DosVProtFarCall(fDosReallocSeg, &r, 0L, -2, 
        (PUSHORT) &sel)) ? 0xFFFF : r.ax;
    // note: pass pointer to _last_ argument
}

USHORT APIENTRY DosFreeSeg(SEL sel)
{
    USHORT ret;
    RMODE_PHAPI(fDosFreeSeg, "DOSCALLS", "DOSFREESEG");
    ret = DosVProtFarCall(fDosFreeSeg, &r, 0L, -1, &sel);
    if ((! ret) && (! r.ax))
        sel_free++;
    return (ret) ? 0xFFFF : r.ax;
    // return DosFreeSeg retval, not DosVProtFarCall!
}

USHORT APIENTRY DosGetSegDesc(SEL sel, PDESC descp)
{
    USHORT ret, sel2;
    RMODE_PHAPI(fDosGetSegDesc, "PHAPI", "DOSGETSEGDESC");
    
    if (DosMapRealSeg(FP_SEG(descp), sizeof(DESC) + FP_OFF(descp), 
        &sel2) != 0)
        return 0xFFFF;
    descp = (PDESC) MAKEP(sel2, FP_OFF(descp));
    ret = DosVProtFarCall(fDosGetSegDesc, &r, 0L, -3,
        (PUSHORT) &descp) ? 0xFFFF : r.ax;
    return (DosFreeSeg(sel2) != 0) ? 0xFFFF : ret;
}

USHORT APIENTRY DosVProtIntr(USHORT int_no, PREGS regsp, PVOID reserved,
                 SHORT word_count,  PUSHORT argsp)
{
    static PFN fDosVProtIntr = (PFN) 0;
    if (! fDosVProtIntr)
        fDosVProtIntr = procaddr("PHAPI", "DOSVPROTINTR");
    // is real-mode function pointer
    return (*(FUNCP)fDosVProtIntr)(int_no, regsp, reserved, word_count, argsp);
}

USHORT APIENTRY DosVProtFarCall(PFN funcp, PREGS regsp, PVOID reserved,
                    SHORT word_count, PUSHORT argcp)
{
    static PFN fDosVProtFarCall = (PFN) 0;
    if (! fDosVProtFarCall)
        fDosVProtFarCall = procaddr("PHAPI", "DOSVPROTFARCALL");
    // is real-mode function pointer
    return (*(FUNCP)fDosVProtFarCall)(funcp, regsp, reserved, word_count, argcp);
}

