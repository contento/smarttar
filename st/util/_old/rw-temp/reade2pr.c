
/* reade2pr.c */

/* programacion de memoria eeprom 93cs46 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>

#define INEEPROM 0x28f
#define OUTEEPROM   0x28d

void outData( unsigned char dtoee);
void codeProg(void);
void codeWrite(void);
void outProgData( unsigned char dtoeeProg);

unsigned char j,i,EvalBit,CodeWriteIni[] = {0x20,0xa8,0xe8,0xc8,0xe8};
unsigned char Write,CodeWriteProg[] = {0x20,0xb8,0xf8,0xd8,0xf8};
unsigned char Address,CodeToWrite[128];
unsigned char CodeIn[] = "memoria eeprom serial nm93cs46 microdiseno ltda. " ;
unsigned char BitFromEeprom , DataAscii, DataAsciiStr[128], CodeFromEeprom[128];

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
    int n=0 ;
    clrscr();

    codeWrite();
    Write = 0x80;
    outData(Write);

    Dato.BitDato.SK = 0x1;
    outportb ( OUTEEPROM , Dato.IntDato );
    /*delay(10)*/
    ;


    Dato.BitDato.SK = 0x0;
    outportb ( OUTEEPROM , Dato.IntDato );
    /*delay(10)*/
    ;

    Dato.BitDato.SK = 0x1;
    outportb ( OUTEEPROM , Dato.IntDato );
    /*delay(10)*/
    ;

    DataAscii = 0xff;


    while ( DataAscii != 0x0 )
    {
        if (kbhit()) break;
        for( j=0 ; j<8 ; j++ )
        {
            if (kbhit()) break;

            Dato.BitDato.SK = 0x1;
            outportb ( OUTEEPROM , Dato.IntDato );
            /*delay(10)*/
            ;

            Dato.BitDato.SK = 0x0;
            outportb ( OUTEEPROM , Dato.IntDato );
            /*delay(10)*/
            ;

            BitFromEeprom = inportb(INEEPROM);
            BitFromEeprom = BitFromEeprom & 0x1;
            DataAscii = DataAscii << 1;
            DataAscii = DataAscii | BitFromEeprom;

            /*delay(10)*/
            ;

            Dato.BitDato.SK = 0x1;
            outportb ( OUTEEPROM , Dato.IntDato );
            /*delay(10)*/
            ;

        }
        if (kbhit()) break;

        DataAsciiStr [ n ] = DataAscii;
        n++;
    }

    Dato.BitDato.PE = 0x1;
    Dato.BitDato.CS = 0x0;
    Dato.BitDato.PRE = 0x0;
    Dato.BitDato.DI = 0x1;

    outportb ( OUTEEPROM , Dato.IntDato );

    swab( DataAsciiStr , CodeFromEeprom , strlen( DataAsciiStr ) );

    printf(" Lectura de Serial = %s\n", CodeFromEeprom );


}

void codeWrite(void)
{
    for (i=0;i<5;i++)
    {
        Dato.IntDato = CodeWriteIni[ i ] ;
        outportb ( OUTEEPROM , Dato.IntDato );
        /*delay(10)*/
        ;
    }
}


void codeProg(void)
{
    for (i=0;i<5;i++)
    {
        Dato.IntDato = CodeWriteProg[ i ] ;
        outportb ( OUTEEPROM , Dato.IntDato );
        /*delay(10)*/
        ;
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

        outportb ( OUTEEPROM , Dato.IntDato );
        /*delay(10)*/
        ;

        EvalBit=0x80;
        EvalBit = EvalBit >> i+1;

        Dato.BitDato.SK = 0x0;
        outportb ( OUTEEPROM , Dato.IntDato );
        /*delay(10)*/
        ;

        Dato.BitDato.SK = 0x1;
        outportb ( OUTEEPROM , Dato.IntDato );
        /*delay(10)*/
        ;

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

        outportb ( OUTEEPROM , Dato.IntDato );
        /*delay(10)*/
        ;

        EvalBit=0x80;
        EvalBit = EvalBit >> i+1;

        Dato.BitDato.SK = 0x0;
        outportb ( OUTEEPROM , Dato.IntDato );
        /*delay(10)*/
        ;

        Dato.BitDato.SK = 0x1;
        outportb ( OUTEEPROM , Dato.IntDato );
        /*delay(10)*/
        ;

    }
    Dato.BitDato.PE = 0x1;
    Dato.BitDato.CS = 0x0;
    Dato.BitDato.PRE = 0x0;
    Dato.BitDato.DI = 0x1;

    outportb ( OUTEEPROM , Dato.IntDato );

    delay(2);
}

