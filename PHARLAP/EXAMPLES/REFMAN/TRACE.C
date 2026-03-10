/* 
TRACE.C -- Non-interactive debugger, demonstrates use of
    DosPTrace() in 286|DOS-Extender
        
cl -Lp trace.c
run286 trace [options] [program [args...]]
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <phapi.h>
#include "ptrace.h"

#ifndef FALSE
#define FALSE       0
#define TRUE        1
#endif

char *event_name[] = { 
    "OK", "ERROR", "SIGNAL", "SINGLE STEP", "BREAKPOINT", "PARITY ERROR",
    "DYING", "GP FAULT", "LOAD DLL", "FLOAT PT ERROR", "THREAD DEAD",
    "ASYNC STOP", "NEW PROCESS", "ALIAS FREE"
    } ;

char *progname(char *s);
char *cmdline(int argc, char *argv[], int start);
BOOL start_trace(char *prog, char *cmdline);
void gp_fault(void);
void load_dll(void);
void dying(void);
void print_regs(void);
void display_event(int event);
void help(void);

void fail(char *s) { puts(s); exit(1); }

PTRACEBUF ptb;
FILE *out;
char *exe;

#define PTRACE(p, x)    \
    p.cmd = x;          \
    DosPTrace(&p);

void help(void)
{
    puts("syntax: [run286] trace [trace options] <progr> [progr options]");
    puts("trace options:");
    puts("\t-O [file] : send output to file");
    puts("\t-S : DosPTrace command = SINGLE STEP");
    puts("\t-Q : DosPTrace command = SINGLE STEP, but quietly");
    exit(1);
}

main(int argc, char *argv[])
{
    unsigned long instr_count = 0L;
    unsigned ptrace_cmd = GO;
    unsigned event;
    BOOL quiet = FALSE;
    int i, j;
    
    puts("TRACE: 286|DOS-Extender non-interactive debugger");
    
    if (argc < 2)
        help();
    
    for (i=1, out=stdout; i<argc; i++)
        if (argv[i][0] == '-')
            switch (toupper(argv[i][1]))
            {
                case 'O' :  // send trace output to file
                    i++;
                    out = fopen(argv[i], "w");
                    break;
                case 'S' :  // single step
                    ptrace_cmd = SINGLE_STEP; 
                    break;
                case 'Q' :  // quiet single step
                    ptrace_cmd = SINGLE_STEP;
                    quiet = TRUE;
                    break;
                default :
                    help();
                    break;
            }
        else
        {
            exe = progname(argv[i]); // .EXE required
            j = i;
            break;
        }

    if (! start_trace(exe, cmdline(argc, argv, j)))
        fail("can't exec program");

    for (;;)
    {
        PTRACE(ptb, ptrace_cmd);
        
        if ((ptrace_cmd == SINGLE_STEP) && 
            (DosExit == MAKEP(ptb.rCS, ptb.rIP)))
        {
            printf("%lu instructions\n", instr_count);
            fail("Can't trace or single-step DosExit");
        }
        
        switch ((signed) ptb.cmd)
        {
            case EVENT_SIGNAL:
                printf("SIGNAL %u RECEIVED\n", ptb.value);
                break;
            case EVENT_GP_FAULT:    
                gp_fault(); 
                PTRACE(ptb, TERMINATE_CHILD);
                return 0;
            case EVENT_LOAD_DLL:    
                load_dll(); 
                break;
            case EVENT_DYING:   
                dying();
                PTRACE(ptb, TERMINATE_CHILD);
                return 0;
            case EVENT_SINGLESTEP: 
                instr_count++;
                if (! quiet)
                    display_event(ptb.cmd);
                break;
            default:        
                display_event(ptb.cmd);
                break;
        }
    }
}

char *progname(char *s)
{
    // don't append .EXE if already has an extension
    if (strrchr(s, '.') > strrchr(s, '\\'))
        return s;
    else
    {
        static char buf[256];
        strcpy(buf, s);
        strcat(buf, ".exe");
        return buf;
    }
}

#if 1
// thise was accidentally left out of the printed version
#define EXEC_TRACE      3
#endif

BOOL start_trace(char *prog, char *cmdline)
{
    RESULTCODES resc;
    if (DosExecPgm(NULL, 0, EXEC_TRACE, cmdline, NULL, &resc, prog))
        return FALSE;
    ptb.pid = resc.codeTerminate;   // Process ID number
    ptb.tid = 0;                    // Thread ID number
    return TRUE;
}

char *cmdline(int argc, char *argv[], int start)
{
    static char s[256], *p;
    int len;
    int i;
    strcpy(s, argv[start]);
    len = strlen(s);
    s[len] = '\0';
    s[len+1] = ' ';
    p = s+len+2;
    for (i=start+1; i<argc; i++)
    {
        strcat(p, argv[i]);
        strcat(p, " ");
    }
    return s;
}

void gp_fault(void)
{
     fprintf(out, "Module %u received GP fault: error=%04X at %04X:%04X\n",
         ptb.mte, ptb.value, ptb.segv, ptb.offv);
     print_regs();
}

void print_regs(void)
{
     PTRACEBUF p;
     p.pid = ptb.pid;
     p.tid = ptb.tid;
     PTRACE(p, READ_REGISTERS);
     fprintf(out, "AX=%04X BX=%04X CX=%04X DX=%04X SI=%04X DI=%04X BP=%04X\n",
         p.rAX, p.rBX, p.rCX, p.rDX, p.rSI, p.rDI, p.rBP);
     fprintf(out, "DS=%04X ES=%04X IP=%04X CS=%04X  F=%04X SP=%04X SS=%04X\n",
         p.rDS, p.rES, p.rIP, p.rCS, p.rF,  p.rSP, p.rSS);
     fflush(out);
}

// can't use DosGetModName() with DosPTrace()
void load_dll(void)
{
     char buf[128], far *fp=buf;
     ptb.segv = SELECTOROF(fp);
     ptb.offv = OFFSETOF(fp);
     PTRACE(ptb, GET_DLL_NAME);
     fprintf(out, "Loading module %u: %s\n", ptb.value, buf);
}

void dying(void)
{
    fprintf(out, "%s dying\n", exe);
    fprintf(out, "Bye!\n");
    fflush(out);
    fclose(out);
}

void display_event(int event)
{
    fprintf(out, "TID=%d EVENT %d: ", ptb.tid, event);

    if ((event <= EVENT_SUCCESS) && (event >= EVENT_ALIAS_FREE))
        fprintf(out, "%s\n", event_name[-event]);
    else
        fprintf(out, "???\n");
    if (event == EVENT_THREAD_DEAD)
        fprintf(out, "TID %d DEAD\n", ptb.tid);
    print_regs();
    fprintf(out, "\n");
}

