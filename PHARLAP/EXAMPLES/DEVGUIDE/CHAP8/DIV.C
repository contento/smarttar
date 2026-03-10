/* 
DIV.C
cl -Lp div.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <phapi.h>

PEHANDLER old0;
PEHANDLER oldgp;

void interrupt far zero(/* EXCEP_FRAME e */)
{
    puts("Divide by zero!");
    DosSetExceptionHandler(0, old0, NULL);
    exit(1);
}

void interrupt far gpfault(/* EXCEP_FRAME e */)
{
    puts("usage: div [numerator] [denominator]");
    DosSetExceptionHandler(0, oldgp, NULL);
    exit(1);
}

main(int argc, char *argv[])
{
    int ret;
    DosSetExceptionHandler(0, zero, &old0);
    DosSetExceptionHandler(0x0D, gpfault, &oldgp);
    ret = atoi(argv[1]) / atoi(argv[2]);
    DosSetExceptionHandler(0x0D, oldgp, NULL);
    DosSetExceptionHandler(0, old0, NULL);
    return ret;
}

