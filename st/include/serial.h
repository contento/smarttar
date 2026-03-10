#ifndef __SERIAL_H
#define __SERIAL_H

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

class SERIAL
{
public:
    enum LSR { // Line Status Register (LSR).
        RCVRDY   = 0x01,
        OVRERR   = 0x02,
        PRTYERR  = 0x04,
        FRMERR   = 0x08,
        BRKERR   = 0x10,
        XMTRDY   = 0x20,
        XMTRSR   = 0x40,
        TIMEOUT	 = 0x80
    };
    enum MSR { // Modem Input Status Register (MSR).
        CTS = 0x10,
        DSR = 0x20
    };
    enum PORT   { COM1, COM2 };
    enum PARITY { NONE, ODD, EVEN };
    enum SPEED  {
        S300 = 300,
        S600 = 600,
        S1200 = 1200,
        S2400 = 2400,
        S4800 = 4800,
        S9600 = 9600,
        S19200 = 19200
    };
    enum BITS     { S7, S8};
    enum STOPBITS { S1, S2};
    //
    SERIAL(WORD port=COM1, WORD speed=S1200, WORD parity=NONE, WORD bits=S8, WORD stopBits=S1, WORD bufLen=0x2000);
    ~SERIAL();
    //
    BOOL IsInstalled(void)
    {
        return Installed;
    }
    WORD GetStatus(void);
    BOOL Put(char byte);
    BOOL Put(const char *msg);
    BOOL Get(char& byte);
private:
    static char *Buffer;
    static WORD BufLen;
    static WORD BufStart;
    static WORD BufEnd;
    static WORD Port;
    static WORD PortBase;
    static WORD Installed;
    //
    inline BOOL Set(WORD port, WORD speed, WORD parity, WORD bits, WORD stopBits);
    //
    static void interrupt far    NewSerialISR(...);
    static void interrupt (far * OldSerialIV)(...);
};

#endif // __SERIAL_H
