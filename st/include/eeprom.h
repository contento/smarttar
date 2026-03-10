#ifndef __EEPROM_H
#define __EEPROM_H

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

class EEPROM
{ // serie 93CS46
public:
    void read (      BYTE *bytes, int nBytes);
    void write(const BYTE *bytes, int nBytes);
    BOOL writeVersionId(void);
    BOOL isValidVersion(void);
    BOOL isCandidateVersion(void);
private:
    static const int MAX_LEN;
    static const int IN_PORT;
    static const int OUT_PORT;
    //
    enum EDGE_DIRECTION {DOWN, UP};
    union Data {
        struct
        {
unsigned  NC:
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

#endif // __EEPROM_H
