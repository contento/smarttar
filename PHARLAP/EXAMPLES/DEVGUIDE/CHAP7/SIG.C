/* 
SIG.C
cl -Lp sig.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <setjmp.h>

jmp_buf toplevel;

void ctrl_c_handler(void)
{
#ifdef SIG_ACK
    signal(SIGINT, SIG_ACK);        // acknowledge signal (pmode only)
#endif
    signal(SIGINT, ctrl_c_handler); // reinstall handler
    longjmp(toplevel, -1);          // throw
}

main()
{
    signal(SIGINT, ctrl_c_handler); // install handler
        
    if (setjmp(toplevel))   // catch
    {
        /* in a genuine program, do cleanup here */
        puts("[break]");
    }
    puts("type EXIT to exit");
    for (;;)
    {
        char buf[80];
        fputs("> ", stdout);
        strupr(gets(buf));
        if (strcmp(buf, "EXIT") == 0)
            break;
        if (buf[0])
            puts(buf);
    }
    puts("bye");
    return 0;
}

