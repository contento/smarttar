#ifndef __DLL_DEFS_H
#define __DLL_DEFS_H

#if !defined(__WINDOWS_H)
typedef unsigned char   BYTE;
typedef          short  SHORT;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;
typedef int             BOOL;
typedef unsigned int  	UINT;
typedef unsigned char   UCHAR;
typedef          long   LONG;
typedef unsigned long   ULONG;
#define  FALSE        0
#define  TRUE         1
#endif

#define EXPORT        _far _loadds _export
#define PASCAL_EXPORT EXPORT _pascal

#endif // __DLL_DEFS_H

