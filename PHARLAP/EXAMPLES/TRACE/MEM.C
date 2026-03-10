/*
MEM.C
usage: mem [n]
requires at least one megabyte of memory

real mode:
    C:\DOS>cl -AL mem.c
    C:\DOS>mem

protected mode:
    C:\DOS>cl -AL -Lp mem.c
    C:\DOS>run286 mem
*/

#include <stdlib.h>
#include <stdio.h>

void fail(char *s) 
{
    puts(s);
    putchar(7);    /* beep */
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
    
    printf("Allocating %u megabyte%s... ", 
        meg, (meg > 1) ? "s" : "");
    
    maxallocs = ((long) meg) << 10; /* number of 1k blocks required */
    for (allocs = 0; allocs < maxallocs; allocs++)
       if (p = malloc(1024))    /* in 1k blocks */
       {
           *p = 'x';            /* do something, anything with */
           p[1023] = 'y';       /*   the allocated memory      */
       }
       else
       {
           printf("Only %lu bytes available\n", allocs << 10);
           fail("Insufficient memory!");
       }

   puts("ok");
   return 0;
}

