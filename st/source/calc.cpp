//
// [ CALC.CPP ]
//

#include "stdst.h"

#include <classlib\stacks.h>
#include <calc.h>

//
//	From Ellis Horowitz, "Data structures", Chapter 3, Page 90.
//
static const char MARK = '#';

double CALC::Evaluate(const char *infix)
{
    TOKEN_LIST postFix;
    if (!InfixToPostfix(infix, postFix))
        return 0;
    BI_StackAsList<TOKEN> stack;
    TOKEN token;
    double leftOperand, rightOperand;
    TOKEN_LIST_ITERATOR iterator(postFix);
    while (iterator)
    {
        token = iterator.current();
        if (token.Tag == TOKEN::OPERAND)
        {
            stack.push(token);
        }
        else
        {
            if (token.Operator != MARK)
            {
                // first the second Operand (no conmutative)
                rightOperand = stack.pop().Operand;
                leftOperand = stack.pop().Operand;
                token.Operand = BiCompute(token.Operator, leftOperand, rightOperand);
                token.Tag = TOKEN::OPERAND;
                stack.push(token);
            } // else: is the mark
        }


        iterator++;
    }
    return stack.pop().Operand;
}

BOOL CALC::IsValidInfix(const char *infix)
{
    if (infix == NULL)
        return FALSE;
    SHORT len = strlen(infix);
    if (!len)
        return FALSE;
    const char *VALID_CHARS = "0.123456789()+-*/ \t^";
    for (SHORT i=0; i < len; i++)
        if (!strchr(VALID_CHARS, infix[i]))
            return FALSE;
    return TRUE;
}

BOOL CALC::InfixToPostfix(const char *infix, TOKEN_LIST& pPostfix)
{
    if (!IsValidInfix(infix))
        return FALSE;
    STR128 tmpInfix;
    strcpy(tmpInfix, infix);
    _DelSpaces(tmpInfix);
    // add mark of infix expression
    SHORT len = strlen(tmpInfix);
    tmpInfix[len] = MARK;
    tmpInfix[len+1] = '\0';
    //
    BI_StackAsList<TOKEN> stack;
    // add mark onto stack
    TOKEN nextToken, token;
    token.Operator = MARK;
    token.Tag = TOKEN::OPERATOR;
    stack.push(token);
    SHORT i=0;
    for (;;)
    {
        nextToken = GetToken(tmpInfix, i);
        if (nextToken.Tag == TOKEN::OPERATOR)
        {
            switch(nextToken.Operator)
            {
            case MARK:
                while (!stack.isEmpty())
                { // clean stack
                    token = stack.pop();
                    // check for remaining right parenthesis in Stack
                    if (token.Tag == TOKEN::OPERATOR && token.Operator == '(')
                        return FALSE;
                    pPostfix.addAtTail(token);
                }
                return TRUE;
            case ')' :
                // pop operators and store in postfix until '('
                token = stack.pop();
                while (token.Operator != '(')
                {
                    pPostfix.addAtTail(token);
                    token = stack.pop();
                    // check for missing left parenthesis
                    if (token.Tag == TOKEN::OPERATOR && token.Operator == MARK)
                        return FALSE;
                }
                break;
            default: // operator or '('
                // pop operators with equal or major precedence
                for (;;)
                {
                    token = stack.pop();
                    if (ISP(token.Operator) >= ICP(nextToken.Operator))
                        pPostfix.addAtTail(token);
                    else
                    {
                        stack.push(token); // adjust Stack
                        break;
                    }
                }
                // now push operator
                stack.push(nextToken);
                break;
            }
        }
        else  // it is an operand
            pPostfix.addAtTail(nextToken);
    }
}

double CALC::BiCompute(char op, double leftOperand, double rightOperand)
{
    switch (op)
    {
    case '^':
        return (pow(leftOperand,rightOperand));
    case '+':
        return (leftOperand+rightOperand);
    case '-':
        return (leftOperand-rightOperand);
    case '*':
        return (leftOperand*rightOperand);
    case '/':
        return (rightOperand)?(leftOperand/rightOperand):rightOperand;
    }
    return 0;
}

CALC::TOKEN CALC::GetToken(char *infix, SHORT& infixPos)
{
    TOKEN token;
    switch (infix[infixPos])
    {
        // Operand
    case '+' :
    case '-' :
    case '*' :
    case '/' :
    case '^' :
    case '(' :
    case ')' :
    case MARK:
        {
            token.Tag = TOKEN::OPERATOR;
            token.Operator = infix[infixPos];
            break;
        }
        // digit
    case '.':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        {
            STR128 s;
            SHORT digitNum = infixPos+1; // search for a number of several digits
            while (isdigit(infix[digitNum]) || infix[digitNum] == '.')
                digitNum++;
            memcpy(s, &infix[infixPos], digitNum-infixPos); // extract number
            s[digitNum-infixPos] = '\0';						// set end of string
            token.Operand = atof(s);
            token.Tag = TOKEN::OPERAND;
            infixPos = digitNum-1; // jump over the number
            break;
        }
    }
    infixPos++;
    return (token);
}

SHORT CALC::ISP(char op)
{
    switch(op)
    {
    case '^' :
        return 3;
    case '*' :
    case '/' :
        return 2;
    case '+' :
    case '-' :
        return 1;
    case '(' :
        return 0;
    case MARK:
        return -1;
    default  :
        return -2; //  !!!
    }


}

SHORT CALC::ICP(char op)
{
    switch(op)
    {
    case '(' :
        return 4;
    case '^' :
        return 4;
    case '*' :
    case '/' :
        return 2;
    case '+' :
    case '-' :
        return 1;
    case MARK:
        return -1;
    default  :
        return -2; // !!!
    }


}
