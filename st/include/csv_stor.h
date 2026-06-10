#ifndef __CSV_STOR_H
#define __CSV_STOR_H

#if !defined(__DB_STOR_H)
#include <db_stor.h>
#endif

#if !defined(__STDLIB_H)
#include <stdlib.h>
#endif

// ----------------------------------------------------------------------------
// [ CsvStorage ]
//
// DB_STORAGE_BACKEND that stores Receipt records as CSV rows in a single
// text file.  The entire dataset is held in memory (demo-scale counts),
// written to disk on Flush() / destruction.
//
// CSV format (one header row, one data row per receipt):
//   Number,MagicNumber,Tag,BoothNumber,Date,Time,City,Phone,Amount,
//   ElapsedTime,ValuePerMin,CeilMin,Percent,Value,Tax,Tax2,DDummy,
//   nStat,bExtendedStat
//
// See MINI_SMARTTAR_PLAN § 2.1b.
// ----------------------------------------------------------------------------

class CsvStorage : public DB_STORAGE_BACKEND
{
public:
	CsvStorage(const char *path, const char *filename, int readOnly = TRUE);
	~CsvStorage();

	// --- DB_STORAGE_BACKEND interface ---------------------------------------

	BOOL Get(Receipt& receipt, long number, int boothNumber = -1);
	BOOL Add(const Receipt& receipt);
	BOOL Update(const Receipt& receipt);
	BOOL Delete(long number, int boothNumber = -1);
	void EnumReceipts(CallbackFnPtr callback);

	long GetEntries()      const;
	long GetLowerNumber()  const;
	long GetHigherNumber() const;
	long GetFirstNumber()  const;
	long GetLastNumber()   const;
	BOOL Exist(long number, int boothNumber = -1) const;

	void Flush();
	BOOL Archive();
	BOOL Repair();

	int  GetStatus()   { return m_status; }
	BOOL IsReadOnly()  { return m_readOnly; }

	enum STAT
	{
		OK           = 0x0000,
		NO_DATA_FILE = 0x0001,
		BAD_DATA_FILE= 0x0002
	};

private:

	struct Entry
	{
		Receipt receipt;
		BOOL    deleted;
	};

	Entry  *m_entries;     // dynamic array
	long    m_count;       // number of entries (including deleted)
	long    m_capacity;    // allocated capacity
	int     m_status;
	BOOL    m_readOnly;
	BOOL    m_dirty;       // unsaved changes
	char    m_path[256];
	char    m_filename[80];

	// Rebuild cached metadata (lower/higher/first/last numbers)
	void RebuildMeta();

	long  m_lowerNumber;
	long  m_higherNumber;
	long  m_firstNumber;
	long  m_lastNumber;

	// Find index of entry matching (number, boothNumber), or -1
	int Find(long number, int boothNumber) const;

	// Grow the entry array to accommodate at least one more entry
	BOOL Grow();

	void Load();
	void Save();
};

#endif // __CSV_STOR_H
