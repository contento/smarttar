/* 
TRYDLL.C -- test driver for TESTDLL.DLL
cl -Lp trydll.c -link testdll.lib
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "testdll.h"

void fail(char *s) { puts(s); exit(1); }

#define MAGIC_STRING        "hello world"
#define MAGIC_NUMBER        1234

main()
{
    char buf[128];
    put(MAGIC_STRING, MAGIC_NUMBER);
    if (get_num() != MAGIC_NUMBER)
        fail("get_num fail");
    printf("%Fs\n", get_str(buf));  /* %Fs prints Far string */
    if (strcmp(buf, MAGIC_STRING) != 0)
        fail("get_str fail");
    puts("ok");
    return 0;
}

