/*
VEC.C

cl -Lp vec.c

sample output:
    Intno          Prot Vec            Real Vec
    -----          --------            --------
    00             0067:3A60           286E:3A3C
    01             0067:3A6B           286E:3A43
    02                                 F000:E2C3
    03             0067:3A76           286E:3A4A
    04             0067:3A81           286E:3A51
    05             0067:3A8C           2EF9:002C
    06             0067:3A97           286E:3A5F
    07             0067:3AA2           286E:3A66
    08                                 F000:FEA5
    ......
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <dos.h>
#include <phapi.h>

#define HLT         0xF4
#define IRET        0xCF

void fail(char *s) { puts(s); exit(1); }

main()
{
    void (interrupt far *phandler)();
    void (interrupt far *rhandler)();
    int intno;
    
    printf("Intno          Prot Vec            Real Vec\n");
    printf("-----          --------            --------\n");
    
    for (intno=0; intno<=0xFF; intno++)
    {
        if (DosGetRealProtVec(intno, &phandler, &rhandler) != 0)
            fail("DosGetRealProtVec - failure");

        /* okay to read code as if were data */
        if (*((BYTE far *) phandler) == HLT)
            phandler = (PIHANDLER) 0;
        
        /* to check e.g. if (*rhandler == IRET) would need
           to first use DosMapRealSeg */

        if (phandler || rhandler)
            printf("%02X             ", intno);
        else
            continue;
        
        if (phandler)
            printf("%Fp           ", phandler);
        else
            printf("                    ");
        
        if (rhandler)
            printf("%Fp\n", rhandler);
        else
            printf("\n");
    }

    return 0;
}
