
/* wre2pr.c */

/* programacion de memoria nm93cs46 con cadena de caracteres */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>

#define EEPROM 0x28d

void outData( unsigned char dtoee);
void codeProg(void);
void codeWrite(void);
void outProgData( unsigned char dtoeeProg);

unsigned char j,i,EvalBit,CodeWriteIni[] = {0x20,0xa8,0xe8,0xc8,0xe8};
unsigned char Write,CodeWriteProg[] = {0x20,0xb8,0xf8,0xd8,0xf8};
char Address,CodeToWrite[128];
char CodeIn[128]={"JEAM"};

union  {
    struct
    {
unsigned  NU:
        3;
unsigned  PE:
        1;
unsigned  PRE:
        1;
unsigned  SK:
        1;
unsigned  DI:
        1;
unsigned  CS:
        1;
    }
    BitDato ;
    unsigned IntDato;
} Dato;





void main(void)
{
    int n;

    clrscr();

    /*	printf("Serial = ");*/

    /*	gets ( CodeIn );*/

    n = strlen(CodeIn);
    if (n%2)
        n++;

    swab( CodeIn , CodeToWrite, n);


    codeWrite();
    Write = 0x30;
    outProgData(Write);


    codeProg();
    Write = 0x30;
    outProgData(Write);

    codeProg();
    Write = 0xff;
    outProgData(Write);



    j = 0;
    Address = 0x40;

    for ( j=0 ;  j <= strlen(CodeIn)  ; j++)
    {
        codeWrite();
        Write = Address;
        outData(Write);
        Write = CodeToWrite [ j ];
        outData(Write);

        j++;

        Write =  CodeToWrite [ j ];
        outProgData(Write);

        Address = Address++;

    }



    /*
    				codeWrite();
    				Write = 0x10;       //Llena toda la memoria
    				outData(Write);     // con   0 x ff ff
    				Write = 0xff;
    				outData(Write);
    				Write =  0xff;
    				outProgData(Write);
    */



    codeWrite();
    Write = 0x00;
    outProgData(Write);

}








void codeWrite(void)
{
    for (i=0;i<5;i++)
    {
        Dato.IntDato = CodeWriteIni[ i ] ;
        outportb ( EEPROM , Dato.IntDato );
        delay(10);
    }
}



void codeProg(void)
{
    for (i=0;i<5;i++)
    {
        Dato.IntDato = CodeWriteProg[ i ] ;
        outportb ( EEPROM , Dato.IntDato );
        delay(10);
    }
}



void outData( unsigned char dtoee)
{
    unsigned char auxiliar;
    auxiliar = dtoee;

    EvalBit=0x80;
    for ( i=0 ; i<8 ; i++ )
    {
        EvalBit = EvalBit & auxiliar;
        if ( EvalBit == 0x0 ) Dato.BitDato.DI = 0x0;
        else Dato.BitDato.DI = 0x1;

        outportb ( EEPROM , Dato.IntDato );
        delay(10);

        EvalBit=0x80;
        EvalBit = EvalBit >> i+1;

        Dato.BitDato.SK = 0x0;
        outportb ( EEPROM , Dato.IntDato );
        delay(10);

        Dato.BitDato.SK = 0x1;
        outportb ( EEPROM , Dato.IntDato );
        delay(10);

    }
}




void outProgData( unsigned char dtoeeProg)
{
    unsigned char auxiliar;
    auxiliar = dtoeeProg;

    EvalBit=0x80;
    for ( i=0 ; i<8 ; i++ )
    {
        EvalBit = EvalBit & auxiliar;
        if ( EvalBit == 0x0 ) Dato.BitDato.DI = 0x0;
        else Dato.BitDato.DI = 0x1;

        outportb ( EEPROM , Dato.IntDato );
        delay(10);

        EvalBit=0x80;
        EvalBit = EvalBit >> i+1;

        Dato.BitDato.SK = 0x0;
        outportb ( EEPROM , Dato.IntDato );
        delay(10);

        Dato.BitDato.SK = 0x1;
        outportb ( EEPROM , Dato.IntDato );
        delay(10);

    }
    Dato.BitDato.PE = 0x1;
    Dato.BitDato.CS = 0x0;
    Dato.BitDato.PRE = 0x0;
    Dato.BitDato.DI = 0x1;

    outportb ( EEPROM , Dato.IntDato );

    delay(20);
}

