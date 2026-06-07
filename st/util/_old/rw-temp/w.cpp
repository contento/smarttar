#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>

#define OUT_ADDRESS 0x28d

typedef unsigned char BYTE;

union Data {
    struct
    {
BYTE NU:
        3;
BYTE PE:
        1;
BYTE PRE:
        1;
BYTE SK:
        1;
BYTE DI:
        1;
BYTE CS:
        1;
    }
    asBits ;
    BYTE asByte;
} data;

void write(BYTE *bytes, int nBytes);
void init(void);
void setPrg(void);
void release(void);
BYTE in (void);
void out(BYTE byte);
enum EDGE_DIRECTION {DOWN, UP};
void edge(EDGE_DIRECTION direction);

void main(void)
{
    clrscr();
    //
    init();

    out(0x30);
    release();
    setPrg();
    out(0x30);
    release();
    setPrg();
    out(0xFF);
    release();
    //
    BYTE bytes[80]={"0017"};
    int n = strlen(bytes);
    if(n%2)
        n++;
    write(bytes, n);
    //
    init();
    out(0x00);

    release();
}

void write(BYTE *bytes, int nBytes)
{
    BYTE wrBytes[80];
    swab(bytes, wrBytes, nBytes);
    BYTE address = 0x40; // ???
    for(int i=0 ; i <= nBytes; i++)
    {
        init();
        out(address);
        out(wrBytes[i]);
        i++;
        out(wrBytes[i]);
        release();
        address++;
    }
}

void init(void)
{
    BYTE initCodes[] = {0x20, 0xA8, 0xE8, 0xC8, 0xE8};
    for (int i=0;i<5;i++)
    {
        data.asByte = initCodes[i];
        outportb(OUT_ADDRESS, data.asByte);
        delay(10);
    }
}

void setPrg(void)
{
    BYTE prgCodes[] = { 0x20, 0xB8, 0xF8, 0xD8, 0xF8};
    for(int i=0; i<5; i++)
    {
        data.asByte = prgCodes[i] ;
        outportb(OUT_ADDRESS , data.asByte );
    }
}

void out(BYTE byte)
{
    for (int i=0 ; i<8 ; i++)
    {
        data.asBits.DI = (byte&(0x80>>i))?1:0;
        outportb(OUT_ADDRESS, data.asByte);
        edge(DOWN);
        edge(UP);
    }
}

void edge(EDGE_DIRECTION direction)
{
    data.asBits.SK = direction;
    outportb(OUT_ADDRESS, data.asByte);
}

void release(void)
{
    data.asBits.PE  = 0x1;
    data.asBits.CS  = 0x0;
    data.asBits.PRE = 0x0;
    data.asBits.DI  = 0x1;
    outportb (OUT_ADDRESS, data.asByte);
}