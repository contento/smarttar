/*
EXITLIST.C -- Demonstrates DosExitList() in 286|DOS-Extender

cl -Lp exitlist.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <phapi.h>

#if 1
// missing from PHAPI.H
// code-termination values (used with DosExitList)
#define TC_EXIT             0
#define TC_HARDERROR        1
#define TC_TRAP             2
#define TC_KILLPROCESS      3
#endif

void fail(char *s) { puts(s); exit(1); }

VOID pascal far cleanup(USHORT reason)
{
    // do cleanup (put back int vects, etc.) here
        
    switch (reason)
    {
        case TC_EXIT:           puts("Normal exit"); break;
        case TC_HARDERROR:      puts("Critical error"); break;
        case TC_TRAP:           puts("Exception [GP fault?]"); break;
        case TC_KILLPROCESS:    puts("Process killed"); break;
    }
    
    DosExitList(EXLST_EXIT, NULL);      // quit
}

main(int argc, char *argv[])
{
    if (DosExitList(EXLST_ADD, cleanup) != 0)
        fail("DosExitList fail");
    
    // hook an interrupt vector or something
        
    // do something that might cause abnormal termination
    for (;;)
    {
        char far *fp = MAKEP(rand(), rand());
        *fp = 0x1234;
    }
        
    // do cleanup (put back int vects, etc.) here
}
