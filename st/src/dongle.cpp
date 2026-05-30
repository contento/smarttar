//
// [ DONGLE.CPP ]
//

#include "stdst.h"

#include <bios.h>
#include <dongle.h>

// --------------------------------------------------------------------------
//     DONGLE
// --------------------------------------------------------------------------

const UINT DONGLE::IN_PORT   = 0x379U;
const UINT DONGLE::CTRL_PORT = 0x378U;
const UINT DONGLE::SIZE      = 0x80U;

BOOL DONGLE::isThere(void)
{
    // warning !!!
    // the dongle must be check several times to ensure its efectiveness. GCC/gcc
    const char *ID = "JEAM";
    STR512 id;
    if (read(id))
        if (strcmp(id, ID))
            return FALSE;
    return TRUE;
}

BOOL DONGLE::read(char *line)
{
    const UINT BIOS_PRINT_STATUS = 0x02;
    const UINT BIOS_PRINT_BUSY   = 0x80;
    if (!(biosprint(BIOS_PRINT_STATUS, 0, 0) & BIOS_PRINT_BUSY))
        return FALSE;
    //
    bias();
    init();
    setRead();
    for (WORD i=0; i<SIZE; i++)
    {
        if (!(line[i] = getChar()))
        {
            if (!(i%2))
            {
                line[++i] = getChar(); // read next odd byte
                i++; // adjust to even
            }


            break;
        }
    }
    // words to back words
    swab(line, line, i);
    line[SIZE-1] = NULL; // close ASCIIZ string, you never know
    unlink();
    return TRUE;
}

BOOL DONGLE::write(char *)//line)
{
    return TRUE;
}

void DONGLE::bias(void)
{
    CtrlData.U = 0x01;
    outportb(CTRL_PORT, CtrlData.U);
    for (UINT i=0xFF; i > 1; i--)
        continue;
}

void DONGLE::init(void)
{
    const char *INIT_SEQ = "\x21\xA9\xE9\xC9\xE9";
    for (WORD i=0; i<0x05; i++)
    {
        CtrlData.U = INIT_SEQ[i];
        outportb(CTRL_PORT, CtrlData.U);
    }
}

void DONGLE::setRead(void)
{
    // set read from 0x00 position
    const UINT READ = 0x80;
    for (WORD i=0; i<8; i++)
    {
        CtrlData.B.DI = (READ & (0x80>>i))>>(8-i-1);
        outportb(CTRL_PORT, CtrlData.U);
        Edge(DOWN);
        Edge(UP);
    }
    Edge(UP);
    Edge(DOWN);
    Edge(UP);
}

char DONGLE::getChar(void)
{
    char byte = 0;
    for (WORD i=0; i<8; i++)
    {
        Edge(UP);
        Edge(DOWN);
        byte <<= 1;
        byte |= (inportb(IN_PORT) >> 4) & 0x01;
        Edge(UP);
    }
    return byte;
}

void DONGLE::unlink(void)
{
    CtrlData.B.PE = 0x1;
    CtrlData.B.CS = 0x0;
    CtrlData.B.PRE = 0x0;
    CtrlData.B.DI = 0x1;
    outportb(CTRL_PORT, CtrlData.U);
}

void DONGLE::Edge(DIR dir)
{
    CtrlData.B.SK = dir;
    outportb(CTRL_PORT, CtrlData.U);
}