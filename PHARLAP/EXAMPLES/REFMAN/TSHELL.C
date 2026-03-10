/*
TSHELL.C -- tiny shell for 286|DOS-Extender

cl -Lp tshell.c

This program uses DosExecPgm() to provide a tiny command shell for
286|DOS-Extender. Commands are typed at the run286> prompt. Commands
can be optionally be run asynchronously by appending an ampersand (&)
to the end of the command.  For example, to run the Microsoft C
compiler in the background:

C:\DOS>run286 tshell
run286>cl -Lp memavail.c &

One built-in command is provided: EXIT (or QUIT)
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#if 1
// the following are missing are PHAPI.H 
typedef unsigned PID;

// DosExecPgm exec flags
#define EXEC_SYNC           0
#define EXEC_ASYNC          1
#define EXEC_ASYNCRESULT    2
#define EXEC_TRACE          3
#define EXEC_BACKGROUND     4

#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

extern unsigned far pascal DosSleep(unsigned long millisec);
#endif

PID do_exec(char *prog, char *cmdline, int flags)
{
    RESULTCODES resc;
    if (DosExecPgm(NULL, 0, flags, cmdline, NULL, &resc, prog))
        return 0;                       // failed
    else if (flags == EXEC_ASYNC)
        return resc.codeTerminate;      // process ID
    else
        return resc.codeResult;         // child exit code
}

char buf[256];
char program[256];

main()
{
    char *command;
    USHORT ret;
    int len;
    int async;
    
    for (;;)
    {
        fputs("run286> ", stdout);
        while (! kbhit())
            DosSleep(0);    // give async child processes chance to run
        buf[0] = '\0';
        if ((! gets(buf)) || (! *buf))
            continue;
        
        len = strlen(buf);
        if (buf[len-1] == '&')  // ampersand on end means ASYNC
        {
            async = TRUE;
            buf[len-1] = '\0';
        }
        else
            async = FALSE;
        buf[len+1] = '\0';       // extra NULL at end
        
        command = strtok(buf, " \t"); // also places NULL after command
        strupr(command);
        // see if built-in
        if ((strcmp(command, "EXIT") == 0) ||
            (strcmp(command, "QUIT") == 0))
            break;

        strcpy(program, command);
        strcat(program, ".EXE");
        
        if (async)
        {
            ret = do_exec(program, buf, EXEC_ASYNC);
            printf("async pid=%d\n", ret);
        }
        else if (ret = do_exec(program, buf, EXEC_SYNC))
            printf("ret=%d\n", ret);
    }
    puts("bye");
    return 0;
}

