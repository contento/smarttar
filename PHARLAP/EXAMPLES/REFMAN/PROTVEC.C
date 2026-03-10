/*
PROTVEC.C -- demonstrates protected-mode interrupt vectors
    in 286|DOS-Extender
        
cl -Lp protvec.c -link slibpe.lib      
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <dos.h>
#include <phapi.h>

#define HLT         0xF4    /* opcode for Intel HLT instruction */

void fail(char *s)  { puts(s); exit(1); }

main(int argc, char *argv[])
{
    void (interrupt far *phandler)();
    int intno;
    
    if (argc < 2)
        fail("usage: protvec [hex intno]");
    else
        sscanf(argv[1], "%02X", &intno);
    
    if (DosGetProtVec(intno, &phandler) != 0)
        fail("DosGetProtVec - failure");
    
    /* check first byte to see if this is really a handler */
    if ((! phandler) || (*((BYTE far *) phandler) == HLT))
        printf("INT %02X - no protected-mode handler\n", intno);
    else
        printf("INT %02Xh ProtVec = %Fp\n", intno, phandler);
        
    /*  show that DosGetProtVec is equivalent to 
        protected-mode INT 21h Function 35h (DOS Get Vector)
        by calling INT 21h Function 35h from protected mode
        (we use Microsoft C _dos_getvect() function here) */
    assert(_dos_getvect(intno) == phandler);
    
    return 0;
}
