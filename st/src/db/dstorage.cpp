//
// [ DSTORAGE.CPP ]
//

// datafile and index management

#include "stdst.h"

#if defined(__TEST__)
#include <conio.h>
#endif

#include <dstorage.h>

//
// we use C I/O to get the best performance !!!
//

static const char *DATAFILE_EXT  = ".DAT";
static const char *INDEXFILE_EXT = ".IDX";

const UINT BinStorage::MAGIC_NUMBER = 0x6719U;
const long DB_STORAGE_BACKEND::MAX_RECEIPTS = 100000000L; // 1000000L;. v.219a

BOOL BinStorage::IsValid(Receipt const & receipt)
{
	return receipt.MagicNumber == BinStorage::MAGIC_NUMBER;
}

extern CFG *g_cfg;

BinStorage::BinStorage(const char *path, const char *name, int readOnly) :
	ReadOnly(readOnly),
	Status(OK)
{
	if (path)
		strcat(strcpy(DataFilename, path), "\\");
	else
		_GetAppPath(DataFilename); // NULL path implies .EXE path

    strcat(DataFilename, name);
    strcpy(IndexFilename, DataFilename);
    strcat(DataFilename, DATAFILE_EXT);
    strcat(IndexFilename, INDEXFILE_EXT);
	//
    // try to open both files
    //
	_fmode = O_BINARY;
	// --- open and check DataFile
	DataFile = -1; // to check for error
	if (access(DataFilename, 0) != 0)
		Status |= NO_DATA_FILE;
	else
	{
		if (access(DataFilename, 6) != 0 && !ReadOnly)
			chmod(DataFilename, S_IREAD|S_IWRITE); // enable read/write

		DataFile = open(DataFilename, O_RDWR|O_BINARY, S_IREAD|S_IWRITE);
		// Check Header
		DataHeader dataHeader;
		if (ReadDataHeader(dataHeader))
			m_dataHeader = dataHeader; // it's good, copy it
		else
			Status |= BAD_DATA_FILE;
	}

	if (!ReadOnly && ((Status & NO_DATA_FILE) || (Status & BAD_DATA_FILE)))
	{
		DataFile = creat(DataFilename, S_IREAD|S_IWRITE);
		if (DataFile == -1)
			Status |= BAD_DATA_FILE;
		else if (WriteDataHeader(m_dataHeader))
			Status |= NEW_DATA_FILE;
		else
			Status |= BAD_DATA_FILE;
	}
	// --- open and check index file
	IndexFile = -1; // to check for error
	if (access(IndexFilename, 0) != 0)
	{
		Status |= NO_INDEX_FILE;
	}
	else
	{
		if (access(IndexFilename, 6) != 0 && !ReadOnly)
			chmod(IndexFilename, S_IREAD|S_IWRITE); // enable read/write

		IndexFile = open(IndexFilename, O_RDWR|O_BINARY, S_IREAD|S_IWRITE);
		// Check Header
		IndexHeader indexHeader;
		if (ReadIndexHeader(indexHeader))
			m_indexHeader = indexHeader; // it's good, copy it
		else
			Status |= BAD_INDEX_FILE;
	}

	if (!ReadOnly && ((Status & NO_INDEX_FILE) || (Status & BAD_INDEX_FILE)))
	{
		IndexFile = creat(IndexFilename, S_IREAD|S_IWRITE);
		if (IndexFile == -1)
			Status |= BAD_INDEX_FILE;
		else if (WriteIndexHeader(m_indexHeader))
			Status |= NEW_INDEX_FILE;
		else
			Status |= BAD_INDEX_FILE;
	}

	// cache
	m_pIndexCache = new IndexCache(*this, g_cfg->CACHE_SIZE);
}

BinStorage::~BinStorage()
{
    if (!ReadOnly)
        Flush();

	if (m_pIndexCache)
		delete m_pIndexCache;
	//
	if (!(Status & NO_DATA_FILE) && DataFile != -1)
		close(DataFile);

	if (!(Status & NO_INDEX_FILE) && IndexFile != -1)
		close(IndexFile);
}

