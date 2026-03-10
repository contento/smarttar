/*
TESTDLL.C -- sample dynamic link library for 286|DOS-Extender

cl -ALu -FPa -Gs2 -Lp -c testdll.c 
link /nod/noi testdll, testdll.dll,,,testdll.def
*/

#ifndef _MSC_VER
/* for Microsoft C 5.1 */
int _acrtused = 0;
#endif

static unsigned x = 0;
static char buf[80];

void far pascal put(char far *s, unsigned y)
{
    char *p = buf;
    while (*s)
        *p++ = *s++;
    *p = '\0';
    
    x = y;
}

unsigned far get_num(void)
{
    return x;
}

void far get_str(char far *s)
{
    char *p = buf;
    while (*p)
        *s++ = *p++;
    *s = '\0';
}

