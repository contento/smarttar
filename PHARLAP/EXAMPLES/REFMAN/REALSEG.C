/* 
REALSEG.C -- allocate conventional memory from 286|DOS-Extender

cl -Lp realseg.c
run286 realseg

sample output:
para=5C0E sel=01C7
This is a test of DosAllocRealSeg
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <phapi.h>

void fail(char *s) { puts(s); exit(1); }

#define SIZE    (32L << 10)     // 32k

main()
{
    REGS16 regs;
    DESC desc;
    void far *fp;
    char *msg;
    USHORT para, sel, ret;
    
    /* allocate 32k block in conventional memory */
    if ((ret = DosAllocRealSeg(SIZE, &para, &sel)) != 0)
    {
        printf("ret=%04X\n", ret);
        fail("DosAllocRealSeg - failure");
    }
    
    printf("para=%04X sel=%04X\n", para, sel);
    
    /* get 286|DOS-Extender descriptor */
    if (DosGetSegDesc(sel, &desc) != 0)
        fail("DosGetSegDesc - failure");
    
    /* these assertions always hold */
    assert(desc.base == ((ULONG) para << 4));
    assert(desc.size == SIZE);
    
    /* Here, we put some data in the block using the protected-mode
    selector (since our program is running in protected mode). We
    then pass the block to some real-mode code, using the paragraph
    address that the real-mode code knows how to deal with.
        286|DOS-Extender supports the DOS Write File function
    (INT 21h Function 40h) transparently in protected mode, but
    here we'll pretend that it doesn't, to provide an example
    of DosAllocRealSeg(). */
    
    /* put some data in the block, using protected-mode address */
    fp = MAKEP(sel, 1024);  // form a far pointer to somewhere in block
    msg = "This is a test of DosAllocRealSeg\r\n";
    _fstrcpy(fp, msg);      // copy msg into conventional memory
    
    /* now call real-mode code, using the paragraph address */
    memset(&regs, 0, sizeof(REGS16));   // clear to zero
    regs.ax = 0x4000;       // INT 21h Function 40h
    regs.bx = 1;            // stdout
    regs.cx = strlen(msg);  // length of bytes to write
    regs.ds = para;         // Paragraph address used here, not sel!
    regs.dx = OFFSETOF(fp); // offset same for sel and para
    if (DosRealIntr(0x21, &regs, 0L, 0) != 0)
        fail("DosRealIntr - failure");

    /* free the block using the protected-mode selector */
    if (DosFreeSeg(sel) != 0)
        fail("DosFreeSeg - failure");
    
    return 0;
}
