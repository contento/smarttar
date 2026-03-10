
#include <stdio.h>
#include <stdlib.h>

/*
   Globals
*/

char Msg[] = "This line written by DLL_2.DLL\n";

/*
   These routines are called from MAIN.EXE
*/

/* CloseTest: write a line into the file and close it */

int _export _loadds CloseTest(FILE *Stream)
{
    int retcode;

    printf("DLL_2: Entered CloseTest routine\n");

    printf("DLL_2: Writing into file\n");

    if (fwrite(Msg, 1, sizeof(Msg)-1 , Stream) != sizeof(Msg)-1)
    {
	/* error */
	goto err;
    }

    printf("DLL_2: Closing file\n");

    if (fclose(Stream))
    {
	/* error */
	goto err;
    }

    return 1;

err:
    printf("DLL_2: Error in CloseTest()\n");
    return 0;
}

/* FreeTest: free the memory */

void _export _loadds FreeTest(char *pMem)
{
    printf("DLL_2: Entered FreeTest routine\n");
    printf("DLL_2: Freeing buffer\n");
    free(pMem);
}
