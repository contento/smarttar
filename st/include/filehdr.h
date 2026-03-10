#ifndef __FILEHDR_H
#define __FILEHDR_H

#if !defined(__STRING_H)
#include <string.h>
#endif

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

#if (__FHEADER==1)
#define FILEHDR_ID    "A&G"
#define FILEHDR_TITLE "SmartTar Datafile 1.0"
#elif (__FHEADER==2)
#define FILEHDR_ID    APP_DEVELOPER
#define FILEHDR_TITLE "SmartTar file 2.0"
#define FILEHDR_VER   0x02U
#elif (__FHEADER==3)
#define FILEHDR_ID    APP_DEVELOPER
#define FILEHDR_TITLE "SmartTar file 3.0"
#define FILEHDR_VER   0x03U
#else
#error You have to provide a valid File Header Version Number. GCC/gcc.
#endif

// Try to keep the header size by adjusting the Dummy. GCC/gcc.
struct FILE_HEADER
{
    FILE_HEADER(void);
    char const *FILE_HEADER::GetId(void);
    WORD FILE_HEADER::IsValid(void);
    char const *FILE_HEADER::getTitle(void);
    void FILE_HEADER::SetAttr(WORD attr);
    WORD FILE_HEADER::GetAttr(void) const;
#if (__FHEADER>=2)
    WORD FILE_HEADER::GetVersion(void) const;
#endif
#if (__FHEADER>=3)
    void getAppVersion(WORD& appMajor, WORD& appMinor, WORD& appUpgrade);
    char const *getSerial(void);
    int  getDate();
    int  getTime();
#endif
    enum ATTR {
        COMPRESSED = 0x0001,
        CRYPTED    = 0x0002,
        CHECKABLE  = 0x0004
    };
private:
    char Id[0x04];
    char Title[0x26];
    WORD TextEnd;     // useful for DOS "Type" command
    // local v 2.0 introduces version as a word
#if (__FHEADER >= 2)
    WORD Version;
#endif
    DWORD CheckSum;    // maybe CRC-32
    WORD  Attrib;      // for future F_COMPRESS or other
#if (__FHEADER >= 3)
    WORD 					MajorAppVersion;
    WORD 					MinorAppVersion;
    WORD 					UpgradeAppVersion;
    SERIAL_NUMBER serial;
    int  					time;   // when the file was created
    int  					date;
#endif
    char Dummy[
        0x40
#if (__FHEADER >= 2)
        - sizeof(WORD) // version
#endif
#if (__FHEADER >= 3)
        - sizeof(WORD) // appversion
        - sizeof(WORD)
        - sizeof(WORD)
        - sizeof(SERIAL_NUMBER)
        - sizeof(int) // date
        - sizeof(int) // time
#endif
    ];
};

#endif // __FILEHDR_H
