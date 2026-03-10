
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/*
   Routines that live in DLLs are declared 'extern'
*/

extern FILE *OpenTest(char *pFilename);
extern int CloseTest(FILE *File);
extern char *AllocTest(char *pString);
extern void FreeTest(char *pMem);

// Note: due to a bug in the Microsoft compiler, all functions that
// return floating point numbers from a DLL must use the _pascal
// calling convention.

extern float _pascal SquareRoot(float Num);

/*
   Globals
*/

char Msg[] = "This line written by MAIN.EXE\n";

void main()
{
    FILE *File;
    int i;
    char *pMem;
    float SumFloat = 0.0, TmpFloat = 0.0;

    /*
       Do some file I/O
    */

    printf("MAIN: Main routine entered\n");

    printf("MAIN: Calling DLL_1 to open file 'testfile'\n");

    if (!(File = OpenTest("testfile")))
    {
	/* error */
	perror("Could not open 'testfile'");
	exit(1);
    }

    printf("MAIN: Writing into file\n");

    if (fwrite(Msg, 1, sizeof(Msg)-1, File) != sizeof(Msg)-1)
    {
	perror("Error writing 'testfile'");
	exit(1);
    }

    printf("MAIN: Calling DLL_2 to close and print file 'testfile'\n");

    if (!CloseTest(File))
    {
	/* error */
	perror("Could not close 'testfile'");
	exit(1);
    }

    /*
       Do some floating point
    */

    printf("MAIN: Computing approximated square root of first "
           "10 natural numbers\n");

    for (i = 1 ; i < 11 ; i++)
    {
	/* in this loop, we are doing floating point in both
	   DLL_1.DLL and here */

    	printf("%6.3f\n", TmpFloat = SquareRoot(i));
	SumFloat += TmpFloat;
    }

    printf("MAIN: Sum of numbers above: %6.3f\n", SumFloat);

    /*
       Do some memory allocation
    */

    if (!(pMem = AllocTest("This is only a test ... ")))
    {
	/* error */
	perror("Could not allocate memory");
	exit(1);
    }

    printf("MAIN: Buffer contains: '%s'\n", pMem);

    FreeTest(pMem);

    printf("MAIN: All done ... bye!\n");
    exit(0);
}

/*
   Stub routines for runtime functions:

   These stub routines have to be set up for each of the runtime functions
   that are called from the DLL. Their job is simply to load DS with
   the correct value (the '_loadds' keyword does this) and call through
   to the 'real' function.
*/

int __cdecl _export _loadds main_printf(const char *Fmt, ...)
{
    int rc;
    va_list Marker;
    va_start(Marker, Fmt);
    rc = vprintf(Fmt,Marker);
    va_end(Marker);
    return rc;
}

FILE * __cdecl _export _loadds main_fopen(const char *Filename,
					  const char *Mode)
{
    return fopen(Filename,Mode);
}



size_t __cdecl _export _loadds main_fwrite(const void *pBuffer,
					   size_t Size,
					   size_t Count,
					   FILE *Stream)
{
    return fwrite(pBuffer, Size, Count, Stream);
}

int __cdecl _export _loadds main_fclose(FILE *Stream)
{
    return fclose(Stream);
}

void * __cdecl _export _loadds main_malloc(size_t Size)
{
    return malloc(Size);
}

void __cdecl _export _loadds main_free(void *pMem)
{
    free(pMem);
    return;
}