void BinStorage::Flush(void)
{
	// force to dump
	int dupFile;
	dupFile = dup(DataFile);
	close(dupFile);
	dupFile = dup(IndexFile);
	close(dupFile);
}

BOOL BinStorage::Archive(void)
{
	if (ReadOnly)
		return FALSE;

	Flush(); // to be sure
	//
	_mkSysDateDir();
	// --- data file
	close(DataFile);
	if (access(DataFilename, 6) != 0)
		chmod(DataFilename, S_IREAD|S_IWRITE); // enable read/write
	FILE_NAME arcFilename;
	STR16 tmp;
	_getSysDatePath(arcFilename);
	_PrefixAppPath(arcFilename);
	WORD year, month, day;
	_GetSysDate(year, month, day);
	sprintf(tmp, "\\RX%02d_%02d%s", day, g_cfg->TURN_NUMBER, DATAFILE_EXT);
	strcat(arcFilename, tmp);
	if (access(arcFilename, 6) != 0)
		chmod(arcFilename, S_IREAD|S_IWRITE); // enable read/write
	unlink(arcFilename); // sorry !!!
	rename(DataFilename, arcFilename); // move it
	// --- index file
	close(IndexFile);
	if (access(IndexFilename, 6) != 0)
		chmod(IndexFilename, S_IREAD|S_IWRITE); // enable read/write
	_getSysDatePath(arcFilename);
	_PrefixAppPath(arcFilename);
	sprintf(tmp, "\\RX%02d_%02d%s", day, g_cfg->TURN_NUMBER, INDEXFILE_EXT);
	strcat(arcFilename, tmp);
	if (access(arcFilename, 6) != 0)
		chmod(arcFilename, S_IREAD|S_IWRITE); // enable read/write
	unlink(arcFilename); // sorry !!!
	rename(IndexFilename, arcFilename); // move it
	//
	return TRUE;
}

void BinStorage::EnumReceipts(CallbackFnPtr callback)
{
	Flush(); // to be sure

	// UnloadCache();

	Receipt receipt;
	int nRead;
	lseek(DataFile, sizeof(DataHeader), SEEK_SET);

    while (!eof(DataFile))
    {
		nRead = read(DataFile, &receipt, sizeof(Receipt));
		if
		(
			nRead == sizeof(Receipt)   		&&
			IsValid(receipt)  				&&
			receipt.Number > 0              &&
			receipt.Number < MAX_RECEIPTS
		)
		{
			callback(receipt);
		}
	}
}

