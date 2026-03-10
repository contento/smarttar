/*
PWINCLIP.C -- 286|DOS-Extender program communication with Windows clipboard
*/

#include <stdlib.h>
#include <malloc.h>
#include <dos.h>
#include <memory.H>
#include <phapi.h>

#include "winclip.h"

/* INT 2Fh old application clipboard functions */
#define WINOLDAP_VERS   0x1700
#define OPEN_CLIP       0x1701
#define EMPTY_CLIP      0x1702
#define SET_CLIP_DATA   0x1703
#define GET_CLIP_SIZE   0x1704
#define GET_CLIP_DATA   0x1705
#define CLOSE_CLIP      0x1708
#define CLIP_COMPACT    0x1709
#define GET_DEV_CAPS    0x170A

#ifndef MK_FP
#define MK_FP(seg,ofs)  ((void far *) \
    (((unsigned long)(seg) << 16) | (ofs)))
#endif

#ifndef MAKELONG        
#define MAKELONG(a, b)  ((long)(((unsigned)(a)) | \
    ((unsigned long)((unsigned)(b))) << 16))
#endif

#define LO(x)           ((x) & 0xFF)
#define HI(x)           ((x) >> 8)

#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

int WinOldApVersion(int *maj, int *min)
{
    REGS16 r;
    memset(&r, 0, sizeof(REGS16));
    r.ax = WINOLDAP_VERS;
    DosRealIntr(0x2F, &r, 0L, 0);
    if (r.ax == WINOLDAP_VERS)      /* didn't change */
        return FALSE;
    else
    {
        *maj = LO(r.ax);
        *min = HI(r.ax);
        return TRUE;
    }
}
    
unsigned GetDeviceCaps(unsigned cap)
{
    REGS16 r;
    memset(&r, 0, sizeof(REGS16));
    r.ax = GET_DEV_CAPS;
    r.dx = cap;
    DosRealIntr(0x2F, &r, 0L, 0);
    return r.ax;
}

void OpenClipboard(void)
{
    REGS16 r;
    memset(&r, 0, sizeof(REGS16));
    r.ax = OPEN_CLIP;
    DosRealIntr(0x2F, &r, 0L, 0);
}

unsigned long GetClipboardSize(CF_FORMAT format)
{
    REGS16 r;
    memset(&r, 0, sizeof(REGS16));
    r.ax = GET_CLIP_SIZE;
    r.dx = format;
    DosRealIntr(0x2F, &r, 0L, 0);
    return MAKELONG(r.ax, r.dx);
}
    
char far *GetClipboardData(CF_FORMAT format)
{
    REGS16 r;
    USHORT seg;
    SEL sel;
    
    if (DosAllocRealSeg(GetClipboardSize(format), &seg, &sel) != 0)
        return (char far *) 0;
    memset(&r, 0, sizeof(REGS16));
    r.ax = GET_CLIP_DATA;
    r.dx = format;
    r.es = seg;         // pass in real-mode address
    r.bx = 0;
    DosRealIntr(0x2F, &r, 0L, 0);
    return (r.ax) ? MK_FP(sel, 0) : 0; // pass back protected-mode address
}

void FreeClipboardData(char far *buf)
{
    DosFreeSeg(FP_SEG(buf));
}

/*
needs:
    SetClipboardData();
    ClipboardCompact();
*/
    
void CloseClipboard(void)
{
    REGS16 r;
    memset(&r, 0, sizeof(REGS16));
    r.ax = CLOSE_CLIP;
    DosRealIntr(0x2F, &r, 0L, 0);
}
    
