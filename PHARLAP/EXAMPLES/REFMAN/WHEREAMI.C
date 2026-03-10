/* 
WHEREAMI.C

cl -Lp whereami.c
bind whereami.exe -n DOSISPHARLAP

NOTE: This BIND command line is for the version of BIND included with
Microsoft C 6.0+ (it does not work with BIND 1.0, included with MSC 5.1)

real mode:
    whereami

286|DOS-Extender:
    run286 whereami

OS/2:
    copy \run286\lib\phapios2.dll \os2\dll\phapi.dll
    whereami
*/

#include <stdio.h>
#include <phapi.h>

main()
{
    UCHAR mode;
    DosGetMachineMode(&mode);
    if (mode == MODE_REAL)
        puts("Real mode");
    else if (mode == MODE_PROTECTED)
    {
        puts("Protected mode");
        if (DosIsPharLap())
        {
            puts("Running under 286|DOS-Extender");
            // can make PHAPI calls here
        }
        else
        {
            puts("Running under OS/2");
            // any PHAPI calls here will return error
        }
    }
    else
        puts("Oh, no! Not another mode!");
}

