/* 
MATH.C
cl -Lp math.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <phapi.h>
#include <string.h>

#ifndef TC_EXIT
#define TC_EXIT 0
#endif

PEHANDLER old0;
PEHANDLER oldgp;
jmp_buf toplevel;

/* four different handlers */
void interrupt far zero(/* EXCEP_FRAME e */)
{
    longjmp(toplevel, -1);
}

void interrupt far gpfault(/* EXCEP_FRAME e */)
{
    longjmp(toplevel, -2);
}

void ctrl_c_handler(void)
{
#ifdef SIG_ACK
    signal(SIGINT, SIG_ACK);        // acknowledge signal
#endif
    signal(SIGINT, ctrl_c_handler); // reinstall handler
    longjmp(toplevel, -3);
}

void pascal far cleanup(USHORT reason)
{
    if (reason != TC_EXIT)          // normal exit
        puts("Internal error: abnormal termination!");
    DosExitList(EXLST_EXIT, NULL);  // quit
}

main()
{
    char buf[80];
    char op;
    unsigned x, y, z;

    /* install all the various handlers */
    DosExitList(EXLST_ADD, cleanup);
    signal(SIGINT, ctrl_c_handler);
    DosSetExceptionHandler(0, zero, &old0);
    DosSetExceptionHandler(0x0D, gpfault, &oldgp);

    /* After an exception or interrupt, execution resumes here */
    switch (setjmp(toplevel))
    {
        case -1 : puts("Divide by zero!"); break;
        case -2 : puts("GP fault!"); break;
        case -3 : puts("Ctrl-C!"); break;
        case -4 : puts("syntax: op1 op op2, or Q to quit"); break;
    }
    for (;;)
    {
        fputs("$ ", stdout);
        gets(buf);
        if (! *buf)
            continue;
        if (toupper(*buf) == 'Q')
            break;
        sscanf(buf, "%u %c %u", &x, &op, &y);
        switch (op)
        {
            case '+' : z = x + y; break;
            case '-' : z = x - y; break;
            case '*' : z = x * y; break;
            case '/' : z = x / y; break;
            case '%' : z = x % y; break; // MOD: another source of div by zero
            case '^' : z = x ^ y; break;
            case ':' : z = *((unsigned far *) MAKEP(x,y)); break; // peek
            default: longjmp(toplevel, -4);
        }
        printf("%u\n", z);
    }

    DosSetExceptionHandler(0x0D, oldgp, NULL);
    DosSetExceptionHandler(0, old0, NULL);
    return 0;
}

