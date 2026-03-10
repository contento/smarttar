/*
GRMEM.C

bound executable (Microsoft C 6.0+)
    cl -Lp -AL -Ox grmem.c fullscrn.c -link llibpe.lib
    bind grmem.exe -n DOSMAPREALSEG DOSGETSEGDESC
        
to run in real mode: C:\DOS>grmem
to run in protected mode: C:\DOS>run286 grmem
Note: BIND 1.0 (included with MSC 5.1) doesn't work this way

Not bound protected-mode executable (Microsoft C/C++, Visual C++)
    cl -Lp -AL -Ox grmem.c fullscrn.c llibpe.lib protmode.def

Not bound protected-mode (Borland C++)
    bcc286 grmem.c fullscrn.c

To run in protected mode:
    C:\DOS>grmem
        
        
Also contains conditional code for Phar Lap 386|DOS-Extender and Watcom C/386:
    wcl386 -p -3r -mf -Oaxt -fpc -DDOSX386 grmem.c fullscrn.c
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <dos.h>
#include "fullscrn.h"

#ifdef DOSX386
char *outofmem = "Out of memory! You need Phar Lap's 386|DOS-Extender!";
#else
#include <phapi.h>

char *outofmem = "Out of memory! You need Phar Lap's 286|DOS-Extender!";
char *insuff_ext = "Insufficient extended memory";
#endif


int desktop_char = HATCH1;
int desktop_attr = 0x33;
int have_key = -1;

#define FATAL   1
#define OK      0

void bye(int err)
{
    cls();
    cursor(1);
    exit(err);
}

void msg(int fatal, char *s)
{
    time_t t1, t2;
    char *press = "Press any key";
    int len = strlen(s);
    int s_x = 40 - (len >> 1);
    clear(10, s_x - 5, 15, s_x + len + 5, REVERSE);
    border(10, s_x - 5, 15, s_x + len + 5, REVERSE, DOUBLE);
    wrt_str(12, s_x, REVERSE, s);
    wrt_str(13, 40 - (strlen(press) >> 1), REVERSE, press);
    time(&t1);
    while (! kbhit())
    {
        time(&t2);
        if ((t2 - t1) > 10)
            goto done;
    }
    getch();
done:   
    fill(10, s_x - 5, 15, s_x + len + 5, desktop_char, desktop_attr);
    if (fatal)
        bye(1);
}

#define CONV_MEM    0
#define EXT_MEM     1

int boxfore[2] = { REDFORE, BLUEFORE } ;
int boxback[2] = { REDBACK, BLUEBACK } ;
            
void box(int y, int x, int c, int mode, char *s)
{
    int i, j;
    for (i=y; i<(y+3); i++)
        for (j=x; j<(x+6); j++)
            wrt_chr(i, j, ATTR(boxfore[mode], boxback[mode]), c);
    wrt_str(y+1, x+1, ATTR(GREENFORE, boxback[mode]), s);
}

void main(void)
{
    char buf[80], *str;
#ifdef DOSX386
    char *p;
#endif
    unsigned alloc;
    unsigned sel;
    int i, row, col;
    
    video_init();
    cursor(0);
    cls();
    clear(0, 0, 0, 79, REVERSE);
#ifdef DOSX386
    center(0, REVERSE, 
        "Running in protected-mode MS-DOS with Phar Lap's 386|DOS-Extender");
#else
    center(0, REVERSE, (_osmode == REAL_MODE) ?
        "Running in real-mode MS-DOS" :
        "Running in protected-mode MS-DOS with Phar Lap's 286|DOS-Extender");
#endif      
    fill(1, 0, 24, 79, desktop_char, desktop_attr);
    for (row=2, alloc=0; row<25; row+=4)
        for (col=3; col<80; col+=7)
        {
            for (i=0; i<4; i++)
            {
                if (kbhit())
                {
                    getch();
                    sprintf(buf, "Allocated %uk", alloc);
                    msg(FATAL, buf);
                }

#ifdef DOSX386
                if ((! (p = malloc(16 << 10))) &&       // 16k block
                    ((! (p = malloc(8 << 10))) &&       // try two 8k blks
                     (! (p = malloc(8 << 10)))))
#else
                // in paragraphs, not bytes
                if (_dos_allocmem(16 << 6, &sel) != 0)          // 16k block
                    if ((_dos_allocmem(8 << 6, &sel) != 0) &&   // try two
                        (_dos_allocmem(8 << 6, &sel) != 0))    //    8k blks
#endif                          
                {
                    if (alloc < 1024)   // 1 megabyte minimum
                        msg(FATAL, 
                            _osmode == MODE_PROTECTED ? 
                                insuff_ext :
                                outofmem);
                    else
                        goto fini;
                }
                alloc += 16;
            }
            str = ultoa(alloc, buf, 10);
#ifdef DOSX386
            box(row, col, BOX, EXT_MEM, str);
#else
            // figure out if p in real or extended memory
            if (_osmode == REAL_MODE)
                box(row, col, BOX, CONV_MEM, str);
            else
            {
                DESC d;
                DosGetSegDesc((SEL)sel, &d);
                if (d.base < 0x100000L) // conventional memory
                    box(row, col, BOX, CONV_MEM, str);
                else                    // extended memory
                    box(row, col, BOX, EXT_MEM, str);
            }
#endif          
        }
        
fini:       
    sprintf(buf, "Allocated %s%u megabytes under MS-DOS", 
        (alloc % 1024) ? "more than " : "",
        alloc >> 10);
    msg(OK, buf);
    bye(0);
}

