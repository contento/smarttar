/*
CODEDATA.C -- demonstrates DosCreateCSAlias() in 286|DOS-Extender

real mode:
    C:\>cl codedata.c
    C:\>codedata
    This is a test of executable data
    
protected mode:
    C:\>cl -Lp -DDOSX286 codedata.c
    C:\>run286 codedata
    This is a test of executable data
        
Produces mildly amusing results if you redirect output:
    C:\>run286 codedata > tmp.tmp
    C:\>type tmp.tmp
*/

#ifdef DOSX286
#include <phapi.h>
#endif

typedef unsigned char BYTE;

BYTE data[] = {
/*  op    ?l    ?h   */ 
    0xb8, 0x00, 0x00,       /* MOV AX, 0 */
    0xbb, 0x00, 0x00,       /* MOV BX, 0 */
    0xb9, 0x00, 0x00,       /* MOV CX, 0 */
    0xba, 0x00, 0x00,       /* MOV DX, 0 */
    0xcd, 0x00,             /* INT 00 */
    0xcb,                   /* RETF */
    } ;

void (far *code)(void);

void put(char *s)
{
    enum { 
        AL=1, AH=2, BL=4, BH=5, CL=7, CH=8, DL=10, DH=11, INTNO=13
        } offsets;

    while (*s)
    {
        data[AH] = 2;           /* AH=2h */
        data[INTNO] = 0x21;     /* INT 21h */
        data[DL] = *s;          /* DL=char */
        (*code)();              
        s++;
        
        if (! *s) break;
        data[AH] = 0x0E;        /* AH=0Eh */
        data[AL] = *s;          /* AL=char */
        data[BH] = 0;           /* BH=page */
        data[INTNO] = 0x10;     /* INT 10h */
        (*code)();
        s++;
    }
}

main()
{
#ifdef DOSX286
    void far *fp;
    USHORT ret;
    SEL cs;
    
    fp = (void far *) data;
    if (ret = DosCreateCSAlias(SELECTOROF(fp), &cs))
        return ret;
    code = MAKEP(cs, OFFSETOF(fp));
    put("This is a test of executable data\r\n");
    return DosFreeSeg(cs);
#else
    code = (void (far *)()) data;
    put("This is a test of executable data\r\n");
    return 0;
#endif  
}
