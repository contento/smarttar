/*
CODDAT.C -- demonstrates DosSetSegAttrib() in 286|DOS-Extender
    See also CODEDATA.C, which does the same thing using 
    DosCreateCSAlias().

C:\>cl -Lp coddat.c
C:\>run286 coddat
This is a test of executable data

NOTE: Very slow when run under Windows 3.0 enhanced mode!
*/

#include <phapi.h>

unsigned char far data[] = {
/*  op    ?l    ?h   */ 
    0xb8, 0x00, 0x00,       /* MOV AX, 0 */
    0xbb, 0x00, 0x00,       /* MOV BX, 0 */
    0xb9, 0x00, 0x00,       /* MOV CX, 0 */
    0xba, 0x00, 0x00,       /* MOV DX, 0 */
    0xcd, 0x00,             /* INT 00 */
    0xcb,                   /* RETF */
    } ;

void (far *code)(void) = (void (far *)()) data;

void fail(char *s) { puts(s); exit(1); }

void put(char *s)
{
    enum { 
        AL=1, AH=2, BL=4, BH=5, CL=7, CH=8, DL=10, DH=11, INTNO=13
        } OFFSETS;

    while (*s)
    {
        data[AH] = 2;           /* AH=2h */
        data[INTNO] = 0x21;     /* INT 21h */
        data[DL] = *s;          /* DL=char */
        /* temporarily make it a code segment */
        if (DosSetSegAttrib(SELECTOROF(code), CODE16) != 0)
            fail("DosSetSegAttrib fail -- CODE16");
        (*code)();
        /* make it back into a data segment */
        if (DosSetSegAttrib(SELECTOROF(code), DATA16) != 0)
            fail("DosSetSegAttrib fail -- DATA16");
        s++;
    }
}

main()
{
    put("This is a test of executable data\r\n");
    return 0;
}

