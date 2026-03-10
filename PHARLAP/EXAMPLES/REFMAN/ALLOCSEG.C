/* 
ALLOCSEG.C -- demonstrates memory allocation in 286|DOS-Extender

cl -Lp allocseg.c -link slibpe.lib
run286 allocseg [bytes]

sample output:
sel=012F
sel=012F
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>

#include <phapi.h>

void fail(char *s) { puts(s); exit(1); }

main(int argc, char *argv[])
{
    USHORT ret, size;
    SEL sel;
    
    if (argc < 2)
        fail("usage: allocseg [bytes]");
    else
        size = atoi(argv[1]);
    
    /* allocate amount of memory given on command line */
    if ((ret = DosAllocSeg(size, &sel, 0)) != 0)
        fail((ret == 8) ?
            "Insufficient memory" : 
            "DosAllocSeg - failure");
        
    printf("sel=%04X\n", sel);
    
    /* free the block */
    if (DosFreeSeg(sel) != 0)
        fail("DosFreeSeg - failure");
    
    /* now do the same thing using MS-DOS functions:
       DOS Allocate function expects number of paragraphs, not bytes
       _dos_allocmem() and _dos_freemem() from Microsoft C (DOS.H) */
        
    if ((ret = _dos_allocmem(size >> 4, &sel)) != 0)    
        fail((ret == 8) ?
            "Insufficient memory" : 
            "_dos_allocmem - failure");
        
    printf("sel=%04X\n", sel);

    if (_dos_freemem(sel) != 0)
        fail("_dos_freemem - failure");
    
    return 0;
}
