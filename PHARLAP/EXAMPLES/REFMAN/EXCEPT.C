/*
EXCEPT.C

cl -Lp except.c

sample output:
EXCEPT 00 ===> 01FF:00D1
EXCEPT 01 ===> DEFAULT
...
*/

#include <stdlib.h>
#include <stdio.h>
#include <phapi.h>

main()
{
    PEHANDLER pehandler;
    int intno;
    
    for (intno=0; intno<=0x1F; intno++)
        if (DosGetExceptionHandler(intno, &pehandler) == 0)
            if (pehandler == (PEHANDLER) 0)
                printf("EXCEPT %02X ===> DEFAULT\n", intno);
            else
                printf("EXCEPT %02X ===> %Fp\n", intno, pehandler);
        
    return 0;
}
