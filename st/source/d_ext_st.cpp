//
// [ D_EXT_ST.CPP ]
//

//
// extension account & statistics
//

#include "stdst.h"

#if defined(__TEST__)
#include <conio.h>
#endif

#include <d_ext_st.h>

extern CFG *g_cfg;

//
// I separated critical and non-critical information to ease the process of
// storing in STM2 and into the log file. GCC/gcc.
//

static const char *STATISTICS_EXT = ".STA";

DB_EXT_STATISTICS::DB_EXT_STATISTICS(const char *path, const char *name, int readOnly) :
        ReadOnly(readOnly),
        Status(OK)
{
    NonCritical = new DXS_NON_CRITICAL_ENTRY[MAX_BOOTH];
    Critical    = new DXS_CRITICAL_ENTRY;
    //
    if (path)
        strcat(strcpy(Filename, path), "\\");
    else
        _GetAppPath(Filename); // NULL path implies .EXE path
    strcat(Filename, name);
    strcat(Filename, STATISTICS_EXT);
    int numOfReadBytes = 0;
    _fmode = O_BINARY;
    File = -1; // to check for error
    if (access(Filename, 0) != 0)
        Status |= NO_FILE;
    else
    {
        if (access(Filename, 6) != 0 && !ReadOnly)
            chmod(Filename, S_IREAD|S_IWRITE); // enable read/write
        File = open(Filename, O_RDWR|O_BINARY, S_IREAD|S_IWRITE);
        // Check file header
        FILE_HEADER header;
        numOfReadBytes = read(File, &header, sizeof(header));
        if (numOfReadBytes < sizeof(header) || !header.IsValid())
            Status |= BAD_FILE;
        if (!(Status & BAD_FILE))
        {
            numOfReadBytes = read(File, NonCritical, sizeof(DXS_NON_CRITICAL_ENTRY)*MAX_BOOTH);
            if (numOfReadBytes < sizeof(DXS_NON_CRITICAL_ENTRY)*MAX_BOOTH)
                Status |= BAD_FILE;
            else
            {
                numOfReadBytes = read(File, Critical, sizeof(DXS_CRITICAL_ENTRY));
                if (numOfReadBytes < sizeof(DXS_CRITICAL_ENTRY))
                    Status |= BAD_FILE;
            }
        }
    }
    if (!ReadOnly && ((Status & NO_FILE) || (Status & BAD_FILE)))
    {
        Init();
        File = creat(Filename, S_IREAD|S_IWRITE);
        FILE_HEADER header;
        write(File, &header, sizeof(header));
        write(File, NonCritical, sizeof(DXS_NON_CRITICAL_ENTRY)*MAX_BOOTH);
        write(File, Critical, sizeof(DXS_CRITICAL_ENTRY));
        Status |= NEW;
    }
}

DB_EXT_STATISTICS::~DB_EXT_STATISTICS(void)
{
    if (!ReadOnly)
        Flush();
    if (!(Status & NO_FILE) && File != -1)
        close(File);
    delete [] NonCritical;
    delete Critical;
}

void DB_EXT_STATISTICS::Init(void)
{
    for (int i = 0; i < MAX_BOOTH; i++)
        NonCritical[i].Init();
    Critical->Init();
}

void DB_EXT_STATISTICS::Flush(void)
{
    if (lseek(File, sizeof(FILE_HEADER), SEEK_SET) != -1)
    {
        write(File, NonCritical, sizeof(DXS_NON_CRITICAL_ENTRY)*MAX_BOOTH);
        write(File, Critical, sizeof(DXS_CRITICAL_ENTRY));
    }
    int dupFile;
    dupFile = dup(File);
    close(dupFile);
}

DXS_NON_CRITICAL_ENTRY *DB_EXT_STATISTICS::GetNonCriticalEntry(WORD extNum) const
{
    return &NonCritical[extNum];
}

BOOL DB_EXT_STATISTICS::PutNonCriticalEntry(WORD extNum, DXS_NON_CRITICAL_ENTRY& entry)
{
    NonCritical[extNum] = entry;
    return TRUE;
}

