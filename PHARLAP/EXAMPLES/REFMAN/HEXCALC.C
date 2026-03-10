/*
HEXCALC.C -- demonstrates DosCreateDSAlias() in 286|DOS-Extender,
based on Charles Petzold's demo of self-modifying code from _PC
Magazine DOS Power Tools_ (pp. 1013-1017).

    bcc286 hexcalc.c
    hexcalc

sample output:      
    > + 1 2
    0003
    > - 1 2
    FFFF
    > = 1 2
    0002
    > q

*/


#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <phapi.h>

#ifdef _MSC_VER

#define ADD         0x03
#define SUB         0x2B
#define XOR         0x33
#define OR          0x0B
#define AND         0x23
#define MOV         0x8B

#define MODRM_AXCX  0xC1

#else

#define ADD         0x01
#define SUB         0x29
#define XOR         0x31
#define OR          0x09
#define AND         0x21
#define MOV         0x89

#define MODRM_AXCX  0xC8

#endif

unsigned calc(unsigned op1, unsigned op2)
{
    _asm mov ax, op1
    _asm mov cx, op2
 // this code will be modified!
    _asm add ax, cx    
}

void fail(char *s) { puts(s); exit(1); }

main()
{
    BYTE far *code;
    unsigned op, op1, op2, dmy;

    // find offset of ADD AX, CX instruction to poke
    // note that there is no problem peeking code in protected mode...
    for (code = (BYTE far *) calc;
         (code < (BYTE far *) main) &&
         (code[0] != ADD) || (code[1] != MODRM_AXCX);
         code++)
             ; 
    if (code == (BYTE far *) main)
        fail("couldn't locate ADD AX, CX");
         
    // ... but you can't _poke_ code in protected mode, so get alias
{
    SEL data;

    if (DosCreateDSAlias(SELECTOROF(code), &data) != 0)
        fail("DosCreateDSAlias - failure");
    code = MAKEP(data, OFFSETOF(code));
}

    // calculator
    for (;;)
    {
        printf("> ");
        if ((op = getchar()) == 'q' || (op == 'Q'))
            break;
        scanf(" %x %x", &op1, &op2);
        dmy = getchar();
        
        // now modify code, based on user's command
        switch (op)
        {
            case '+' : *code = ADD; break;
            case '&' : *code = AND; break;
            case '=' : *code = MOV; break;
            case '|' : *code = OR;  break;
            case '-' : *code = SUB; break;
            case '^' : *code = XOR; break;
	    default:
		printf("Unknown operator -- %c\n", op);
		printf("Valid operators are: + - & | ^ =\n");
		continue;
        }
        /* calling calc() will clear the instruction prefetch queue */
        printf("%04X\n", calc(op1, op2));
    }
    
    if (DosFreeSeg(SELECTOROF(code)) != 0)
        fail("DosFreeSeg - failure");

    return 0;
}