BOOL BinStorage::RepairDataFile(void)
{
	Flush(); // to be sure

	// assume that data file are bad
	// --- create a temporal file to save receipts
	char *tmpFilename = "rx~.tmp";
	if (access(tmpFilename, 6) != 0)
		chmod(tmpFilename, S_IREAD|S_IWRITE); // enable read/write

	int tmpFile = creat(tmpFilename, S_IREAD|S_IWRITE);
	if (tmpFile == -1)
		return FALSE;

	int nWritten = write(tmpFile, &m_dataHeader, sizeof(DataHeader));
	if (nWritten != sizeof(DataHeader))
	{
		close(tmpFile);
		return FALSE;
	}

	// take from data file and Recover to tmp file
#if defined(__TEST__)
#if !defined(__UTIL__)
	cprintf("\n\r- Data file, Record: ");
	short x = wherex(), y = wherey();
#endif
#endif
	lseek(DataFile, sizeof(DataHeader), SEEK_SET);
	//
	// 2.21.1 build 6
	// avoid consecutive repeated receipts
	const int NUMOFENTRIES  = 5000;
	const int HALFOFENTRIES = NUMOFENTRIES/2;

	DWORD *pNumbers = new DWORD[NUMOFENTRIES];
	memset(pNumbers, 0, sizeof(DWORD)*NUMOFENTRIES);
	WORD nNumbers = 0;

	Receipt receipt;
	int nRead;

	while (!eof(DataFile))
	{
		nRead = read(DataFile, &receipt, sizeof(Receipt));
		if
		(
			nRead == sizeof(Receipt)   			&&
			receipt.MagicNumber == MAGIC_NUMBER &&
			receipt.Number > 0                  &&
			receipt.Number < MAX_RECEIPTS
		)
		{
			if (!g_LongLFind(receipt.Number, pNumbers, nNumbers))
			{
				if (nNumbers == NUMOFENTRIES)
				{
					// clean half of the vector and start again
					nNumbers = HALFOFENTRIES;
					memcpy(pNumbers, &pNumbers[HALFOFENTRIES], sizeof(DWORD)*HALFOFENTRIES);
					memset(&pNumbers[HALFOFENTRIES], 0, sizeof(DWORD)*HALFOFENTRIES);
				}

				nNumbers++;
				pNumbers[nNumbers-1] = receipt.Number;

				if (write(tmpFile, &receipt, sizeof(Receipt)) != sizeof(Receipt))
				{
					delete [] pNumbers;
					close(tmpFile);
					return FALSE;
				}
			}
#if defined(__TEST__)
#if !defined(__UTIL__)
			gotoxy(x, y);
			cprintf("%ld", receipt.Number);
#endif
#endif
		}
	}

	delete [] pNumbers;

	// set tmp as new data file
	close(DataFile);
	if (access(DataFilename, 6) != 0)
		chmod(DataFilename, S_IREAD|S_IWRITE); // enable read/write

	unlink(DataFilename);
	close(tmpFile);
	rename(tmpFilename, DataFilename);

	DataFile = open(DataFilename, O_RDWR|O_BINARY, S_IREAD|S_IWRITE);

	Flush();

	return TRUE;
}

BOOL BinStorage::RepairIndexFile(void)
{
	Flush(); // to be sure
	// assume that index file is bad
	// --- recreate index
	close(IndexFile);
	// Init datafileHeader
	m_indexHeader.NumOfEntries = 0;
	m_indexHeader.LowerNumber  = 0;
	m_indexHeader.HigherNumber = 0;
	//
	if (access(IndexFilename, 6) != 0)
		chmod(IndexFilename, S_IREAD|S_IWRITE); // enable read/write

	IndexFile = creat(IndexFilename, S_IREAD|S_IWRITE);
	if (IndexFile == -1)
		return FALSE;

	if (!WriteIndexHeader(m_indexHeader))
		return FALSE;

	// take from data file and Recover index
#if defined(__TEST__)
#if !defined(__UTIL__)
	cprintf("\n\r- Index file, Record: ");
	short x = wherex(), y = wherey();
#endif
#endif
	lseek(DataFile, sizeof(DataHeader), SEEK_SET);

	Receipt receipt;
	int nRead;
	long seekPos;
	IndexEntry indexEntry;

	while (!eof(DataFile))
	{
		seekPos = tell(DataFile);
		nRead = read(DataFile, &receipt, sizeof(Receipt));
		if
		(
			nRead == sizeof(Receipt)   			&&
			receipt.MagicNumber == MAGIC_NUMBER &&
			receipt.Number > 0                  &&
			receipt.Number < MAX_RECEIPTS
		)
		{
			// index file
			indexEntry.MagicNumber = MAGIC_NUMBER;
			indexEntry.Number      = receipt.Number;
			indexEntry.BoothNumber = receipt.BoothNumber;
			indexEntry.SeekPos     = seekPos;
			if (write(IndexFile, &indexEntry, sizeof(IndexEntry)) != sizeof(IndexEntry))
				return FALSE;
			// datafileHeader
			if (!m_indexHeader.LowerNumber)
				m_indexHeader.LowerNumber = indexEntry.Number;

			m_indexHeader.HigherNumber = indexEntry.Number;
			m_indexHeader.NumOfEntries++; // !!!
#if defined(__TEST__)
#if !defined(__UTIL__)
			gotoxy(x, y);
			printf("%ld", receipt.Number);
#endif
#endif
		}
	}

	// Rebuild datafileHeader
	lseek(IndexFile, 0, SEEK_SET);

	if (!WriteIndexHeader(m_indexHeader))
		return FALSE;

	Flush();

	return TRUE;
}

