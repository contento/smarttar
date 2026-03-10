/*
WHEREAMI.C -- 
call both OS/2 and DOS functions, under 286|DOS-Extender
This program shows off the ability to write a single executable
which runs under both DOS and OS/2, as a native OS/2 app.  While
this program compiles with all supported compilers, only the MSC
5.1 and 6 executables will actually run under OS/2 as a native
OS/2 1.x application.  (All 7 versions will run under OS/2 2.x
in the DOS box.)
       
Borland C++ 2.0, 3.0, 3.1: bcc286 whereami.c
Microsoft Visual C++ 1.0: cl -Lp whereami.c
Microsoft C/C++ 7.0: cl -Lp whereami.c
Microsoft C 6.0: cl -Lp whereami.c os2.lib
Microsoft C 5.1: cl -Lp whereami.c slibpe.lib doscalls.lib
*/

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#if (_MSC_VER < 700) && (! defined(__BORLANDC__))
#define INCL_NOPM
#define INCL_DOSMISC
#include <os2.h>      /* Only MSC 5.1, 6 support OS/2 */
#endif
#include <phapi.h>

#ifndef MODE_REAL
#define MODE_REAL           0
#define MODE_PROTECTED      1
#endif

void fail(char *s) { puts(s); exit(1); }

main()
{
    unsigned major=0, minor=0;
    unsigned vers=0;
    unsigned char mode=0;

    /* call MS-DOS Get Version function (INT 21h AX=3000) */
#if (_MSC_VER >= 600) || (defined(__BORLANDC__))
    /* inline assembler */
    _asm mov ax, 3000h
    _asm int 21h
    _asm mov byte ptr major, al
    _asm mov byte ptr minor, ah
#else
    /* using intdos() function from real mode library */
    union REGS r;
    r.x.ax = 0x3000;
    intdos(&r, &r);
    major = r.h.al;
    minor = r.h.ah;
#endif  
    printf("DOS version %d.%d\n", major, minor);

#if (_MSC_VER < 700) && (!defined(__BORLANDC__))
    /* call OS/2 DosGetVersion(), not present in <phapi.h> */
    if (DosGetVersion(&vers))
        fail("DosGetVersion failed");
    printf("OS/2 version %d.%d\n", HIBYTE(vers), LOBYTE(vers));
#endif

    /* use built-in global variables */
    printf("_osmajor/_osminor %d.%d\n", _osmajor, _osminor);
    
    /* call DosGetMachineMode(): can also use _osmode */
    if (DosGetMachineMode(&mode))
        fail("DosGetMachineMode failed");
    printf("%s mode\n", 
        (mode == MODE_REAL) ? "Real" : 
        (mode == MODE_PROTECTED) ? "Protected" : "?");

    return 0;
}
