//
// [ EEPROM.CPP ]
//

#include "stdst.h"

#include <eeprom.h>

const int EEPROM::IN_PORT  = 0x28F;
const int EEPROM::OUT_PORT = 0x28D;
const int EEPROM::MAX_LEN  = 0x400;

static const int nBytes = 80;
static BYTE  versionId[nBytes] = APP_DEVELOPER " " APP_NAME " " APP_VER_ID;

#pragma warn -ucp-

BOOL EEPROM::writeVersionId(void)
{
    write(versionId, strlen(versionId)+1); // incluido ASCIIZ
    return TRUE;
}

BOOL EEPROM::isValidVersion(void)
{
	BOOL ok = FALSE;
    BYTE bytes[nBytes];
    read(bytes, nBytes); // incluido ASCIIZ
#if (APP_MAJOR_VER == 2)
	if (strlen(versionId) == strlen(bytes))
	{
		// Force the version to be written by MicroDise¤o before installation.
		// GCC SmartTar 2.xx.x. V 2.33 build 2
		ok =
			bytes[13] == '2' &&
			bytes[14] == '.' &&
			bytes[15] == '3' &&
			bytes[16] == '4'
		;

	}
#endif
#if (APP_MAJOR_VER == 3)
	if (strlen(versionId) == strlen(bytes))
	{
		ok = bytes[13] == '3';
	}
#endif
	return ok;
}

BOOL EEPROM::isCandidateVersion(void)
{
	// it is now a synonim. V 2.33 build 2.
	return isValidVersion();
}

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
    for (int i=0; i < nBytes; i++)
    {
        init();
        out(address);
        out(wrBytes[i]);
        i++;
        out(wrBytes[i]);
        release();
        address++;
    }
    // v.220. HJ/gc
    setPrg();   // CS=PRE=PE=1, Habilita Protección de memoria
    out(0x30);  // 00110000=PREN
    release();
    setPrg();
    out(0x40);  // 01   000000   = PRWRITE,proteccion contra escritura
    // OPC   dir inicial
    release();
    //
    init();
    out(0x00);
    release();
}

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

void EEPROM::release(void)
{
    data.asBits.PE  = 0x1;
    data.asBits.CS  = 0x0;
    data.asBits.PRE = 0x0;
    data.asBits.DI  = 0x1;
    outportb (OUT_PORT, data.asByte);
}

BYTE EEPROM::in(void)
{
    BYTE byte = 0;
    for(int i=0 ; i<8 ; i++)
    {
        edge(UP);
        edge(DOWN);
        byte <<= 1;
        byte |= inportb(IN_PORT) & 0x01;
        edge(UP);
    }
    return byte;
}

void EEPROM::setRead(void)
{
    out(0x80); // read ?
    edge(UP);
    edge(DOWN);
    edge(UP);
}

void EEPROM::setPrg(void)
{
    BYTE prgCodes[] = { 0x20, 0xB8, 0xF8, 0xD8, 0xF8 };
    for(int i=0; i<5; i++)
    {
        data.asByte = prgCodes[i] ;
        outportb(OUT_PORT, data.asByte);
    }
}

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

void EEPROM::edge(EDGE_DIRECTION direction)
{
    data.asBits.SK = direction;
    outportb(OUT_PORT, data.asByte);
}