BOOL BinStorage::Get(Receipt& receipt, long number, int boothNumber)
{
	long seekPos;
	if (!m_pIndexCache->Find(number, boothNumber, seekPos))
		return FALSE;

	return Read(receipt, seekPos);
}

BOOL BinStorage::Add(const Receipt& receipt)
{
	if (g_cfg->CHECK_DUPS)
	{
		m_pIndexCache->Clear(); // from the beginning

		if (Exist(receipt.Number))
			return FALSE;
	}

	m_pIndexCache->Clear(); // force again the beginning

	/////////////////////////////////////////////
	// DataFile

	long dfSeekPos = lseek(DataFile, 0, SEEK_END);
	if (dfSeekPos == -1)
		return FALSE;

	// check to see if the end of file is corrupted
	long rem = (dfSeekPos-sizeof(DataHeader))%sizeof(Receipt);

	// we assume a good data file header
	if (rem)
		dfSeekPos -= rem; // fix bound

	if (!Write(receipt, dfSeekPos))
		return FALSE;

	/////////////////////////////////////////////
	// IndexFile

	// write entry
	long ifSeekPos = lseek(IndexFile, 0, SEEK_END);
	if (ifSeekPos == -1)
		return FALSE;

	rem = (ifSeekPos-sizeof(IndexHeader))%sizeof(IndexEntry);
	if (rem)
	{
		ifSeekPos -= rem; // fix a good bound
		ifSeekPos = lseek(IndexFile, ifSeekPos, SEEK_SET);
	}

	if (ifSeekPos == -1)
		return FALSE;

	// prepare index entry
	IndexEntry indexEntry;
	indexEntry.MagicNumber  = MAGIC_NUMBER;
	indexEntry.SeekPos 		= dfSeekPos;
	indexEntry.Number  		= receipt.Number;
	indexEntry.BoothNumber  = receipt.BoothNumber;
	//
	int nWritten = write(IndexFile, &indexEntry, sizeof(IndexEntry));
	if (nWritten != sizeof(IndexEntry))
		return FALSE;

	// update index header
	if (!m_indexHeader.LowerNumber)
		m_indexHeader.LowerNumber = receipt.Number;

	m_indexHeader.HigherNumber  = receipt.Number;
	m_indexHeader.NumOfEntries++;

	if (!WriteIndexHeader(m_indexHeader))
		return FALSE;

	return TRUE;
}

BOOL BinStorage::Delete(long number, int boothNumber)
{
    long seekPos;
	if (!m_pIndexCache->Find(number, boothNumber, seekPos))
		return FALSE;

	Receipt receipt;
    if (!Read(receipt, seekPos))
		return FALSE;

	receipt.Stat.Deleted = TRUE;

	return Write(receipt, seekPos);
}

BOOL BinStorage::Update(const Receipt& receipt)
{
	long seekPos;
	if (!m_pIndexCache->Find(receipt.Number, receipt.BoothNumber, seekPos))
	    return FALSE;

	return Write(receipt, seekPos);
}

BOOL BinStorage::IsCorrectNumber(long number)
{
	// check range, remember the wrap around efect !!!
	if (number > 0 && number <= MAX_RECEIPTS)
	{ // out of range
		if (m_indexHeader.LowerNumber > m_indexHeader.HigherNumber)
		{ // wrap ?
			if (m_indexHeader.LowerNumber <= number || number <= m_indexHeader.HigherNumber)
				return TRUE;
		}
		else // normal ranges
			if (m_indexHeader.LowerNumber <= number && number <= m_indexHeader.HigherNumber)
				return TRUE;
	}
	return FALSE;
}

