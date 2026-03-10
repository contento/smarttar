#ifndef __ST_UTIL_H
#define __ST_UTIL_H

// same as UTIL.H but we need to avoid UTIL.H in Turbo Vision

#if !defined(__STDLIB_H)
#include <stdlib.h>
#endif

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

char _ISO2ASCII(char byte);
char _ISO2ASCII2(char byte);
char *_ISO2ASCII(char* str);
char *_ISO2ASCII2(char* str);
void _ReadPassword(char *password, WORD size);
BOOL _isDatePassword(const char *password);

//
// encrypt and decrypt
//
void _Encrypt(void *buffer, WORD bufferSize, WORD seed = 0x0F);
void _Decrypt(void *buffer, WORD bufferSize, WORD seed = 0x0F);

//
// beautify strings
//
void _RTrim    (char *string);
void _DelSpaces(char *string);

//
// Files
//

// build a filename with the path of the program
char *_PrefixAppPath(char *filename);
char *_GetAppPath(char *path);
char *_getSysDatePath(char *path, BOOL dayPath=FALSE);
BOOL _mkSysDateDir(const char *basePath=NULL, BOOL createDayPath=FALSE);

char * g_GetErrnoStr(char *pszErr);

// save a file and create a bak
BOOL _SaveAsBak(const FILE_NAME& filename);

// copy file
enum COPY_STAT_TAG {
    CP_OK,
    CP_SOURCE_NOT_FOUND,
    CP_TARGET_NOT_OPEN,
    CP_COPY_ITSELF,
    CP_GEN_FAIL
};
WORD _CopyFile(const char *source, const char *target);

// patch file, useful for serializing
enum PATCH_STAT_TAG {
    PATCH_OK,
    PATCH_ID_NOT_FOUND,
    PATCH_FILE_NOT_FOUND,
    PATCH_GEN_FAIL
};

WORD _PatchFile(const char *filename, const char *key, void *patch, WORD size);
WORD _GetPatchFromFile(const char *filename, const char *key, void *patch, WORD size);

// to quick sort and binary search on DWORD numbers
void _LongQSort(DWORD *items, size_t numOfItems);
DWORD *_LongBSearch(DWORD key, DWORD *items, size_t numOfItems);
BOOL    g_LongLFind(DWORD key, DWORD *pItems, size_t numOfItems);

// date miscelaneous Zinc compatible
double g_Milisec2Time(DWORD ms, int correctionTime);
void   g_Milisec2Time(DWORD ms, int& hour, int& minutes, int &seconds);

WORD _GetSysDate(void);
void _GetSysDate(WORD& year, WORD& month, WORD& day);
char *_GetSysDate(char *strDate, BOOL europeanStyle = TRUE);
void _SetSysDate(WORD packedDate);
void _SetSysDate(WORD year, WORD month, WORD day);
BOOL _IsSaturday(WORD year, WORD month, WORD day);
BOOL _IsSaturday(WORD packedDate);
BOOL _IsSunday(WORD year, WORD month, WORD day);
BOOL _IsSunday(WORD packedDate);
BOOL _IsWeekend(WORD year, WORD month, WORD day);
BOOL _IsWeekend(WORD packedDate);
WORD _PackDate(WORD& packedDate, WORD year, WORD month, WORD day);
void _UnpackDate(WORD packedDate, WORD& year, WORD& month, WORD& day);
void _UnpackDate(WORD packedDate, WORD& year, WORD& month, WORD& day, WORD& dayOfWeek);
WORD  _GetDayOfWeek(WORD packedDate);
WORD  _GetDayOfWeek(WORD year, WORD month, WORD day);
// time miscelaneous  Zinc compatible
WORD _GetSysTime(void);
void _GetSysTime(WORD& hour, WORD& minute);
void _GetSysTime(WORD& hour, WORD& minute, WORD& second);
char *_GetSysTime(char *strTime, BOOL printSeconds = FALSE);
void _SetSysTime(WORD packedTime);
void _SetSysTime(WORD hour, WORD minute, WORD second=0);
WORD _PackTime(WORD& packedTime, WORD hour, WORD minute, WORD second=0);
void _UnpackTime(WORD packedTime, WORD& hour, WORD& minute);
void _UnpackTime(WORD packedTime, WORD& hour, WORD& minute, WORD& second);
BOOL _Str2Date(const char *strDate, WORD& year, WORD& month, WORD& day, BOOL europeanStyle=TRUE);
char *_Date2Str(char *strDate, WORD year, WORD month, WORD day, BOOL europeanStyle=TRUE);
char *_Time2Str(char *strTime, WORD hour, WORD minute, WORD second=0, BOOL printSeconds = FALSE);

char * g_Number2Str(char *pszNumber, long number, WORD nDigits = 0, BOOL bLeadingZeros = FALSE);

double g_Round(double value, double roundVal);
double g_Ceil (double value, double ceilVal);

#endif // __ST_UTIL_H
