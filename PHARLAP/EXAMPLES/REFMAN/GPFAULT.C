/* 
GPFAULT.C -- catch General Protection violation (GP fault)
    in 286|DOS-Extender, using DosSetExceptionHandler()
        
cl -Lp -Gs gpfault.c
run286 gpfault

sample output:
    GP fault! error=1234
*/

#include <stdio.h>
#include <string.h>
#include <phapi.h>

void fail(char *s) { puts(s); exit(1); }

PEHANDLER old_gpfault;

/* Create an exception handler, using Microsoft C interrupt keyword */
void interrupt far gpfault_handler(EXCEP_FRAME e)
{
    printf("GP fault! error=%04X\n", e.error_code);
    DosSetExceptionHandler(0x0D, old_gpfault, NULL);
    exit(1);
}

main(int argc, char *argv[])
{
    USHORT catch;
    
    if (catch = (argc < 2) ? 1 : 0)
        DosSetExceptionHandler(0x0D, gpfault_handler, &old_gpfault);
    
    *((USHORT far *) 0x12345678L) = 666;    /* cause GP fault */
    /* still here? */
    puts("ok");
    DosSetExceptionHandler(0x0D, old_gpfault, NULL);
    return 0;
}
