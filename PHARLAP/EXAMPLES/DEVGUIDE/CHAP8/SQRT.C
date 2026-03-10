/* 
SQRT.C -- for MSC 6.0+
cl -Lp sqrt.c

NOTE: If run under Windows 3.0 enhanced mode on a machine with a numeric
coprocessor, RUN286 SQRT -1 produces Fatal Error 286.3390:

Fatal error 286.3390: An unmasked coprocessor exception has occurred.
In Windows 3.0, protected mode coprocessor exceptions are not supported.
*/

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <math.h>
#include <float.h>

void sigfpe(int sig, int error)
{
    _fpreset();
    if (error == FPE_SQRTNEG)   // in FLOAT.H
        puts("Square root of negative number");
    else
        puts("Some other floating point exception");
    exit(1);
}

double my_sqrt(double x)
{
    double y;
    _asm fld x  ; load
    _asm fsqrt
    _asm fstp y  ; store and pop
    return y;
}

void fail(char *s) { puts(s); exit(1); }

main(int argc, char *argv[])
{
    double x, y;
    
    signal(SIGFPE, sigfpe);
    x = atof(argv[1]);
    y = my_sqrt(x);
    printf("sqrt(%f) ==> %f\n", x, y);
}

