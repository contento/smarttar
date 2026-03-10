/*
MACHMODE.C -- demonstrates DosGetMachineMode()

cl -Lp -Fb machmode.c

sample output:
    C:\>machmode
    This program is running in real mode

    C:\>run286 machmode
    This program is running in protected mode
*/

#include <stdlib.h>
#include <stdio.h>
#include <phapi.h>

void fail(char *s) { puts(s); exit(1); }

main()
{
    BYTE mode;
    if (DosGetMachineMode(&mode) != 0)
        fail("DosGetMachineMode - failure");
    switch (mode)
    {
        case MODE_REAL:
            puts("This program is running in real mode");
            // can bang on absolute memory locations
            break;
        case MODE_PROTECTED:
            puts("This program is running in protected mode");
            // can allocate tons of memory
            break;
        default:
            fail("Oh, no! Not another mode?!");
    }
    return 0;
}
