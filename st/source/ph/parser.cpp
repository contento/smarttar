//
// [ PARSER.CPP ]
//

#include <classlib\stacks.h>
#include <parser.h>

#ifdef DOSX286
#include <phapi.h>
#endif

Parser::Parser(const char *line, Tokens& tokens)
{
    WORD iChar;
#if (__BORLANDC__ > 0x410) // > borland C++ 3.1
    TStackAsList<char> stack;
#else
    BI_StackAsList<char> stack;
#endif
    STR512 tmpLine;
    for (WORD i=0; i<strlen(line)+1; i++)
    { // include NULL character
        switch (line[i])
        {
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '^':
        case '&':
        case '|':
        case '~':
        case '=':
        case ':':
        case ',':
        case '.':
        case ';':
        case '(':
        case ')':
        case '[':
        case ']':
        case ' ':
        case '\t':
        case '\0':
            iChar=0;
            // pop objects
#if (__BORLANDC__ > 0x410) // > borland C++ 3.1
            while (!stack.IsEmpty())
                tmpLine[iChar++] = stack.Pop();
#else
            while (!stack.isEmpty())
                tmpLine[iChar++] = stack.pop();
#endif
            // store objects in stack
            if (iChar)
            {
                tmpLine[iChar] = NULL;
#if (__BORLANDC__ > 0x410) // > borland C++ 3.1
                tokens.AddAtTail(string(strrev(tmpLine)));
#else
                tokens.addAtTail(String(strrev(tmpLine)));
#endif
            }
            // store current object
            if (line[i])
            { // avoid NULL
                tmpLine[0] = line[i];
                tmpLine[1] = '\0';
#if (__BORLANDC__ > 0x410) // > borland C++ 3.1
                tokens.AddAtTail(string(tmpLine));
#else
                tokens.addAtTail(String(tmpLine));
#endif
            }
            break;
        default:
#if (__BORLANDC__ > 0x410) // > borland C++ 3.1
            stack.Push(line[i]);
#else
stack.push(line[i]);
#endif
        }
    }
}

