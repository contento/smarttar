#ifndef __LOG_H
#define __LOG_H

#if !defined(__STRING_H)
#include <string.h>
#endif

#if !defined(__FSTREAM_H)
#include <fstream.h>
#endif

#if !defined(__ST_UTIL_H)
#include <st_util.h>
#endif

class Log
{
public:
    Log(int openMode = IN);
    Log(const char *filename, int openMode = IN);
    ~Log();
    //
    enum {
        IN     = 0x01,
        OUT    = 0x02,
        //
        CREATE = 0x10
    };
    enum Motif {
        NONE,
        NORMALSTART,
        NORMALSHUTDOWN,
        CHANGETURN,
        APPBADTRY,
        CFGBADTRY,
        DONGLEBADTRY,
        EEPROMBADTRY,
        STM2BADTRY,
        STM2GARBAGE,
        STM2BADSHUTDOWN,
        STM2RECOVER,
        STM2IGNORERECOVER,
        //
        UNRECOGNIZED
    };
    class Entry
    {
public:
        Entry()
        {
            memset(this, 0, sizeof(*this));
            date  = _GetSysDate();
            time  = _GetSysTime();
            motif = NONE;
        }
        int date;
        int time;
        int motif;
        //
        int dummy[ // for future enhancements
            0x10
        ];
    };
    //
    BOOL put(int motif);
    BOOL put(int date, int time, int motif);
    BOOL put(Entry& entry);
    BOOL get(Entry& entry);
    BOOL restart();
    BOOL archive(void);
    char *motif2Str(char *str, int motif, BOOL shortMotif = FALSE) const;
    BOOL openFail(void)
    {
        return !openOk;
    }
private:
    fstream   *file;
    int       openMode;
    BOOL      openOk;
    FILE_NAME filename;
    //
    BOOL open(const char *filename, int openMode);
};

#endif // __LOG_H
