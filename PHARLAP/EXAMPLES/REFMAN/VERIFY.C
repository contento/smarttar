/*
VERIFY.C

cl -Lp verify.c

sample output:
    ...
    01F7 READABLE WRITEABLE DATA
    01FF READABLE CODE
    0207 READABLE WRITEABLE DATA
    ...
*/

#include <stdlib.h>
#include <stdio.h>
#include <phapi.h>

main()
{
    USHORT i;
    USHORT flags;
    
    for (i=0; i<0xFFFF; i++)
        if (DosVerifyAccess(i, &flags) == 0)
            if (flags & IS_SEL)
            {
                printf("%04X ", i);
                if (flags & IS_READABLE) printf("READABLE ");
                if (flags & IS_WRITEABLE) printf("WRITEABLE ");
                printf("%s\n", (flags & IS_CODE) ? "CODE" : "DATA");
            }
            else if (i && (! (i % 0x1000)))     // odometer
                putchar('.');
    return 0;
}