BOOL BinStorage::ReadDataHeader(DataHeader & header)
{
	if (lseek(DataFile, 0L, SEEK_SET) == -1)
		return FALSE;

	int nRead = read(DataFile, &header, sizeof(DataHeader));
	return
	(
		nRead == sizeof(DataHeader) &&
		header.FileHeader.IsValid()
	);
}

BOOL BinStorage::WriteDataHeader(DataHeader const & header)
{
	if (lseek(DataFile, 0L, SEEK_SET) == -1)
		return FALSE;

	int nWritten = write(DataFile, &header, sizeof(DataHeader));
	return (nWritten == sizeof(DataHeader));
}

BOOL BinStorage::Read(Receipt& receipt, long seekPos)
{
	if (lseek(DataFile, seekPos, SEEK_SET) == -1)
		return FALSE;

	int nRead = read(DataFile, &receipt, sizeof(Receipt));
	return
	(
		(nRead == sizeof(Receipt))  &&
		IsValid(receipt) 			&&
		!receipt.Stat.Deleted
	);
}

BOOL BinStorage::Write(Receipt const& receipt, long seekPos)
{
	if (lseek(DataFile, seekPos, SEEK_SET) == -1)
		return FALSE;

	int nWritten;
	nWritten = write(DataFile, &receipt, sizeof(Receipt));

	return (nWritten == sizeof(Receipt));
}

BOOL BinStorage::ReadIndexHeader(IndexHeader & header)
{
	if (lseek(IndexFile, 0L, SEEK_SET) == -1)
		return FALSE;

	int nRead = read(IndexFile, &header, sizeof(IndexHeader));
	return
	(
		nRead == sizeof(IndexHeader) &&
		header.FileHeader.IsValid()
	);
}

BOOL BinStorage::WriteIndexHeader(IndexHeader const & header)
{
	if (lseek(IndexFile, 0L, SEEK_SET) == -1)
		return FALSE;

	int nWritten = write(IndexFile, &header, sizeof(IndexHeader));
	return (nWritten == sizeof(IndexHeader));
}

/////////////////////////////////////////////////////////////////////
// DATA HEADER
/////////////////////////////////////////////////////////////////////

BinStorage::DataHeader::DataHeader(void)
{
	taxPercent 	= g_cfg->TAX_PERCENT;
	ddnPercent 	= g_cfg->DDN_TAX;
	ddiPercent 	= g_cfg->DDI_TAX;
	NA_01		= 0.0;
	internetTax	= g_cfg->INTERNET_TAX;
	round 		= (int)g_cfg->M_ROUND; // compatibility
	m_round 	= g_cfg->M_ROUND; // new round 2.20.7

	for (int i = 0; i < MAX_MAGNETIC_CARDS; i++)
		mcardsValues[i] = g_cfg->MCARD[i];
}

/////////////////////////////////////////////////////////////////////
// 	IndexCache
/////////////////////////////////////////////////////////////////////

BinStorage::IndexCache::IndexCache(BinStorage &dbStorage, WORD size)
	:
	m_dbStorage(dbStorage),
	m_size(size),
	m_curPos(0),
	m_lastSeekPos(sizeof(IndexHeader)),
	m_actualSize(0)
{
	m_pEntries = new IndexEntry[m_size];
}

BinStorage::IndexCache::~IndexCache()
{
	delete [] m_pEntries;
}

BOOL BinStorage::IndexCache::Clear()
{
	m_actualSize = 0;
	m_curPos 	 = 0;

	memset(m_pEntries, 0, sizeof(IndexEntry)*m_size);

	m_lastSeekPos = sizeof(IndexHeader); // restart

	return TRUE;
}

long BinStorage::IndexCache::Current()
{
	return m_pEntries[m_curPos].Number;
}

long BinStorage::IndexCache::ActualSize()
{
	return m_actualSize;
}

