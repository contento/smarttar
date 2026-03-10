/* 
TRYDLL.C -- test driver for TESTDLL.DLL
cl -Lp trydll.c -link testdll.lib
*/

#include <stdlib.h>
#include <stdio.h>
#include "ftestdll.h"

double pi = 3.1415926535;
double x;

main()
{
    char buf[128];
    printf("Hello\n"); 
    printf("Value put is %f\n",pi);
    put(pi);
    get_half(&x);
    printf("Returned value should be %f, is %f\n",pi/2.0,x);
}
