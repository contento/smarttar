/*
MEM2.C -- uses WPRINTF
usage: mem2 [n]
requires at least one megabyte of memory

real mode:
    cl -AL -Ox -Gs2 mem2.c wprintf.c
        
protected mode:
    cl -AL -Ox -Gs2 -Lp -DDOSX286 mem2.c wprintf.c -link llibpe.lib
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <bios.h>
#include "wprintf.h"

void fail(char *s)  /* change to dialog box */
{
    wprintf(5, 10, REVERSE, "%s", s);
    wprintf(6, 10, REVERSE, "Press any key to continue...");
    putchar(7);          /* beep */
    _bios_keybrd(_KEYBRD_READ);
    exit(1);
}

main(int argc, char *argv[])
{
    char *p;
    unsigned meg;               
    unsigned long maxallocs;         
    unsigned long allocs;

    if (argc < 2)                   
        meg = 1;                /* default 1 megabyte required */
    else 
        meg = atoi(argv[1]);    /* command-line option: n megabytes */
    maxallocs = ((long) meg) << 10; /* number of 1k blocks required */

    video_init();
    clear(0, 0, 24, 79, REVERSE);
    
    wprintf(4, 10, REVERSE, "Allocating %u megabyte%s... ", 
        meg, (meg > 1) ? "s" : "");
    
    for (allocs = 0; allocs < maxallocs; allocs++)
    {
       p = malloc(1024);    /* in 1k blocks */
       if (p)    	    
       {
           *p = 'x';            /* do something, anything with */
           p[1023] = 'y';       /*   the allocated memory      */
       }
       else
       {
           wprintf(4, 36, REVERSE, "Only %lu bytes available", 
               allocs << 10);
           fail("Insufficient memory!");
       }
   }
   wprintf(4, 36, REVERSE, "done");
   wprintf(5, 10, REVERSE, "Press any key to continue...");
   _bios_keybrd(_KEYBRD_READ);
   return 0;
}

