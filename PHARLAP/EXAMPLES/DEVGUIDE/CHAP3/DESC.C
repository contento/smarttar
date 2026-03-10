/* 
DESC.C
cl -Lp desc.c
bcc286 desc.c
run286 desc
*/

#include <stdlib.h>
#include <stdio.h>
#include <phapi.h>

char *segtype(unsigned attrib)
{
    switch (attrib)
    {
        case CODE16:         return "code";
        case DATA16:         return "data";
        case CODE16_NOREAD:  return "exec-only code";
        case DATA16_NOWRITE: return "read-only data";
        default:             return "";
    }
}

main()
{
    DESC desc;
    SEL i;
    for (i=0; i<0xFFFF; i++) /* for all possible selectors */
    {
        if (DosGetSegDesc(i, &desc) == 0) /* if valid selector */
            printf("%04x base=%08lx size=%08lx %s\n",
                i, desc.base, desc.size, segtype(desc.attrib));
        if ((i % 0x1000) == 0)
            putchar('.'); /* just give indication that something is happening */
    }
    return 0;
}

