/* 
FULLSCRN.C -- package of routines for direct screen writes

Borland C:
    uses inline assembler

Microsoft C 6.0:
    uses inline assembler, based variables
        
Microsoft C 5.1:
    uses int86()
        
Conditional compilation:
    _MSC_VER >= 600 built-in for Microsoft C 6
    DOSX386 for Phar Lap 386|DOS-Extender and Watcom C/386 
        
DOSX286 switch not needed for FAPI Microsoft executables, because examining
_osmode at run-time        
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <dos.h>

#ifdef DOSX386
#define int86(x,y,z)        int386((x),(y),(z))
#define MAKEP(sel,ofs)      MK_FP(0x34, ((sel) << 4) + (ofs))
#define cx                  ecx
#else
#include <phapi.h>
#endif

#include "fullscrn.h"

typedef unsigned char BYTE;
typedef void far *FP;
typedef unsigned long ULONG;

#define VIDEO               0x10
#define SCROLL_UP           0x06
#define GET_VIDEO_MODE      0x0F

#if defined(_MSC_VER) && (_MSC_VER >= 600)
static _segment vid;
#else
static BYTE far *vid_mem;
#endif

#ifdef __BORLANDC__  /* Borland does not have _osmode */
int _osmode =
#ifdef DOSX286
    PROTECTED_MODE;
#else
    REAL_MODE;
#endif
#endif

#define SCR(y,x)            (((y) * 160) + ((x) << 1))

int video_mode(void)
{
#if (defined(_MSC_VER) && (_MSC_VER >= 600)) || defined(__BORLANDC__)
    int mode;
    _asm mov ah, GET_VIDEO_MODE
    _asm int 10h
    _asm xor ah, ah
    _asm mov mode,ax
    return mode;
#else
    union REGS r;
    r.h.ah = GET_VIDEO_MODE;
    int86(VIDEO, &r, &r);
    return r.h.al;
#endif  
}

unsigned get_vid_mem(void)
{
    int vmode = video_mode();
    unsigned short vid_seg;

    if (vmode == 7)
        vid_seg = 0xB000;
    else if ((vmode == 2) || (vmode == 3))
        vid_seg = 0xB800;
    else
        return 0;

#ifdef DOSX386  
    return vid_seg;
#else
    if (_osmode == PROTECTED_MODE)
    {
        SEL sel;
        /*
            DosMapRealSeg() takes a real-mode paragraph address and
            a count of bytes, and gives back a selector that can be used
            to address the memory from protected mode. Like all PHAPI
            functions, DosMapRealSeg() returns 0 for success, or a
            non-zero error code. Any returned information (such as the
            selector we're interested in) is returned via parameters.
        */
        if (DosMapRealSeg(vid_seg, (long) 25*80*2, &sel) == 0)
            return sel;
        else
            return 0;
    }
    else
        return vid_seg;
#endif  
}

int video_init(void)
{
#if defined(_MSC_VER) && (_MSC_VER >= 600)
    vid = get_vid_mem();
    return 1;
#else
    return !! (vid_mem = MAKEP(get_vid_mem(), 0));
#endif
}

#define MAKEWORD(l, h)  (((unsigned short) (l)) | ((unsigned short) (h)) << 8)

#if 1
/* faster and safer version */
void wrt_str(int y, int x, ATTRIB attr, unsigned char *p)
{
#if defined(_MSC_VER) && (_MSC_VER >= 600)
    unsigned _based(vid) *v = (unsigned _based(vid) *) SCR(y, x);
#else
    unsigned short far *v = vid_mem + SCR(y, x);
#endif
    int ok = 80 - x;
    while (ok && *p)
    {
        *v++ = MAKEWORD(*p++, attr);
        ok--;
    }
}
#else
void wrt_str(int y, int x, ATTRIB attr, unsigned char *p)
{
#if defined(_MSC_VER) && (_MSC_VER >= 600)
    BYTE _based(vid) *v = SCR(y, x);
#else
    BYTE far *v = vid_mem + SCR(y, x);
#endif
    while (*p)
    {
        *v++ = *p++;
        *v++ = attr;
    }
}
#endif

static char buf[128] = {0};

void wrt_printf(int y, int x, ATTRIB attr, char *fmt, ...)
{
    va_list marker;
    va_start(marker, fmt);
    vsprintf(buf, fmt, marker); 
    va_end(marker);
    wrt_str(y, x, attr, buf);
}

void wrt_chr(int y, int x, ATTRIB attr, unsigned char c)
{
#if defined(_MSC_VER) && (_MSC_VER >= 600)
    BYTE _based(vid) *v = (BYTE _based(vid) *)SCR(y, x);
#else
    BYTE far *v = vid_mem + SCR(y, x);
#endif
    *v++ = c;
    *v = attr;
}

