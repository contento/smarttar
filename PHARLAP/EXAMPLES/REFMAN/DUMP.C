/*
DUMP.C -- dumps conventional mode memory from protected mode, to
    illustrate DosMapRealSeg() call in 286|DOS-Extender
        
cl -Lp dump.c

sample output:
C:\DOS>run286 dump fe00:0008
Real-mode pointer: FE00:0008
Protected-mode pointer: 012F:0008
FE00:0008 | 41 27 43 48 4C 27 35 43 52 4A 20 28 43 29 43 6F | A'CHL'5CRJ (C)Co
FE00:0018 | 70 79 72 69 67 68 74 20 43 4F 4D 50 41 51 20 43 | pyright COMPAQ C
FE00:0028 | 6F 6D 70 75 74 65 72 20 43 6F 72 70 6F 72 61 74 | omputer Corporat
FE00:0038 | 69 6F 6E 20 31 39 38 32 2C 38 33 2C 38 34 2C 38 | ion 1982,83,84,8
FE00:0048 | 35 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | 5...............
*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <phapi.h>

void fail(char *s) { puts(s); exit(1); }

main(int argc, char *argv[])
{
    BYTE far *fp, far *p;
    ULONG phys = -1;
    ULONG bytes = 1024;
    USHORT sel;
    USHORT i, j;
    
    if (argc < 2)
        fail("usage: dump [seg:ofs | #physaddr] [bytes]");
    else if (argc > 2)
        bytes = atoi(argv[2]);
    if (bytes & 0x0F)   // round up to next 16
        bytes = (bytes & 0xFFF0) + 0x10;
    
    if (argv[1][0] == '#')
    {
        // MAP PHYSICAL ADDRESS TO PROTECTED MODE
        sscanf(&argv[1][1], "%08lX", &phys);
        printf("Physical address: %08lX\n", phys);
        if (DosMapPhysSeg(phys, bytes, &sel) != 0)
            fail("DosMapPhysSeg - failure");
        fp = MAKEP(sel, 0);
        printf("Protected-mode pointer: %Fp\n", fp);
    }
    else
    {
        // MAP REAL-MODE POINTER TO PROTECTED MODE
        USHORT seg, ofs;
        sscanf(argv[1], "%04X:%04X", &seg, &ofs);
        seg += (ofs >> 4);  // normalize pointer
        ofs &= 0x0F;
        fp = MAKEP(seg, ofs);
        printf("Real-mode pointer: %Fp\n", fp);

        /*
            DosMapRealSeg() maps a real-mode paragraph into a protected-mode
            selector, not one four-byte pointer into another. Therefore,
            if the original real-mode pointer has a non-zero offset, this
            should be added into the number of bytes you want mapped, as
            we do here...
        */
        if (DosMapRealSeg(SELECTOROF(fp), bytes + OFFSETOF(fp), &sel) != 0)
            fail("DosMapRealSeg - failure");
        fp = MAKEP(sel, OFFSETOF(fp));
        printf("Protected-mode pointer: %Fp\n", fp);
    }

    for (i=0; i<bytes; i += 16)
    {
        p=fp+i;
        /*
            There's no point in printing out the mapped protected-mode
            address, so for display purposes we show the underlying real-
            mode address, using DosProtToReal()...
        */
        if (phys == -1)
            printf("%Fp | ", DosProtToReal(p));
        else
            printf("%08lX | ", phys+i);
        for (j=0; j<16; j++, p++)
            printf("%02X ", *p);
        printf("| ");
        for (j=0, p=fp+i; j<16; j++, p++)
            printf("%c", isprint(*p) ? *p : '.');
        printf("\n");
    }
}
