/*
WINCLIP.C -- DOS program communication with Windows clipboard
*/

#include <stdlib.h>
#include <malloc.h>
#include <dos.h>

#include "winclip.h"

/* old application clipboard functions */
#define WINOLDAP_VERS   0x1700
#define OPEN_CLIP       0x1701
#define EMPTY_CLIP      0x1702
#define SET_CLIP_DATA   0x1703
#define GET_CLIP_SIZE   0x1704
#define GET_CLIP_DATA   0x1705
#define CLOSE_CLIP      0x1708
#define CLIP_COMPACT    0x1709
#define GET_DEV_CAPS    0x170A

#define MAKELONG(a, b)  ((long)(((unsigned)(a)) | \
    ((unsigned long)((unsigned)(b))) << 16))

int WinOldApVersion(int *maj, int *min)
{
    union REGS r;
    r.x.ax = WINOLDAP_VERS;
    int86(0x2F, &r, &r);
    if (r.x.ax == WINOLDAP_VERS)    /* didn't change */
        return FALSE;
    else
    {
        *maj = r.h.al;
        *min = r.h.ah;
        return TRUE;
    }
}
    
unsigned GetDeviceCaps(unsigned cap)
{
    union REGS r;
    r.x.ax = GET_DEV_CAPS;
    r.x.dx = cap;
    int86(0x2F, &r, &r);
    return r.x.ax;
}

void OpenClipboard(void)
{
    union REGS r;
    r.x.ax = OPEN_CLIP;
    int86(0x2F, &r, &r);
}

unsigned long GetClipboardSize(CF_FORMAT format)
{
    union REGS r;
    r.x.ax = GET_CLIP_SIZE;
    r.x.dx = format;
    int86(0x2F, &r, &r);
    return MAKELONG(r.x.ax, r.x.dx);
}
    
char far *GetClipboardData(CF_FORMAT format)
{
    union REGS r;
    struct SREGS s;
    char far *buf;
    if (! (buf = _fmalloc(GetClipboardSize(format))))
        return (char far *) 0;
    segread(&s);
    r.x.ax = GET_CLIP_DATA;
    r.x.dx = format;
    s.es = FP_SEG(buf);
    r.x.bx = FP_OFF(buf);
    int86x(0x2F, &r, &r, &s);
    return (r.x.ax) ? buf : (char far *) 0;
}

void FreeClipboardData(char far *buf)
{
    _ffree(buf);
}

/*
needs:
    SetClipboardData();
    ClipboardCompact();
*/
    
void CloseClipboard(void)
{
    union REGS r;
    r.x.ax = CLOSE_CLIP;
    int86(0x2F, &r, &r);
}
    