void set_attr(int starty, int startx, int endy, int endx, ATTRIB attr)
{
    int x, y;
    for (y=starty; y<=endy; y++)
    {
#if defined(_MSC_VER) && (_MSC_VER >= 600)
        BYTE _based(vid) *v = (BYTE _based(vid) *) SCR(y, startx);
#else
        BYTE far *v = vid_mem + SCR(y, startx);
#endif
        for (x=startx; x<=endx; x++)
        {
            v++;
            *v++ = attr;
        }
    }
}

void fill(int starty, int startx, int endy, int endx, unsigned char c,
    ATTRIB attr)
{
#if defined(_MSC_VER) && (_MSC_VER >= 600)
    BYTE _based(vid) *v;
#else
    BYTE far *v = vid_mem;
#endif
    int x, y;
    for (y=starty; y<=endy; y++)
#if defined(_MSC_VER) && (_MSC_VER >= 600)
        for (x=startx, v=(BYTE _based(vid) *)SCR(y, startx); x<=endx; x++)
#else           
        for (x=startx, v=vid_mem+SCR(y, startx); x<=endx; x++)
#endif          
        {
            *v++ = c;
            *v++ = attr;
        }
}

void clear(int starty, int startx, int endy, int endx, ATTRIB attr)
{
#if (defined(_MSC_VER) && (_MSC_VER >= 600)) || defined(__BORLANDC__)
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

void cls(void)
{
    clear(0, 0, 24, 79, NORMAL);
}

static unsigned char brd[2][6] = {
    179, 196, 218, 191, 192, 217,       /* single box chars */
    186, 205, 201, 187, 200, 188        /* double box chars */
    } ;

void border(int starty, int startx, int endy, int endx, ATTRIB attr, int dbl)
{
#if defined(_MSC_VER) && (_MSC_VER >= 600)
    BYTE _based(vid) *v;
#else
    BYTE far *v;
#endif
    unsigned char *b;
    register int i;
    
    b = brd[(dbl-1) & 1];
    
    for (i=starty+1; i<endy; i++)
    {
        wrt_chr(i, startx, attr, *b);
        wrt_chr(i, endx, attr, *b);
    }
    b++;
#if defined(_MSC_VER) && (_MSC_VER >= 600)
    for (i=startx+1, v=(BYTE _based(vid) *) SCR(starty, startx+1); i<endx; i++)
#else       
    for (i=startx+1, v=vid_mem+SCR(starty, startx+1); i<endx; i++)
#endif      
    {
        *v++ = *b;
        *v++ = attr;
        // equivalent to wrt_chr(starty, i, attr, *b);
    }
#if defined(_MSC_VER) && (_MSC_VER >= 600)
    for (i=startx+1, v=(BYTE _based(vid) *)SCR(endy, startx+1); i<endx; i++)
#else       
    for (i=startx+1, v=vid_mem+SCR(endy, startx+1); i<endx; i++)
#endif      
    {
        *v++ = *b;
        *v++ = attr;
        // equivalent to wrt_chr(endy, i, attr, *b);
    }
    b++;
    wrt_chr(starty, startx, attr, *b++);
    wrt_chr(starty, endx, attr, *b++);
    wrt_chr(endy, startx, attr, *b++);
    wrt_chr(endy, endx, attr, *b);
}

void cursor(int on)
{
    static int old_curs = 0;
    if (on && old_curs)
    {
#if (defined(_MSC_VER) && (_MSC_VER >= 600)) || defined(__BORLANDC__)
        _asm mov cx, old_curs
        _asm mov ah, 1
        _asm int 10h
#else
        union REGS r;
        r.x.cx = old_curs;
        r.h.ah = 1;
        int86(0x10, &r, &r);
#endif
    }
    else if (! (on || old_curs))
    {
#if (defined(_MSC_VER) && (_MSC_VER >= 600)) || defined(__BORLANDC__)
        _asm mov ah, 3
        _asm mov bh, 0
        _asm int 10h
        _asm mov old_curs, cx
        _asm mov cx, 2000h
        _asm mov ah, 1
        _asm int 10h
#else
        union REGS r;
        r.h.ah = 3;
        r.h.bh = 0;
        int86(0x10, &r, &r);
        old_curs = r.x.cx;
        r.x.cx = 0x2000;
        r.h.ah = 1;
        int86(0x10, &r, &r);
#endif
    }
}

void center(int y, ATTRIB attr, char *s)
{
    wrt_str(y, 40 - (strlen(s) >> 1), attr, s);
}
