#ifndef __PARSER_H
#define __PARSER_H

#if !defined(__DLISTIMP_H)
#include <classlib\dlistimp.h>
#endif

#if (__BORLANDC__ > 0x410) // > Borland 3.1
#if !defined(__CSTRING_H)
#include <cstring.h>
#endif
#else
#if !defined(__STRNG_H)
#include <classlib\strng.h>
#endif
#endif

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

class Parser
{
public:
#if (__BORLANDC__ > 0x410)
    typedef TDoubleListImp<string> Tokens;
    typedef TDoubleListIteratorImp<string> Iterator;
#else
    typedef BI_DoubleListImp<String> Tokens;
    typedef BI_DoubleListIteratorImp<String> Iterator;
#endif
    Parser(const char *line, Tokens& tokens);
};

#endif // __PARSER_H
