/*
REALAVL.C
cl -Lp realavl.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <phapi.h>

main()
{
    ULONG avail;
    DosMemAvail(&avail);
    printf("%lu bytes available\n", avail);
    DosRealAvail(&avail);
    printf("%lu conventional-memory bytes available\n", avail);
    return 0;
}

