//
// [ FILEHDR.CPP ]
//

#include "stdst.h"

#if !defined(__TEST__) && !defined(__DEMO__)
#include <info.h>
#endif

FILE_HEADER::FILE_HEADER(void)
{
	memset(this, 0, sizeof(*this));
	strcpy(Id, FILEHDR_ID);
	strcpy(Title, FILEHDR_TITLE);
	TextEnd = 0x041A; // + SUB + EOT with backword
#if (__FHEADER>=2)
	Version = FILEHDR_VER;
#if (__FHEADER>=3)
	MajorAppVersion   = APP_MAJOR_VER;
	MinorAppVersion   = APP_MINOR_VER;
    UpgradeAppVersion = APP_UPGRADE_VER;
    time = _GetSysTime();
    date = _GetSysDate();
#if !defined(__NOAPPINFO__) && !defined(__DEMO__) // v.220
	extern SUPER_APP_INFO g_superAppInfo;
	APP_INFO appInfo = g_superAppInfo.Data;
	if (g_superAppInfo.Attr.Serialized)
        _Decrypt(&appInfo, sizeof(APP_INFO));
    strcpy(serial, appInfo.Serial);
#endif
#endif
#endif
    CheckSum = 0L;
    Attrib   = 0;
}

char const *FILE_HEADER::GetId(void)
{
    return Id;
}

WORD FILE_HEADER::IsValid(void)
{
    return (!strcmp(Id, FILEHDR_ID));
}

char const *FILE_HEADER::getTitle(void)
{
    return Title;
}

void FILE_HEADER::SetAttr(WORD attr)
{
    Attrib = attr;
}

WORD FILE_HEADER::GetAttr(void) const
{
    return Attrib;
}

#if (__FHEADER>=2)
WORD FILE_HEADER::GetVersion(void) const
{
    return Version;
}
#endif

#if (__FHEADER>=3)
void FILE_HEADER::getAppVersion(WORD& appMajor, WORD& appMinor, WORD& appUpgrade)
{
    appMajor   = MajorAppVersion; // v.220.2
    appMinor   = MinorAppVersion;
    appUpgrade = UpgradeAppVersion;
}

int FILE_HEADER::getTime(void)
{
    return time;
}

int FILE_HEADER::getDate(void)
{
    return date;
}
#endif
