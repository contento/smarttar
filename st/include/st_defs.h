#ifndef __ST_DEFS_H
#define __ST_DEFS_H

// product info
#define APP_NAME           "SmartTar"
#define APP_COMPANY        "MicroDise¤o Ltda."
#define APP_ISO_COMPANY    "MicroDiseño Ltda."
#define APP_DEVELOPER      "GCC"
#define APP_DEVELOPER_NAME "Gonzalo Contento"
#define APP_COPYRIGHT_DATE "Copyright (c) 1993-2003"
#define APP_COPYRIGHT      APP_COPYRIGHT_DATE " " APP_COMPANY
#define APP_ISO_COPYRIGHT  APP_COPYRIGHT_DATE " " APP_ISO_COMPANY
// Version Information
// Derived from st/include/version.h -- the single source of truth.
// Run bump-version.sh / bump-version.ps1 to change. Do not hand-edit.
#include "version.h"
#define APP_MAJOR_VER     ST_VERSION_MAJOR
#if APP_MAJOR_VER>9
#error Major version number great than 9. GCC/gcc.
#endif
#define APP_MINOR_VER     ST_VERSION_MINOR
#if APP_MINOR_VER>99
#error Minor version number great than 99. GCC/gcc.
#endif
#define APP_UPGRADE_VER   ST_VERSION_PATCH
#if APP_UPGRADE_VER>9
#error Upgrade Version Number great than 9. GCC/gcc.
#endif
// "Build N" historically tracked patch-equivalent counts within a
// single MAJOR.MINOR.UPGRADE. Folded into ST_VERSION_PATCH now, so the
// displayed Build number mirrors the patch.
#define APP_BUILD         "Build " ST_STRINGIFY(ST_VERSION_PATCH)

#define APP_VER_ID        ST_VERSION         // "v.ss.uu" e.g. "2.34.2"
#define APP_VER           ST_VERSION_SHORT   // "v.ss"    e.g. "2.34"

#if !defined(__EDA__)
#define APP_VER_NAME       APP_NAME " " APP_VER
#define APP_VER_ID_NAME    APP_NAME " " APP_VER_ID
#else
#define APP_VER_NAME       APP_NAME " " APP_VER " EDA"
#define APP_VER_ID_NAME    APP_NAME " " APP_VER_ID " EDA"
#endif

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

typedef char STR16 [16];
typedef char STR32 [32];
typedef char STR64 [64];
typedef char STR128[128];
typedef char STR256[256];
typedef char STR512[512];

typedef char COMPANY_NAME[31];
typedef char CALL_HEADER[5];        // access codes + first next digit.
typedef char CALL_ACCESS_HEADER[4]; // only access codes.
typedef WORD CALL_AREA_CODE;        // the digit after access codes
typedef WORD CALL_ATTR;
typedef char CITY_NAME[21];
typedef char PHONE[17];
typedef long PHONE_NUMBER;
typedef char FILE_NAME[80];
typedef char TEXT_FILE_LINE[512];
typedef char TITLE[0x20];
typedef char SERIAL_NUMBER[0x20];
typedef char KEY[0x20];
typedef DWORD RECEIPT_NUMBER;

const int MAX_HOLLYDAYS   = 4;
const int MAX_HOLLY_YEARS = 3;

struct CALENDAR_ENTRY
{
	WORD Year;
	WORD Table[12][MAX_HOLLYDAYS];
};

const UINT CLUSTER_SIZE       = 8; // each cluster made up by 8 booths
const UINT MAX_CLUSTER        = 4; // max number of system cluster
const UINT MAX_BOOTH          = MAX_CLUSTER*CLUSTER_SIZE;
const UINT MAX_MAGNETIC_CARDS = 4;

// RT_ENGINE
const UINT RT_MAXRECEIPTS     = 0x40; // real time/STM2

// DSTATISTICS
const UINT DS_MAXENTRIES          = 5;
const UINT DS_MAXCELLULARENTRIES  = 5;
const UINT DS_MAXDOUBLEPRNENTRIES = 2;
//
// Receipt attributes: paid state: 4 bits and call attributes: 6 bits.
//
const WORD NOT_PAID_CALL    = 0x1; //  0001
const WORD TOLL_FREE_CALL	= 0x2; //  0010
const WORD PAID_CALL        = 0x4; //  0100
//
const WORD INTERNATIONAL_CALL_MASK   = 0x20; // 1x xxxx Mask
const WORD DDI_DIAL_MASK             = 0x10; // x1 xxxx Mask
const WORD LOCAL_DIAL_MASK           = 0x08; // xx 1xxx Mask
const WORD NOT_INCLUDED_CALL_MASK    = 0x04; // xx x1xx Mask
#if !defined(__EDA__)
const WORD SPECIAL_CALL              = 0x09; // 00 1001
const WORD LOCAL_CALL	             = 0x0A; // 00 1010
const WORD CELLULAR_CALL             = 0x01; // 00 0001
const WORD DDN_CALL		             = 0x02; // 00 0010
const WORD DDI_CALL	                 = 0x31; // 11 0001
const WORD BORDER_CALL               = 0x21; // 10 0001
#else
const WORD SPECIAL_CALL              = 0x09; // 00 1001
const WORD EDA2EDA_CALL              = 0x0A; // 00 1010
const WORD LOCAL_EDA2EPM_CALL        = 0x0B; // 00 1011
const WORD CELLULAR_CALL             = 0x01; // 00 0001
const WORD DDN_EDA2EPM_CALL          = 0x02; // 00 0010
const WORD DDN_EDA2TEL_CALL          = 0x03; // 00 0011
const WORD DDI_EDA2TEL_CALL          = 0x31; // 11 0001
const WORD BORDER_CALL               = 0x21; // 10 0001
#endif

#endif // __ST_DEFS_H
