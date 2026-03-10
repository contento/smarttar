
#include <stdio.h>
#include <stdlib.h>

/*
   Need this to prevent the Microsoft emulator from being brought in
*/

int _fltused = 1;

/*
   Globals
*/

char Msg[] = "This line written by DLL_1.DLL\n";

/*
   These routines are called from from MAIN.EXE
*/

/* OpenTest: open a file and write a line into it */

FILE * _export _loadds OpenTest(char *pFilename)
{
    FILE *retcode;

    printf("DLL_1: Entered OpenTest routine\n");

    printf("DLL_1: Opening file '%s'\n", pFilename);

    if (!(retcode = fopen(pFilename, "w+")))
    {
	/* error */
	goto err;
    }

    printf("DLL_1: Writing into file\n");

    if (fwrite(Msg, 1, sizeof(Msg)-1 , retcode) != sizeof(Msg)-1)
    {
	/* error */
	goto err;
    }

    /* all done ... return */

    return retcode;

err:
    printf("DLL_1: Error in OpenTest()\n");
    return NULL;
}

/* SquareRoot: calculate the square root of a number */

float _export _loadds _pascal SquareRoot(float Num)
{
    float retcode, diff, delta;

    /* compute square root by brute force */

    printf("DLL_1: The approx square root of %6.3f is ", Num);

    retcode = delta = Num / 2.0;

    while (1)
    {
	diff = Num - retcode * retcode;

	if ((diff <= 0.0001) && (diff >= -0.0001))
	{
	    /* good enough */
	    return retcode;
	}

	if (diff > 0) retcode += delta;
	else retcode -= delta;

	delta /= 2.0;
    }
}

/* AllocTest: allocate some memory and fill it with pString */

char * _export _loadds AllocTest(char *pString)
{
    char *retcode;
    int SizeLeft, Index, i;

    printf("DLL_1: Entered AllocTest routine\n");

    printf("DLL_1: Allocating 160 bytes for string\n");

    if (!(retcode = malloc(161)))   // 1 extra for terminating 0
    {
	/* error */
	goto done;
    }

    printf("DLL_1: Replicating string into buffer\n");

    for (i = 0, Index = 0 ; i < 161 ; i++)
    {
	*(retcode + i) = *(pString + Index);
	Index ++;
	if (!*(pString + Index)) Index = 0;
    }

    retcode[160] = 0;		// insert terminating 0

done:

    return retcode;
}
