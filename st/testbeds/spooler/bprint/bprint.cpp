//
// [ BPRINT.CPP ]
//
#if !defined(__IOSTREAM_H)
#include <iostream.h>
#endif

#if !defined(__BIOS_H)
#include <bios.h>
#endif

#define BIOS_PRINT_WRITE     0x00
#define BIOS_PRINT_STATUS    0x02
#define BIOS_PRINT_BUSY      0x80
#define BIOS_PRINT_ERROR     0x08
#define BIOS_PRINT_PAPER_OUT 0x20

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef int            BOOL;
#define  FALSE        0
#define  TRUE         1

BOOL printByte(char byte);
void printBytes(char *bytes);

void main(void)
{
    cout << "Printing" << endl;
    printBytes(
        "Linea 1""\n"
        "Linea 2""\n"
        "\n"
        "\xFF"
    );
    printBytes(
        "Linea 3""\n"
        "Linea 4""\n"
        "\n"
        "\xFF"
    );
}

BOOL printByte(char byte)
{
    const BYTE LPT1 = 0;
    WORD status = biosprint(BIOS_PRINT_STATUS, 0, LPT1);
    if ((status&BIOS_PRINT_BUSY) && !(status&BIOS_PRINT_ERROR) && !(status&BIOS_PRINT_PAPER_OUT))
    {
        biosprint(BIOS_PRINT_WRITE, byte, LPT1);
        return TRUE;
    }
    else
        return FALSE;
}

void printBytes(char *bytes)
{
    int i = 0;
    while (bytes[i] != '\xFF')
    {
        if (printByte(bytes[i]))
            i++;
    }
}


