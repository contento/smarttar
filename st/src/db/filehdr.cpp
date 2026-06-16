//
// [ FILEHDR.CPP ]
//

#include "stdst.h"

// --- CRC-32 (ISO 3309 / IEEE 802.3) ----------------------------------------
// Used to validate FILE_HEADER integrity.  The CheckSum field stores the CRC
// of the entire header with CheckSum zeroed before computation.  This is the
// same polynomial (reflected) used by ZIP, PNG, Ethernet.

static DWORD crc32_tab[256];
static int   crc32_ready;

static void crc32_init(void)
{
    for (DWORD i = 0; i < 256; i++)
    {
        DWORD crc = i;
        int j;
        for (j = 0; j < 8; j++)
            crc = (crc >> 1) ^ (crc & 1 ? 0xEDB88320UL : 0UL);
        crc32_tab[i] = crc;
    }
    crc32_ready = 1;
}

static DWORD crc32(const void *data, int len)
{
    if (!crc32_ready) crc32_init();
    DWORD crc = 0xFFFFFFFFUL;
    const BYTE *p = (const BYTE *)data;
    int i;
    for (i = 0; i < len; i++)
        crc = crc32_tab[(crc ^ p[i]) & 0xFF] ^ (crc >> 8);
    return crc ^ 0xFFFFFFFFUL;
}
// CRC-32 over the header with CheckSum zeroed.
// ComputeCheckSum returns the CRC that should be stored; ValidateCheckSum
// returns TRUE if the stored CRC matches (or is zero — backward compat).

DWORD FILE_HEADER::ComputeCheckSum(FILE_HEADER const &hdr)
{
    FILE_HEADER &rw = (FILE_HEADER &)hdr;
    DWORD saved = rw.CheckSum;
    rw.CheckSum = 0UL;
    DWORD val = crc32(&rw, sizeof(rw));
    rw.CheckSum = saved;
    return val;
}

BOOL FILE_HEADER::ValidateCheckSum(FILE_HEADER const &hdr)
{
    if (hdr.CheckSum == 0UL)
        return TRUE; // backward-compatible with pre-2.95 files
    return ComputeCheckSum(hdr) == hdr.CheckSum;
}

#if !defined(__TEST__)
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
#if !defined(__NOAPPINFO__)
	extern SUPER_APP_INFO g_superAppInfo;
	APP_INFO appInfo = g_superAppInfo.Data;
	if (g_superAppInfo.Attr.Serialized)
        _Decrypt(&appInfo, sizeof(APP_INFO));
    strcpy(serial, appInfo.Serial);
#endif
#endif
#endif
    Attrib = 0;
    CheckSum = ComputeCheckSum(*this);
}

WORD FILE_HEADER::IsValid(void)
{
    BOOL ok = (!strcmp(Id, FILEHDR_ID));
    if (ok)
        ok = ValidateCheckSum(*this);
    return (WORD)ok;
}

char const *FILE_HEADER::GetId(void)
{
    return Id;
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
    appMajor   = MajorAppVersion;
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
