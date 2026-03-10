/* 

CRITERR.C

        bcc286 criterr.c 

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <process.h>
#include <conio.h>
#include <dos.h>
#include <phapi.h>

#ifdef DOSX286
PIHANDLER old_crit_err_prot;
REALPTR old_crit_err_real;
#else
void (interrupt far *old_crit_err)();
#endif

void put_str(char far *s)
{
    _asm push ds
    _asm mov ah, 9          // Display String
    _asm lds dx, s
    _asm int 21h
    _asm pop ds
}

int get_char(void)
{
    _asm mov ah, 1          // Character Input with Echo
    _asm int 21h
    _asm xor ah, ah
}

#ifdef _MSC_VER            
void interrupt far critical_error(REGS16 r)
#else
void interrupt far critical_error(REGS_BINT r)
#endif
{
    int c;
    unsigned _ah = r.ax & 0xFF00;
    unsigned _al;
    do {
#ifdef DOSX286      
        if (DosIsRealIntr((PVOID) &r))
            put_str("CRITICAL ERROR IN REAL MODE\r\n$");
        else
            put_str("CRITICAL ERROR IN PROTECTED MODE\r\n$");
        put_str("HANDLED IN PROTECTED MODE\r\n$");
#else
        put_str("CRITICAL ERROR\r\n$");
#endif
        put_str("Abort, Retry, Ignore, Fail? $");
        c = get_char();
        put_str("\r\n$");
    } while (! strchr("AaRrIiFf", c));
    
    switch (toupper(c))
    {
        enum { IGNORE, RETRY, ABORT, FAIL } criterr_ret;
        case 'A' : _al = ABORT; break;
        case 'R' : _al = RETRY; break;
        case 'I' : _al = IGNORE; break;
        case 'F' : _al = FAIL; break;
    }
    
    r.ax = _ah + _al;
}

main(int argc, char *argv[])
{
    if (argc < 2)
    {
        puts("usage: criterr [program [args...]]");
        puts("example: criterr \\bin\\ls.exe a:");
        return 1;
    }
    
#ifdef DOSX286  
    DosSetPassToProtVec(0x24, (PIHANDLER)critical_error,
        &old_crit_err_prot, &old_crit_err_real);

    /* can't do system() here: COMMAND.COM has its own INT 24 handler */
    spawnvp(P_WAIT, argv[1], &argv[1]);
    
    DosSetRealProtVec(0x24, old_crit_err_prot, old_crit_err_real,
        NULL, NULL);
#else
    old_crit_err = _dos_getvect(0x24);
    _dos_setvect(0x24, critical_error);
    /* can't do system() here: COMMAND.COM has its own INT 24 handler */
    spawnvp(P_WAIT, argv[1], &argv[1]);
    _dos_setvect(0x24, old_crit_err);
#endif  
    puts("bye");
    return 0;
}

