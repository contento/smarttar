/* 
TRUENAME.C
from the book UNDOCUMENTED DOS, Chapter 4
fixed a bug in the printed version

real mode: cl truename.c
           bcc truename.c
protected mode: cl -Lp -DDOSX286 truename.c
                bcc286 truename.c

NOTE: If run under DOS 4 or higher, need to run .\TRUENAME, since TRUENAME
is an (undocumented) internal DOS command!
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>

void fail(char *s) { puts(s); exit(1); }

#ifdef DOSX286
#include <phapi.h>

char far *truename(char far *s, char far *d)
{
    char far *s2;
    REGS16 r;
    USHORT spara, dpara;
    SEL sseg, dseg;
    
    /* INT 21h AH=60h doesn't like leading or trailing blanks */
    while (isspace(*s))
        s++;
    s2 = s;
    while (*s2) s2++;
    s2--;
    while (isspace(*s2))
        *s2-- = 0;

    memset(&r, 0, sizeof(r));   /* always initialize a REGS16 to 0 */

    /* try to allocate the conventional-memory transfer buffers */
    if (DosAllocRealSeg(128, &spara, &sseg) != 0) 
        return (char far *) 0;
    if (DosAllocRealSeg(128, &dpara, &dseg) != 0) 
    {
        DosFreeSeg(sseg);   /* if second failed, free first */
        return (char far *) 0;
    }

    /* copy SOURCE into transfer buffer BEFORE call, using the
       protected-mode selector */
    _fstrcpy(MAKEP(sseg, 0), s);

    /* pass the real-mode paragraph addresses to DOS */
    r.ds = spara;
    r.si = 0;
    r.es = dpara;
    r.di = 0;
    r.ax = 0x6000;          /* ah=60h */
    DosRealIntr(0x21, &r, 0, 0);

    /* copy transfer buffer into DESTINATION AFTER call, using
       the protected-mode selector */
    _fmemcpy(d, MAKEP(dseg, 0), 128);

    /* now free the transfer buffers, using the prot mode selectors */
    DosFreeSeg(sseg);
    DosFreeSeg(dseg);

    if (r.flags & 1)
        return (char far *) 0;
    else
        return d;
}
#else
char far *truename(char far *s, char far *d)
{
    char far *s2;
    
    /* INT 21h AH=60h doesn't like leading or trailing blanks */
    while (isspace(*s))
        s++;
    s2 = s;
    while (*s2) s2++;
    s2--;
    while (isspace(*s2))
        *s2-- = 0;
    
    _asm push di        /* save regs that compiler wants preserved */
    _asm push si
    _asm les di, d      /* destination */
    _asm lds si, s      /* source */
    _asm mov ah, 60h
    _asm int 21h        /* INT 21h Function 60h */
    _asm pop si
    _asm pop di
    _asm jc error       /* if carry set, error */
    return d;           /* else return destination string */
error:
    return (char far *) 0;
}
#endif

main(int argc, char *argv[])
{
    char buf[128];
    if (argc < 2)
        fail("usage: truename <filename>");

    if (_osmajor < 3)
        fail("requires DOS 3.0 or greater");

    if (! truename(argv[1], buf))
        fail("invalid filename");

    puts(buf);
    return 0;
}
            