DXS_CRITICAL_ENTRY *DB_EXT_STATISTICS::GetCritical(void) const
{
    return Critical;
}

BOOL DB_EXT_STATISTICS::PutCritical(DXS_CRITICAL_ENTRY& critical)
{
    *Critical = critical;
    return TRUE;
}

BOOL DB_EXT_STATISTICS::Store(WORD extNum)
{
    DXS_CRITICAL_ENTRY::STORED_ENTRY *stored = &Critical->Stored;
    DXS_CRITICAL_ENTRY::ONLINE_ENTRY *online = &Critical->Online[extNum];
    DXS_NON_CRITICAL_ENTRY *nonCritical = &NonCritical[extNum];
    // store
    stored->DDN += online->DDN;
    stored->DDI += online->DDI;
    for (int i = 0; i < 3; i++)
    {
        stored->Credits += nonCritical->Credits[i].Value;
        stored->Debits  += nonCritical->Debits[i].Value;
        stored->Others  += nonCritical->Others[i].Value;
		stored->Line    += g_cfg->E_LINE_COST;
		stored->Install += g_cfg->E_INSTALL_COST;
    }
    // clear
    online->Init();
    nonCritical->Init();
    return TRUE;
}

BOOL DB_EXT_STATISTICS::Archive(void)
{
    if (ReadOnly)
        return FALSE;
    Flush();
    close(File);
    if (access(Filename, 6) != 0)
        chmod(Filename, S_IREAD|S_IWRITE); // enable read/write
    //
    _mkSysDateDir();
    FILE_NAME arcFilename;
    STR16 tmp;
    _getSysDatePath(arcFilename);
    _PrefixAppPath(arcFilename);
    WORD year, month, day;
    _GetSysDate(year, month, day);
	extern CFG *g_cfg;
	sprintf(tmp, "\\RX%02d_%02d%s", day, g_cfg->TURN_NUMBER, STATISTICS_EXT);
    strcat(arcFilename, tmp);
    if (access(arcFilename, 6) != 0)
        chmod(arcFilename, S_IREAD|S_IWRITE); // enable read/write
    unlink(arcFilename); // sorry !!!
    rename(Filename, arcFilename);
    File = open(Filename, O_RDWR|O_BINARY, S_IREAD|S_IWRITE);
    return TRUE;
}

BOOL DB_EXT_STATISTICS::Repair(DB_STORAGE *dBStorage, BOOL all)
{
    dBStorage->Flush(); // to be sure
    if (all)
        Init();
    else
        for (int i = 0; i < MAX_BOOTH; i++)
            Critical->Online[i].Init();
    close(File);
    if (access(Filename, 6) != 0)
        chmod(Filename, S_IREAD|S_IWRITE); // enable read/write
    File = creat(Filename, S_IREAD|S_IWRITE);
    FILE_HEADER header;
    write(File, &header, sizeof(header));
	//
#if defined(__TEST__)
#if !defined(__UTIL__)
 //	cprintf("\n\r- Extensi¢n Statistics file, Record: ");
 //	short x = wherex(), y = wherey(); // 2.21.8 Build 6
#endif
#endif

	DB_STORAGE::Iterator it(*dBStorage);
	it.Restart();
	while (it)
	{
		Receipt receipt;

		long number = it.Current();

		if (dBStorage->Get(receipt, number))
		{
			Add(receipt);
		}

		it++;
	}

	write(File, NonCritical, sizeof(DXS_NON_CRITICAL_ENTRY)*MAX_BOOTH);
	write(File, Critical, sizeof(DXS_CRITICAL_ENTRY));
	return TRUE;
}

BOOL DB_EXT_STATISTICS::Add(Receipt& receipt)
{
	Critical->Online[receipt.BoothNumber] += receipt;
	return TRUE; // 2.21.8 build 9
}

BOOL DB_EXT_STATISTICS::Subtract(Receipt& receipt)
{
	Critical->Online[receipt.BoothNumber] -= receipt;
	return TRUE; // 2.21.8 build 9
}
