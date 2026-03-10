#ifndef __DONGLE_H
#define __DONGLE_H

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

// LPT1 Dongle by using EEPROM 93CS46

class DONGLE
{
public:
    DONGLE (void)
    {}


    ~DONGLE(void)
    {}
    //


    BOOL isThere(void);
    BOOL read (char *aLine);
    BOOL write(char *aLine);
private:
    enum DIR { DOWN=0x00, UP=0x01 };
    union {
        struct
        {
UINT  VCC:
            1;
UINT  NU :
            2;
UINT  PE :
            1;
UINT  PRE:
            1;
UINT  SK :
            1;
UINT  DI :
            1;
UINT  CS :
            1;
        }
        B; // bit per bit
        UINT U; // just an UINT
    } CtrlData;

    static const UINT  IN_PORT;
    static const UINT  CTRL_PORT;
    static const UINT  SIZE;
    //
    inline void bias    (void);
    inline void init    (void);
    inline void setRead (void);
    inline void setWrite(void);
    inline char getChar (void);
    inline void unlink  (void);
    inline void Edge    (DIR aDir);
};

#endif // __DONGLE_H
