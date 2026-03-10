/* cl -Lp b800.c */

#include <stdlib.h>
#include <stdio.h>
#include <phapi.h>

PEHANDLER oldgp;

void interrupt far gpfault(EXCEP_FRAME e)
{
    printf("GP fault! ERRORCODE=%04X\n", e.error_code);
    DosSetExceptionHandler(0, oldgp, NULL);
    exit(1);
}

main()
{
    unsigned char far *screen = (unsigned char far *) 0xb8000000L;
    DosSetExceptionHandler(0x0D, gpfault, &oldgp);
    *screen = 'x';
    /* be very surprised if ever get here */
    DosSetExceptionHandler(0x0D, oldgp, NULL);
    return 0;
}

