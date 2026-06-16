#ifndef __DSTORAGE_H
#define __DSTORAGE_H

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

#if !defined(__STDLIB_H)
#include <stdlib.h>
#endif

#if !defined(__FILEHDR_H)
#include <filehdr.h>
#endif

#if !defined(__RECEIPT_H)
#include <receipt.h>
#endif
#include <ireceipt.h>

class DB_STORAGE : public IReceiptStorage
{
	class 	Iterator;
	friend 	Iterator;

public:
	DB_STORAGE(const char *path, const char *filename, int readOnly = TRUE);
	~DB_STORAGE();
	//
	typedef BOOL (*CallbackFnPtr)(Receipt const &receipt);
	void EnumReceipts(CallbackFnPtr callback);

	int  GetStatus(void);
	BOOL IsReadOnly(void);

	BOOL Get(Receipt& receipt, long number, int boothNumber = -1);
	BOOL Add(const Receipt& receipt);
	BOOL Update(const Receipt& receipt);
	BOOL Delete(long number, int boothNumber = -1);

	// Important: Don't use neither long++ nor long--;
	//
	BOOL IsCorrectNumber(long number);
	BOOL IsValid(Receipt const & receipt);

	BOOL Exist(long number, int boothNumber = -1) const;

	long GetLowerNumber() const;
	long GetHigherNumber() const;

	long GetEntries(void) const;
	long GetFirstNumber()  const;

private:
	long GetNextNumber() const;

public:

	long GetLastNumber()  const;

	//
	void Flush(void);
	BOOL Archive(void);
	inline BOOL Repair();

public:

	class Iterator // 2.21.8 Build 6
	{
	public:
		Iterator(DB_STORAGE const & dbStorage);

		long Current();
		long Restart(long nNumber = 0L);

		operator int();
		long operator ++();
		long operator ++(int);

	private:

		DB_STORAGE const & m_dbStorage;
		long m_number;
		long m_firstNumber;
		BOOL m_bLast;
	};

public:

	enum STAT
	{
		OK             = 0x0000,
		NO_DATA_FILE   = 0x0001,
		BAD_DATA_FILE  = 0x0002,
		NEW_DATA_FILE  = 0x0004,
		NO_INDEX_FILE  = 0x0010,
		BAD_INDEX_FILE = 0x0020,
		NEW_INDEX_FILE = 0x2040
	};

	static const long MAX_RECEIPTS;
	static const UINT MAGIC_NUMBER;

private:

	class IndexEntry
	{
	public:
		UINT MagicNumber; // to know if the record is valid
		//
		long Number;      // primary key
		int  BoothNumber; // secondary key
		long SeekPos;     // position into data file
	};

private:

	class DataHeader
	{
	public:
		DataHeader(void);
		FILE_HEADER FileHeader;
		//
		double taxPercent;
		double ddnPercent;
		double ddiPercent;
		double mcardsValues[MAX_MAGNETIC_CARDS];
		double internetTax; // 2.30
		double NA_01; // 2.30
		WORD   round;
		// 2.20.7 changes round from integer to double
		double m_round;
		// for backward compatibility
		// keep record of changes. GCC/gcc.
		// v 1.0 : Size of Dummy = 0x20

		/* Since 2.20.7 this dummy is size = 0
		char Dummy[
			0x52
			- sizeof(double) * (5+MAX_MAGNETIC_CARDS)
			- sizeof(WORD)
			- sizeof(double) // 2.20.7
			];
		*/
	};

	class IndexHeader
	{
	public:
		IndexHeader(void)
		{
			NumOfEntries = 0;
			LowerNumber = 0;
			HigherNumber = 0;
			memset(Dummy  , 0, sizeof(Dummy));
		}
		FILE_HEADER FileHeader;
		//
		long NumOfEntries; // number of entries
		long LowerNumber; // lower number
		long HigherNumber; // higher number
		// for backward compatibility
		// keep record of changes. GCC/gcc.
		// v 1.0 : Size of Dummy = 0x20
		char Dummy[0x20];
	};

	BOOL RepairDataFile(void);
	BOOL CompactDataFile(const char *dstPath);
	BOOL RepairIndexFile(void);

