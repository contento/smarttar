/* 
WPRINTF.C -- direct screen write version of printf()

Microsoft C 6.0:
    uses inline assembler, based variables
        
Microsoft C 5.1:
    uses int86(), MK_FP macro
        
Conditional compilation:
    _MSC_VER >= 600 built-in for Microsoft C 6
    DOSX286 for 286|DOS-Extender
		
See test program MEM2.C		
*/

#include <stdio.h>
#include <stdarg.h>
#include <dos.h>

#ifdef DOSX286
#include <phapi.h>
#endif

#include "wprintf.h"

typedef unsigned char BYTE;
typedef void far *FP;
typedef unsigned long ULONG;

#ifndef MK_FP
#define MK_FP(seg,ofs)  ((FP)(((ULONG)(seg) << 16) | (ofs)))
#endif

#define VIDEO               0x10
#define SCROLL_UP           0x06
#define GET_VIDEO_MODE      0x0F

#if defined(_MSC_VER) && (_MSC_VER >= 600)
static _segment vid;
#else
static BYTE far *vid_mem;
#endif

#define SCR(y,x)            (((y) * 160) + ((x) << 1))

int video_mode(void)
{
#if defined(_MSC_VER) && (_MSC_VER >= 600)
    _asm mov ah, GET_VIDEO_MODE
    _asm int 10h
    _asm xor ah, ah
#else
    union REGS r;
    r.h.ah = GET_VIDEO_MODE;
    int86(VIDEO, &r, &r);
    return r.h.al;
#endif  
}

unsigned get_vid_mem(void)
{
    unsigned seg;
    int vmode = video_mode();

    if (vmode == 7)
        seg = 0xB000;
    else if ((vmode == 2) || (vmode == 3))
        seg = 0xB800;
    else
        return 0;
	
	/* Should do DESQview alternate buffer call here */

#ifdef DOSX286
{
    unsigned short sel;
    /*
        DosMapRealSeg() takes a real-mode paragraph address and
        a count of bytes, and gives back a selector that can be used
        to address the memory from protected mode. Like all PHAPI
        functions, DosMapRealSeg() returns 0 for success, or a
        non-zero error code. Any other information (such as the
        selector we're interested in) is returned via parameters.
    */
    if (DosMapRealSeg(seg, (long) 25*80*2, &sel) == 0)
        return sel;
    else
        return 0;
}
#else    
    return seg;
#endif
}

int video_init(void)
{
#if defined(_MSC_VER) && (_MSC_VER >= 600)
    vid = get_vid_mem();
#else
    return !! (vid_mem = MK_FP(get_vid_mem(), 0));
#endif
}

void wrt_str(int y, int x, ATTRIB attr, unsigned char *p)
{
#if defined(_MSC_VER) && (_MSC_VER >= 600)
    BYTE _based(vid) *v = (BYTE _based(vid) *) SCR(y, x);
#else
    BYTE far *v = vid_mem + SCR(y, x);
#endif
    while (*p)
    {
        *v++ = *p++;
        *v++ = attr;
    }
}

void wprintf(int y, int x, ATTRIB attr, char *fmt, ...)
{
    static char buf[128] = {0};
    va_list marker;
    va_start(marker, fmt);
    vsprintf(buf, fmt, marker); 
    va_end(marker);
    wrt_str(y, x, attr, buf);
}

void clear(int starty, int startx, int endy, int endx, ATTRIB attr)
{
#if defined(_MSC_VER) && (_MSC_VER >= 600)
    _asm mov ax, 0600h
    _asm mov bh, byte ptr attr
    _asm mov ch, byte ptr starty
    _asm mov cl, byte ptr startx
    _asm mov dh, byte ptr endy
    _asm mov dl, byte ptr endx
    _asm int 10h
#else
    union REGS r;
    r.h.ah = SCROLL_UP;
    r.h.al = 0;
    r.h.bh = attr;
    r.h.ch = starty;
    r.h.cl = startx;
    r.h.dh = endy;
    r.h.dl = endx;
    int86(VIDEO, &r, &r);
#endif  
}

