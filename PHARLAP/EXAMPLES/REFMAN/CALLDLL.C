/*
CALLDLL.C -- run-time dynamic linking in 286|DOS-Extender

cl -Lp calldll.c

Requires TESTDLL.DLL (see TESTDLL.C and TESTDLL.DEF)
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

void fail(char *msg)    { puts(msg); exit(1); }

main(int argc, char *argv[])
{
    char buf[80];
    void (far pascal *put)(char far *s, unsigned y);
    unsigned (far *get_num)(void);
    char far * (far *get_str)(char far *s);
    unsigned testdll;

    /* get a module handle, using the DLL's ASCII name */
    if (DosLoadModule(NULL, 0, "TESTDLL.DLL", &testdll) != 0)
        fail("This program requires TESTDLL.DLL");

    /* now get function pointers, using the ASCII names of
       the functions in the DLL. Note that functions using the
       Pascal calling convention (such as put()) generally
       have names in CAPS, whereas cdecl functions (such as
       get_num()) usually have names in lower-case with a 
       leading underscore */
    DosGetProcAddr(testdll, "PUT", (PPFN) &put);
    DosGetProcAddr(testdll, "_get_num", (PPFN) &get_num);
    DosGetProcAddr(testdll, "_get_str", (PPFN) &get_str);
    
    /* call the functions in the DLL, via function pointers */ 
    (*put)("hello world", 1066);
    /* in ANSI C, the following is also legal */
    put("hello world", 1066);
    if ((*get_num)() != 1066)
        fail("get_num: failure");
    (*get_str)(buf);
    if (strcmp(buf, "hello world") != 0)
        fail("get_str: failure");

    /* NOTE: If the DLL you're run-time dynamically linking to has a 
       termination routine installed with DosExitList(), there are going
       to be problems. You can't call DosFreeModule(). Instead, the DLL
       should provide an application-callable cleanup routine */
           
    /* release the module handle
       (this also invalidates the function pointers) */
    /* This is okay here because the version of TESTDLL.DLL we're using
       doesn't have a DosExitList() termination routine */
    DosFreeModule(testdll);
    puts("ok");

    /* now do run-time dynamic linking to function in this EXE */
{
    unsigned mod;
    void (far *fooptr)(unsigned u);
    /* argv[0] is name.exe, so drop .exe */
    if (DosGetModHandle(strtok(argv[0], "."), &mod) != 0)
        fail("DosGetModHandle - failure");
    if (DosGetProcAddr(mod, "_foo", (PPFN) &fooptr) != 0)
        fail("DosGetProcAddr - failure");
    (*fooptr)(1234);
    /* show the alternate ways to specify func, using ordinal number */
    DosGetProcAddr(mod, "#1", (PPFN) &fooptr);
    (*fooptr)(1234);
    DosGetProcAddr(mod, MAKEP(0, 1), (PPFN) &fooptr);
    (*fooptr)(1234);
}

    return 0;
}

void _far _export foo(unsigned u)
{
    printf("this is foo: %u\n", u);
}
