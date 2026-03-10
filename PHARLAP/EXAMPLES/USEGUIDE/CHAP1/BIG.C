/* 
BIG.C
bcc286 -f- -G -O big.c
*/

#include <stdlib.h>
#include <stdio.h>

#define SIZE    512

static long huge array[SIZE][SIZE] ;  /* one-megabyte array */

main()
{
    int i, j;

    printf("Using %lu-byte array\n", 
        (long) SIZE * SIZE * sizeof(long));

    for (i=0; i<SIZE; i++)
    {
        for (j=0; j<SIZE; j++)
            array[i][j] = (long) i * j; /* touch every element */
        printf("%d\r", i);              /* display odometer */
    }

    printf("done\n");
    return 0;
}

