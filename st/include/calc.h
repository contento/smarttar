#ifndef __CALC__
#define __CALC__

#if !defined(__DLISTIMP_H)
#include <classlib\dlistimp.h>
#endif

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

class CALC
{
public:
    double Evaluate(const char *infix);
private:
    class TOKEN
    {
public:
        enum { OPERAND, OPERATOR } Tag;
        union {
            double Operand;
            char   Operator;
        };
        SHORT operator ==(const TOKEN & token) const
        {
            return (Operand == token.Operand);
        }
        SHORT operator  <(const TOKEN & token) const
        {
            return (Operand < token.Operand);
        }
    };
    typedef BI_DoubleListImp<TOKEN> 				TOKEN_LIST;
    typedef BI_DoubleListIteratorImp<TOKEN> TOKEN_LIST_ITERATOR;
    //
    BOOL InfixToPostfix(const char *infix, TOKEN_LIST& postfix);
    BOOL IsValidInfix  (const char *infix);
    TOKEN GetToken(char *infix, SHORT& infixPos);
    inline SHORT ISP(char op); // in-stack priority
    inline SHORT ICP(char op); // in-coming priority
    inline double BiCompute(char op, double leftOperand, double rightOperand);
};

#endif // __CALC__
