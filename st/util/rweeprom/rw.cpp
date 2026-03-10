//
// [ RWEEPROM.CPP ]
//
#include <iostream.h>
#include <stdlib.h>
#include <dos.h>
#include <string.h>

#include <st_defs.h>
#include <version.h>

class EEPROM
{ // serie 93CS46
public:
    void read (      BYTE *bytes, int nBytes);
    void write(const BYTE *bytes, int nBytes);
private:
    static const int MAX_LEN;
    static const int IN_PORT;
    static const int OUT_PORT;
    //
    enum EDGE_DIRECTION {DOWN, UP};
    union Data {
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
        asBits;
        BYTE asByte;
    } data;
    void init(void);
    void setRead(void);
    void setPrg (void);
    void release(void);
    BYTE in (void);
    void out(BYTE byte);
    void edge(EDGE_DIRECTION direction);
};

const int EEPROM::IN_PORT  = 0x28F;
const int EEPROM::OUT_PORT = 0x28D;
const int EEPROM::MAX_LEN  = 0x400;


//***************READ***************

void EEPROM::read(BYTE *bytes, int nBytes)
{
    if(nBytes%2)
        nBytes++; // by pairs
    if (nBytes>MAX_LEN)
        return;
    init();
    setRead();
    //
    for (int i=0; i<nBytes; i++)
        bytes[i] = in();
    swab(bytes, bytes, nBytes);
    //
    release();
}


//****************WRITE************

void EEPROM::write(const BYTE *bytes, int nBytes)
{
    if(nBytes%2)
        nBytes++; // by pairs
    if (nBytes>MAX_LEN)
        return;
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
    BYTE wrBytes[80];
    swab((char *)bytes, wrBytes, nBytes);
    BYTE address = 0x40; // ???
    for (int i=0; i <= nBytes; i++)
    {
        init();
        out(address);
        out(wrBytes[i]);
        i++;
        out(wrBytes[i]);
        release();
        address++;
    }
    //
    //********************proteccion de memoria desde la direccion 00*********

    setPrg();   // CS=PRE=PE=1, Habilita Protección de memoria
    out(0x30);  // 00110000=PREN
    release();
    setPrg();
    out(0x40);  //  01   000000   = PRWRITE,proteccion contra escritura
    // OPC   dir inicial
    release();
    //*************************************************************************

    init();
    out(0x00);
    release();
}

//********************
void EEPROM::init(void)
{
    BYTE initCodes[] = {0x20, 0xA8, 0xE8, 0xC8, 0xE8};
    for (int i=0;i<5;i++)
    {
        data.asByte = initCodes[i];
        outportb(OUT_PORT, data.asByte);
        delay(10);
    }
}

//******************
void EEPROM::release(void)
{
    data.asBits.PE  = 0x1;
    data.asBits.CS  = 0x0;
    data.asBits.PRE = 0x0;
    data.asBits.DI  = 0x1;
    outportb (OUT_PORT, data.asByte);
}


//********************

BYTE EEPROM::in(void)
{
    BYTE byte = 0;
    for(int i=0 ; i<8 ; i++)
    {
        edge(UP);
        edge(DOWN);
        byte <<= 1;
        byte = byte | inportb(IN_PORT) & 0x01;
        edge(UP);
    }
    return byte;
}

//********************

void EEPROM::setRead(void)
{
    out(0x80); // read ?
    edge(UP);
    edge(DOWN);
    edge(UP);
}

//*****************
void EEPROM::setPrg(void)
{
    BYTE prgCodes[] = { 0x20, 0xB8, 0xF8, 0xD8, 0xF8 };
    for(int i=0; i<5; i++)
    {
        data.asByte = prgCodes[i] ;
        outportb(OUT_PORT, data.asByte);
    }
}


//******************
void EEPROM::out(BYTE byte)
{
    for (int i=0 ; i<8 ; i++)
    {
        data.asBits.DI = (byte&(0x80>>i))?1:0;
        outportb(OUT_PORT, data.asByte);
        edge(DOWN);
        edge(UP);
    }
}


//******************


void EEPROM::edge(EDGE_DIRECTION direction)
{
    data.asBits.SK = direction;
    outportb(OUT_PORT, data.asByte);
}




//*****************************************************************************
int main(int argc, char *argv[])
{
    cout
    << "RWEEPROM 1.0 (SmartTar " << szVerName << ')' << endl
    << "Copyright (c) 1993-1999 MicroDise¤o Ltda." << endl << endl
    << "  rweeprom [/w]" << endl
    << "    /w para grabar" << endl
    << endl
    ;
    BOOL writing;
    writing = (argc > 1) && (!strcmp(argv[1], "/w") || !strcmp(argv[1], "/w"));
    EEPROM eeprom;
    const int nBytes = 80;
    BYTE bytes[nBytes];
    if (writing)
    {
        cout << " Bytes: ";
        cin  >> bytes;
        cout << "  Escribiendo [" << bytes << "] ..." << endl;
        eeprom.write(bytes, strlen(bytes)+1); // incluido ASCIIZ
        // FEEDBACK
        cout << "  Leyendo ..." << endl;
        eeprom.read(bytes, nBytes);
        cout << "  Bytes escritos: [" << bytes << "]" << endl;
    }
    else
    {
        cout << "  Leyendo ..." << endl;
        eeprom.read(bytes, nBytes);
        cout << "  Bytes leidos: [" << bytes << "]" << endl;
    }
    //
    return 0;
}
