/* 
D.C -- Phar Lap extensions to CVP

Microsoft C 6.0+:
    cl -Od -Zi -Gs2 -c -Lp d.c
    cl -Od -Zi -Lp myprog.c d.obj
Microsoft C 5.1: 
    masm -Zi -Ml d_asm.asm;
    cl -Od -Zi -Gs2 -c -Lp d.c
    cl -Od -Zi -Lp myprog.c d.obj d_asm.obj

Can be linked into other programs, but program does not need to call
functions. Instead, invoke functions via the CVP ? command. Note that
functions can be called from CVP ? at any time after function
stack-frame initialization has occurred.

Example:
C:\DOS>cl -Od -Zi -Gs2 myprog.c d.c
C:\DOS>run286 \c600\binp\cvp myprog
> ? segdesc(ds),s
"041D base=807632F4 size=00000FE0 data16"
*/

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <phapi.h>

unsigned loadmod(char far *modname)
{
    unsigned handle;
    return DosLoadModule(NULL, 0, modname, &handle) ? 0 : handle;
}

unsigned modhandle(char far *modname)
{
    unsigned handle;
    return DosGetModHandle(modname, &handle) ? loadmod(modname) : handle;
}

char *modname(unsigned modhand)
{
    static char buf[128];
    return DosGetModName(modhand, 128, buf) ? 0 : buf;
}

void far *procaddr(unsigned modhand, char far *funcname)
{
    void far *f;
    return DosGetProcAddr(modhand, funcname, &f) ? (void far *) 0 : f;
}

void far *getprocaddr(char far *modname, char far *funcname)
{
    unsigned handle = modhandle(modname);
    return handle? procaddr(handle, funcname) : 0;
}

void far *protvec(int intno)
{
    PIHANDLER vec;
    return DosGetProtVec(intno, &vec) ? (void far *) 0 : vec;
}

void far *realvec(int intno)
{
    REALPTR vec;
    return (void far *) (DosGetRealVec(intno, &vec) ? 0 : vec);
}

char *attribname(int attrib)
{
    switch (attrib)
    {
        case CODE16:            return "code16";
        case DATA16:            return "data16";
        case CODE16_NOREAD:     return "code16_noread";
        case DATA16_NOWRITE:    return "data16_nowrite";
        default:                return "attrib??";
    }
}

/* doesn't work for code segments under Windows 3.0 enhanced mode */
char *segdesc(unsigned sel)
{
    static char buf[128], *p=buf;
    DESC d;
    if (DosGetSegDesc(sel, &d)) 
        return "invalid";
    sprintf(p, "%04X base=%08lX size=%08lX %s",
        sel, d.base, d.size, attribname(d.attrib));
    return p;
}

unsigned maprealseg(unsigned rmpara, unsigned long size)
{
    unsigned sel;
    return DosMapRealSeg(rmpara, size, &sel) ? 0 : sel;
}

unsigned biosseg(void)
{
    unsigned bios;
    return DosGetBIOSSeg(&bios) ? 0 : bios;
}

unsigned freeseg(unsigned sel)
{
    return DosFreeSeg(sel) ? 0 : 1;
}

#ifdef _MSC_VER
unsigned lsl(unsigned sel)
{
    _asm sub ax, ax
    _asm lsl ax, sel
}

unsigned lar(unsigned sel)
{
    _asm sub ax, ax
    _asm lar ax, sel
    _asm shr ax, 8
}

unsigned verr(unsigned sel)
{
    _asm mov ax, 1
    _asm verr sel
    _asm je short ok
    _asm dec ax
ok:;
}

unsigned verw(unsigned sel)
{
    _asm mov ax, 1
    _asm verw sel
    _asm je short ok
    _asm dec ax
ok:;
}
#else
/* no inline assembler in MSC 5.1 -- see d_asm.asm */
extern unsigned far _lsl(unsigned sel);
extern unsigned far _lar(unsigned sel);
extern unsigned far _verr(unsigned sel);
extern unsigned far _verw(unsigned sel);

unsigned lsl(unsigned sel)  { return _lsl(sel); }
unsigned lar(unsigned sel)  { return _lar(sel); }
unsigned verr(unsigned sel) { return _verr(sel); }
unsigned verw(unsigned sel) { return _verw(sel); }
#endif

unsigned fp_seg(void far *fp)
{
    return SELECTOROF(fp);
}

unsigned fp_off(void far *fp)
{
    return OFFSETOF(fp);
}

void far *mk_fp(unsigned sel, unsigned ofs)
{
    return MAKEP(sel, ofs);
}