	BOOL ReadDataHeader(DataHeader & header);
	BOOL WriteDataHeader(DataHeader const & header);
	BOOL Read(Receipt& receipt, long seekPos);
	BOOL Write(Receipt const& receipt, long seekPos);

	BOOL ReadIndexHeader(IndexHeader & header);
	BOOL WriteIndexHeader(IndexHeader const & header);

	class IndexCache
	{
	public:
		IndexCache(DB_STORAGE &dbStorage, WORD size);
		~IndexCache();

		long Current();
		long ActualSize();

		BOOL Find(long number, int boothNumber, long & seekPos);

		long FindLowerNumber();
		long FindHigherNumber();

		long FindFirstNumber();
		long FindNextNumber ();
		long FindLastNumber();

		BOOL Clear();

	private:
		BOOL CacheFind(long number); // inside actual cache
		BOOL Load();

		DB_STORAGE& m_dbStorage;

		WORD  		m_size;
		WORD  		m_actualSize;
		int   		m_curPos;
		long  		m_lastSeekPos;

		IndexEntry  *m_pEntries;
	};

private:

	FILE_NAME DataFilename;
	FILE_NAME IndexFilename;
	int  Status;
	BOOL ReadOnly;

	int DataFile;
	int IndexFile;

	DataHeader  m_dataHeader;
	IndexHeader m_indexHeader;

	friend IndexCache;
	IndexCache* 	m_pIndexCache;
};

inline int  DB_STORAGE::GetStatus(void)
{
	return Status;
}
inline BOOL DB_STORAGE::IsReadOnly(void)
{
	return ReadOnly;
}

inline BOOL DB_STORAGE::Exist(long number, int boothNumber) const
{
	long seekPos;
	return m_pIndexCache->Find(number, boothNumber, seekPos);
}

inline long DB_STORAGE::GetEntries() const
{
	return m_indexHeader.NumOfEntries;
}

inline long DB_STORAGE::GetLowerNumber() const
{
	// not necessarily the first one !!!
	return m_pIndexCache->FindLowerNumber();
}

inline long DB_STORAGE::GetHigherNumber() const
{
	// not necessarily the last one !!!
	return m_pIndexCache->FindHigherNumber();
}

inline long DB_STORAGE::GetFirstNumber() const
{
	return m_pIndexCache->FindFirstNumber();
}

inline long DB_STORAGE::GetLastNumber() const
{
	return m_pIndexCache->FindLastNumber();
}

inline long DB_STORAGE::GetNextNumber() const
{
	return m_pIndexCache->FindNextNumber();
}

inline BOOL DB_STORAGE::Repair()
{
	return
	(
		RepairDataFile() &&
		RepairIndexFile()
	);
}

/////////////////////////////////////////////////////////////////////
// DB_STORAGE::Iterator
/////////////////////////////////////////////////////////////////////

inline DB_STORAGE::Iterator::Iterator(DB_STORAGE const & dbStorage)
	:
	m_dbStorage(dbStorage),
	m_number(0L),
	m_firstNumber(0L),
	m_bLast(FALSE)
{
	Restart();
}

inline long DB_STORAGE::Iterator::Current()
{
	return m_number;
}

inline long DB_STORAGE::Iterator::Restart(long nNumber)
{
	m_firstNumber = m_dbStorage.GetFirstNumber();;

	if (nNumber)
	{
		long seekPos;
		if (m_dbStorage.Exist(nNumber))
			m_number = nNumber;
		else
			m_number = m_firstNumber;
	}
	else
		m_number = m_firstNumber;

	m_bLast = (m_number == 0L)?TRUE:FALSE;

	return m_number;
}

inline DB_STORAGE::Iterator::operator int()
{
	return !m_bLast;
}

inline long DB_STORAGE::Iterator::operator ++()
{
	m_number = m_dbStorage.GetNextNumber();

	m_bLast = (m_number == m_firstNumber);

	return m_number;
}

inline long DB_STORAGE::Iterator::operator ++(int)
{
	long number = m_number;
	m_number = m_dbStorage.GetNextNumber();

	m_bLast = (m_number == m_firstNumber);

	return number;
}

#endif // __DSTORAGE_H