BOOL BinStorage::IndexCache::Find(long number, int boothNumber, long& seekPos)
{
	// begin 2.21.8
	/////////////////////////////////////////////////////////////////
	// find based on number and boot number. Returns file seek position.
	/////////////////////////////////////////////////////////////////

	BOOL found = FALSE;

	/////////////////////////////////////////////////////////////////
	// look into cache first

	found = CacheFind(number);
	if (found)
	{
		if (boothNumber != -1) // looking for a specific booth?
			found = m_pEntries[m_curPos].BoothNumber == boothNumber;

		if (found)
			seekPos = m_pEntries[m_curPos].SeekPos;

		return found;
	}

	/////////////////////////////////////////////////////////////////
	// reload cache until it can find it or no more receipts
	long savedSeekPos = m_lastSeekPos;

	do
	{
		if (!Load())
			return FALSE; // definitely we have a problem

		found = CacheFind(number);
		if (found)
		{
			if (boothNumber != -1) // looking for a specific booth?
				found = m_pEntries[m_curPos].BoothNumber == boothNumber;

			if (found)
				seekPos = m_pEntries[m_curPos].SeekPos;

			return found;
		}
	}
	while (savedSeekPos != m_lastSeekPos);

	return FALSE; // if we get here is because we couldn't find it
	// end 2.21.8
}

BOOL BinStorage::IndexCache::CacheFind(long number)
{
	// find inside cache. Returns pos in cache
	m_curPos = 0;

	for (int i = 0; i < m_actualSize; ++i)
	{
		if (m_pEntries[i].Number == number)
		{
			m_curPos = i;
			return TRUE;
		}
	}

	return FALSE;
}

long BinStorage::IndexCache::FindLowerNumber()
{
	// begin 2.21.8 build 6

	long first;

	first = FindFirstNumber();

	if (!first)
		return 0L;

	long lower, number;

	number = lower = first;

	// end 2.21.8 build 6

	do
	{
		if (number < lower)
			lower = number;

		number = FindNextNumber();
	}
	while (number && number != first);

	return lower;
}

long BinStorage::IndexCache::FindHigherNumber()
{
	// begin 2.21.8 build 6

	long first;

	first = FindFirstNumber();

	if (!first)
		return 0L;

	long higher, number;

	number = higher = first;

	// end 2.21.8 build 6

	do
	{
		if (number > higher)
			higher = number;

		number = FindNextNumber();
	}
	while (number && number != first);

	return higher;
}

long BinStorage::IndexCache::FindFirstNumber()
{
	Clear(); // start from the beginning

	Load();

	return m_pEntries[0].Number;
}

long BinStorage::IndexCache::FindLastNumber()
{
	// begin 2.21.8 build 6

	long first;

	first = FindFirstNumber();

	if (!first)
		return 0L;

	long last, number;

	number = last = first;

	// end 2.21.8 build 6

	do
	{
		last   = number;
		number = FindNextNumber();
	}
	while (number && number != first);

	return last;
}

long BinStorage::IndexCache::FindNextNumber()
{
	/////////////////////////////////////////////////////////////////
	// cache

	if ((m_curPos+1) < m_actualSize)
	{
		++m_curPos;

		return m_pEntries[m_curPos].Number;
	}

	/////////////////////////////////////////////////////////////////
	// reload cache

	Load();

	return m_pEntries[0].Number;
}

BOOL BinStorage::IndexCache::Load()
{
	m_curPos     = 0;
	m_actualSize = 0;

	if (lseek(m_dbStorage.IndexFile, m_lastSeekPos, SEEK_SET) == -1)
		return FALSE;

	memset(m_pEntries, 0, sizeof(IndexEntry)*m_size);

	m_actualSize = read(m_dbStorage.IndexFile, m_pEntries, sizeof(IndexEntry)*m_size)/sizeof(IndexEntry);

	if (eof(m_dbStorage.IndexFile))
		m_lastSeekPos = sizeof(IndexHeader); // wrap around
	else
		m_lastSeekPos = tell(m_dbStorage.IndexFile);

	return TRUE; // 2.21.8 Build 6
}

