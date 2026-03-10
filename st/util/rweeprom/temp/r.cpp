/* programacion de memoria eeprom 93cs46 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>

#define IN_ADDRESS  0x28F
#define OUT_ADDRESS 0x28D

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

void init(void);
void set(void);
void release(void);
void read(BYTE *bytes, int nBytes);
BYTE in (void);
void out(BYTE byte);

enum EDGE_DIRECTION {DOWN, UP};
void edge(EDGE_DIRECTION direction);

void main(void)
{
    clrscr();
    init();
    set();
    const nBytes = 80;
    BYTE bytes[nBytes];
    read(bytes, nBytes);
    release();
    printf(" Lectura de Serial = %s\n",bytes);
}

void init(void)
{
    BYTE initCodes[] = {0x20, 0xA8, 0xE8, 0xC8, 0xE8};
    for (int i=0;i<5;i++)
    {
        data.asByte = initCodes[i];
        outportb(OUT_ADDRESS, data.asByte);
    }
}

void release(void)
{
    data.asBits.PE  = 0x1;
    data.asBits.CS  = 0x0;
    data.asBits.PRE = 0x0;
    data.asBits.DI  = 0x1;
    outportb (OUT_ADDRESS, data.asByte);
}

void read(BYTE *bytes, int nBytes)
{
    memset(bytes, 0, nBytes);
    int i=0 ;
    do
    {
        bytes [i] = in();
    }
    while (bytes[i++] && i < nBytes);
    swab(bytes, bytes, nBytes);
    bytes[i] = 0; // ASCIIZ
}


BYTE in(void)
{
    BYTE byte = 0;
    for(int i=0 ; i<8 ; i++)
    {
        edge(UP);
        edge(DOWN);
        byte <<= 1;
        byte = byte | inportb(IN_ADDRESS) & 0x01;
        edge(UP);
    }
    return byte;
}

void set(void)
{
    out(0x80); // write ???
    edge(UP);
    edge(DOWN);
    edge(UP);
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
