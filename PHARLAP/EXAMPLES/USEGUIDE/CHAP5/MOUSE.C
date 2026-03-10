/*
MOUSE.C -- installs mouse callback routine, located in real-mode DLL

real mode:          
    cl -Gs mouse.c -MAml handler.asm
        
protected mode:
    masm -ml -DDOSX286 handler;
    link handler,handler.dll,,,handler.def
    cl -Lp -DDOSX286 -c mouse.c
    link mouse, mouse /noi,, /nod:slibce slibcep,mouse.def
        
usage:
    move mouse to update display
    left click for "Press any key" prompt
    right click to exit
        
SEE ALSO HANDLER.ASM, HANDLER.DEF

MOUSE.DEF:
    NAME MOUSE3
    PROTMODE
    DESCRIPTION 'PROTECTED MODE MOUSE APPLICATION'
    IMPORTS
        _ev_handler=handler._ev_handler
        _full=handler._full
        _x=handler._x
        _y=handler._y
        _fini=handler._fini
        _left=handler._left
*/

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

#ifdef DOSX286
#include <phapi.h>
#endif

void fail(char *s) { puts(s); exit(1); }

#ifndef FALSE
#define FALSE   0
#define TRUE    1
/* Microsoft C 5.1 doesn't like typedef enum { FALSE, TRUE } BOOL; */
#endif

typedef enum { LEFT=1, RIGHT=2, CENTER=4 } BUTTON;

typedef enum { MOUSE_MOVE=1, LEFT_PRESS=2, LEFT_REL=4, 
    RIGHT_PRESS=8, RIGHT_REL=16 } EVENT;

extern void far ev_handler(void);

typedef struct {
    unsigned buttondown;
    unsigned posx;
    unsigned posy;
    } MOUSE;
    
BOOL ResetMouse(unsigned *pbuttons);
void ShowMouse(void);
void HideMouse(void);
void GetMouse(MOUSE *mouse);
BOOL SetMouseEventHandler(EVENT mask, void (far *handler)());

extern BOOL far full;       // semaphore
extern BOOL far fini;
extern BOOL far left;
extern int far x;
extern int far y;

main()
{
    MOUSE m;
    void far *fp;
    unsigned buttons;
    short cursor;
    
#ifdef DOSX286  
    // automapped real mode seg never accessed as code
    fp = &full;
    DosSetSegAttrib(FP_SEG(fp), DATA16);
#endif  

    if (! ResetMouse(&buttons))
        fail("This program requires a Microsoft-compatible mouse");
    ShowMouse();
    if (! SetMouseEventHandler(MOUSE_MOVE | RIGHT_PRESS | LEFT_PRESS, 
        ev_handler))
    {
        HideMouse();
        fail("SetMouseEventHandler fail");
    }

    GetMouse(&m);
    x = m.posx; 
    y = m.posy;
    
    while (! fini)
    {
        // mouse stuff
        if (! full)
            continue;
        fprintf(stderr, "x=%5d y=%5d\r", x, y);
        if (left)
        {
            HideMouse();
            fputs("Press any key to continue...", stderr);
            getch();
            fputs("\r                            \r", stderr);
            printf("x=%5d y=%5d\r", x, y);
            ShowMouse();
            left = FALSE;
        }
        full = FALSE;
    }
    
    SetMouseEventHandler(0, NULL);
    HideMouse();
    printf("\n");
    return 0;    
}

BOOL ResetMouse(unsigned *pbuttons)
{
#ifdef DOSX286
    REGS16 r;
    memset(&r, 0, sizeof(REGS16));
    r.ax = 0;
    if (DosRealIntr(0x33, &r, 0L, 0) != 0)
        fail("DosRealIntr fail");
    if (r.ax == 0xFFFF) // this fixes embarassing bug in printed doc!
    {
        *pbuttons = r.bx;
        return TRUE;
    }
#else
    union REGS r;
    r.x.ax = 0;
    int86(0x33, &r, &r);
    if (r.x.ax == 0xFFFF)
    {
        *pbuttons = r.x.bx;
        return TRUE;
    }
#endif  
    else return FALSE;
}

void ShowMouse(void)
{
#ifdef DOSX286
    REGS16 r;
    memset(&r, 0, sizeof(REGS16));
    r.ax = 1;
    if (DosRealIntr(0x33, &r, 0L, 0) != 0)
        fail("DosRealIntr fail");
#elif defined(_MSC_VER) && (_MSC_VER >= 600)    
    _asm mov ax, 1
    _asm int 33h
#else
    union REGS r;
    r.x.ax = 1;
    int86(0x33, &r, &r);
#endif
}

void HideMouse(void)
{
#ifdef DOSX286  
    REGS16 r;
    memset(&r, 0, sizeof(REGS16));
    r.ax = 2;
    if (DosRealIntr(0x33, &r, 0L, 0) != 0)
        fail("DosRealIntr fail");
#elif defined(_MSC_VER) && (_MSC_VER >= 600)
    _asm mov ax, 2
    _asm int 33h
#else
    union REGS r;
    r.x.ax = 2;
    int86(0x33, &r, &r);
#endif  
}

void GetMouse(MOUSE *mouse)
{
#ifdef DOSX286  
    REGS16 r;
    memset(&r, 0, sizeof(REGS16));
    r.ax = 3;
    if (DosRealIntr(0x33, &r, 0L, 0) != 0)
        fail("DosRealIntr fail");
    mouse->buttondown = r.bx;
    mouse->posx = r.cx;
    mouse->posy = r.dx;
#elif defined(_MSC_VER) && (_MSC_VER >= 600)    
    unsigned status, x, y;
    _asm mov ax, 3
    _asm int 33h
    _asm mov status, bx
    _asm mov x, cx
    _asm mov y, dx
    mouse->buttondown = status;
    mouse->posx = x;
    mouse->posy = y;
#else
    union REGS r;
    r.x.ax = 3;
    int86(0x33, &r, &r);
    mouse->buttondown = r.x.bx;
    mouse->posx = r.x.cx;
    mouse->posy = r.x.dx;
#endif
}

BOOL SetMouseEventHandler(EVENT mask, void (far *handler)())
{
#ifdef DOSX286
    REGS16 r;
    memset(&r, 0, sizeof(REGS16));
    r.ax = 0x0c;
    r.cx = mask;
    handler = (void (far *)()) DosProtToReal((PVOID) handler);
    r.es = FP_SEG((void far *) handler);
    r.dx = FP_OFF((void far *) handler);
    if (DosRealIntr(0x33, &r, 0L, 0) != 0)
        fail("DosRealIntr fail");
#else
    union REGS r;
    struct SREGS s;
    segread(&s);
    r.x.ax = 0x0c;
    r.x.cx = mask;
    s.es = FP_SEG((void far *) handler);
    r.x.dx = FP_OFF((void far *) handler);
    int86x(0x33, &r, &r, &s);
#endif  
    return TRUE;
}

