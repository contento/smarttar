/* 
CTRL_C.C
cl -Lp -Gs ctrl_c.c 
*/

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <phapi.h>

#define CTRL_C      0x23
#define CTRL_BRK    0x1B

volatile int done;
int which;
int interrupted_func;

void interrupt far ctrl_c_handler(REGS16 r)
{
    done++;
    which = CTRL_C;
    interrupted_func = r.ax >> 8;
    /*
        The printed manual contains an odd line of code (attempting to
        abort the interrupted DOS function) which is not appropriate
        in this case!!
    */
}

void interrupt far ctrl_brk_handler(REGS16 r)
{
    done++;
    which = CTRL_BRK;
}

void fail(char *s) { fputs("fail: ", stdout); puts(s); exit(1); }

main()
{
    PIHANDLER old_ctrl_c_prot;
    REALPTR old_ctrl_c_real;
    PIHANDLER old_ctrl_brk_prot;
    REALPTR old_ctrl_brk_real;

    DosSetPassToProtVec(CTRL_BRK, (PIHANDLER)ctrl_brk_handler, 
        &old_ctrl_brk_prot, &old_ctrl_brk_real);
    DosSetPassToProtVec(CTRL_C, (PIHANDLER)ctrl_c_handler, 
        &old_ctrl_c_prot, &old_ctrl_c_real);
    
    puts("Type anything, or hit Ctrl-C or Ctrl-Break");

    /* simulate top-level of program with for (;;) input loop */
    for (;;)
    {
        int c;
        fputs("\n> ", stdout);
        done=0;
        for (done=0; ! done; )
            if (kbhit() && (! done))    // this check for !done was
                putch(getch());         // missing from printed version

        if (which == CTRL_BRK)    
            puts("\nReceived Ctrl-Break");
        else if (which == CTRL_C) 
            printf("\nReceived Ctrl-C during INT 21h AH=%02Xh\n", 
                interrupted_func);
        fputs("Ok to exit? [Y/N] ", stdout);
        c = toupper(getche());
        putchar('\n');
        if (c == 'Y')
            break;
    }

    DosSetRealProtVec(CTRL_BRK, old_ctrl_brk_prot,
        old_ctrl_brk_real, NULL, NULL);
    DosSetRealProtVec(CTRL_C, old_ctrl_c_prot,
        old_ctrl_c_real, NULL, NULL);

    return 0;
}

