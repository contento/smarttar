/*
CALLFUNC.C -- run-time dynamic linking from the command-line
callfunc <dll name> <function name or ordinal number> [args...] [%mask] [!]

The optional %mask arg is a printf() mask for printing out the return value
The optional ! arg is for specifying cdecl function

Microsoft C 6.0:
cl -c -AL -Gs2 -Zpe -Lp -D_MT callfunc.c
link c:\c600\lib\crtexe callfunc,callfunc,,/nod crtlib.lib os2.lib;

Microsoft C 5.1:
cl -c -AL -Gs2 -Zpe -Lp -D_MT -DMT callfunc.c
link c:\c51\lib\crtexe callfunc,callfunc,,/nod crtlib.lib doscalls.lib;
*/

#ifdef _MSC_VER
#pragma message("Compiling with Microsoft C 6.0 or higher")
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#else
#ifdef MT
#pragma message("Compiling with Microsoft C 5.1 -- MT defined")
#include <mt\stdlib.h>
#include <mt\stdio.h>
#include <mt\string.h>
#else
#pragma message("Compiling with Microsoft C 5.1")
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif
#endif
#include <phapi.h>

typedef unsigned (far *FN)();
typedef char far * (far *STRFN)();
typedef char (far *BYTEFN)();
typedef unsigned (far *WORDFN)();
typedef unsigned long (far *LONGFN)();
typedef double (far pascal *FLOATFN)();

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif

void fail(char *msg) { puts(msg); exit(1); }

unsigned loadmod(char far *modname)
{
    unsigned handle;
    return DosLoadModule(NULL, 0, modname, &handle) ? 0 : handle;
}

void far *procaddr(unsigned modhand, char far *funcname)
{
    void far *f;
    /* DosGetProcAddr expects a pointer to a function pointer */
    return DosGetProcAddr(modhand, funcname, &f) ? 0 : f;
}

unsigned modhandle(char far *modname)
{
    unsigned handle;
        /* If DosGetModHandle() returns non-zero (failure), 
           only then do we call DosLoadModule() (via loadmod()) */
    return DosGetModHandle(modname, &handle) ? loadmod(modname) : handle;
}

typedef enum { typ_string, typ_byte, typ_word, typ_long, typ_float } TYPE;

TYPE type(char *arg);
TYPE retval_type(char *s);

/* push(): a trick that relies on pascal calling convention */
void pascal push() { }

#define PUSH_ARG(arg)   \
{   \
    switch (type(arg))  \
    {   \
        case typ_string:    push(arg);          c += 2; break;  \
        case typ_byte:      push(arg[1]);       c += 1; break;  \
        case typ_word:      push(atoi(arg));    c += 1; break;  \
        case typ_long:      push(atol(arg));    c += 2; break;  \
        case typ_float:     push(atof(arg));    c += 4; break;  \
    }   \
}

main(int argc, char *argv[])
{
    FN f;
    TYPE retval_typ = typ_word;
    char *mask = "%u";
    unsigned module;
    BOOL is_cdecl = FALSE;
    int i, c;
    
    if (argc < 3)
        fail("usage: callfunc <dllname> <func name> [args...] [%mask] [!]");

    /* see if cdecl */
    if (argv[argc-1][0] == '!')
    {
        is_cdecl = TRUE;
        argc--;
    }

    /* handle optional printf mask */
    if (strchr(argv[argc-1], '%'))
        retval_typ = retval_type(mask = argv[--argc]);
    
    if ((module = modhandle(argv[1])) == 0)
        fail("can't load dll");

    /* pass ASCIIZ string or ordinal number */
    f = procaddr(module, isdigit(argv[2][0]) ? atol(argv[2]) : argv[2]);
    if (! f)
        fail("can't find function");

    /* push in reverse order for cdecl */
    if (is_cdecl)
    {
        for (i=argc-1, c=0; i>=3; i--)
            PUSH_ARG(argv[i]);
    }
    else
    {
        for (i=3; i<argc; i++)
            PUSH_ARG(argv[i]);
    }

    /* args are on the stack : call (*f)() and print retval */
    switch (retval_typ)
    {
        case typ_string: printf(mask, ((STRFN) f)()); break;
        case typ_byte:   printf(mask, ((BYTEFN) f)()); break;
        case typ_word:   printf(mask, f()); break;
        case typ_long:   printf(mask, ((LONGFN) f)()); break;
        case typ_float:  printf(mask, ((FLOATFN) f)()); break;
    }
    
    printf("\n");
    return 0;
}

/*
    type() uses some dumb rules to determine the type of an argument:
        if first character of arg is a digit or '-'
            and if arg contains '.' then it's a floating-point number
            else if last character is an 'L' then it's a long
            else it's a unsigned word
        else if first character is an apostrophe
            it's a single-byte character
        otherwise
            it's a string 
*/          
TYPE type(char *arg)
{
    if (isdigit(arg[0]) || (arg[0] == '-' && isdigit(arg[1])))
    {
        char *p = arg;
        while (*p)
            if (*p++ == '.') 
                return typ_float;
        return (*--p == 'L') ? typ_long : typ_word;
    }
    else
        return (arg[0] == '\'') ? typ_byte : typ_string;
}

/*
    retval_type() uses a printf() mask (e.g., %s or %lX) to determine
    type of return value
*/
TYPE retval_type(char *s)
{
    while (*s)
    {
        switch (*s)
        {
            case 's' :  return typ_string; break;
            case 'c' :  return typ_byte; break;
            case 'p' : case 'l' : case 'I' : case 'O' : case 'U' :
                        return typ_long; break;
            case 'e' : case 'E' : case 'f' : case 'g' : case 'G' :
                        return typ_float; break;
        }
        s++;
    }

    /* still here */
    return typ_word;
}

