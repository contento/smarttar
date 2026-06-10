//
// [ CSV_STOR.CPP ]
//
// CsvStorage — CSV receipt storage backend for demo builds.
// Entire dataset held in memory; flushed to a single .csv file on save.
// See MINI_SMARTTAR_PLAN § 2.1b.
//

#include "stdst.h"

#include <csv_stor.h>
#include <string.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// Format helpers avoid sprintf format strings in the translator's path.
// Keeping them together so a future port can replace the I/O layer cleanly.

static void writeReceipt(FILE *fp, Receipt const &r)
{
	fprintf(fp, "%ld,%u,%d,%d,%d,%d,%s,%s,%d,%ld,%.2lf,%.2lf,%d,%.2lf,%.2lf,%.2lf,%.2lf,%u,%u\n",
		r.Number,
		(unsigned)r.MagicNumber,
		(int)r.Tag,
		r.BoothNumber,
		r.Date,
		r.Time,
		r.City,
		r.Phone,
		r.Amount,
		r.ElapsedTime,
		r.ValuePerMin,
		r.CeilMin,
		r.Percent,
		r.Value,
		r.Tax,
		r.Tax2,
		r.DDummy,
		(unsigned)r.nStat,
		(unsigned)r.bExtendedStat);
}

static int readReceipt(FILE *fp, Receipt &r)
{
	// Read one CSV line and parse into receipt fields.
	// City and Phone are fixed-width char arrays that may contain commas
	// or be empty; we read them as %20[^,] / %17[^,] bounded strings.
	int n = fscanf(fp, " %ld,%u,%d,%d,%d,%d,%20[^,],%17[^,],%d,%ld,%lf,%lf,%d,%lf,%lf,%lf,%lf,%u,%u\n",
		&r.Number,
		(unsigned *)&r.MagicNumber,
		(int *)&r.Tag,
		&r.BoothNumber,
		&r.Date,
		&r.Time,
		r.City,
		r.Phone,
		&r.Amount,
		&r.ElapsedTime,
		&r.ValuePerMin,
		&r.CeilMin,
		&r.Percent,
		&r.Value,
		&r.Tax,
		&r.Tax2,
		&r.DDummy,
		(unsigned *)&r.nStat,
		(unsigned *)&r.bExtendedStat);
	return n == 19;
}

// ---------------------------------------------------------------------------
// Construction / destruction
// ---------------------------------------------------------------------------

CsvStorage::CsvStorage(const char *path, const char *filename, int readOnly)
	: m_entries(NULL)
	, m_count(0L)
	, m_capacity(0L)
	, m_status(OK)
	, m_readOnly(readOnly)
	, m_dirty(FALSE)
	, m_lowerNumber(0L)
	, m_higherNumber(0L)
	, m_firstNumber(0L)
	, m_lastNumber(0L)
{
	if (path)
		strcpy(m_path, path);
	else
		m_path[0] = '\0';
	strcpy(m_filename, filename);

	if (!m_readOnly)
		Load();
}

CsvStorage::~CsvStorage()
{
	if (!m_readOnly && m_dirty)
		Flush();
	if (m_entries)
		free(m_entries);
}

// ---------------------------------------------------------------------------
// Core CRUD
// ---------------------------------------------------------------------------

int CsvStorage::Find(long number, int boothNumber) const
{
	for (long i = 0; i < m_count; i++)
	{
		if (!m_entries[i].deleted &&
			m_entries[i].receipt.Number == number &&
			(boothNumber < 0 || m_entries[i].receipt.BoothNumber == boothNumber))
			return (int)i;
	}
	return -1;
}

BOOL CsvStorage::Grow()
{
	long newCap = m_capacity ? m_capacity * 2 : 64L;
	Entry *p = (Entry *)realloc(m_entries, (size_t)newCap * sizeof(Entry));
	if (!p)
		return FALSE;
	m_entries = p;
	m_capacity = newCap;
	return TRUE;
}

BOOL CsvStorage::Get(Receipt& receipt, long number, int boothNumber)
{
	int idx = Find(number, boothNumber);
	if (idx < 0)
		return FALSE;
	receipt = m_entries[idx].receipt;
	return TRUE;
}

BOOL CsvStorage::Add(const Receipt& receipt)
{
	if (m_readOnly)
		return FALSE;
	if (m_count >= m_capacity && !Grow())
		return FALSE;

	m_entries[m_count].receipt = receipt;
	m_entries[m_count].deleted = FALSE;
	m_count++;
	m_dirty = TRUE;
	RebuildMeta();
	return TRUE;
}

BOOL CsvStorage::Update(const Receipt& receipt)
{
	if (m_readOnly)
		return FALSE;
	int idx = Find(receipt.Number, receipt.BoothNumber);
	if (idx < 0)
		return FALSE;
	m_entries[idx].receipt = receipt;
	m_dirty = TRUE;
	return TRUE;
}

BOOL CsvStorage::Delete(long number, int boothNumber)
{
	if (m_readOnly)
		return FALSE;
	int idx = Find(number, boothNumber);
	if (idx < 0)
		return FALSE;
	m_entries[idx].deleted = TRUE;
	m_dirty = TRUE;
	RebuildMeta();
	return TRUE;
}

void CsvStorage::EnumReceipts(CallbackFnPtr callback)
{
	for (long i = 0; i < m_count; i++)
	{
		if (!m_entries[i].deleted)
		{
			if (!callback(m_entries[i].receipt))
				return;
		}
	}
}

BOOL CsvStorage::IsValid(Receipt const &receipt)
{
	return receipt.Number > 0L && receipt.Number <= MAX_RECEIPTS;
}


// ---------------------------------------------------------------------------
// Metadata
// ---------------------------------------------------------------------------

void CsvStorage::RebuildMeta()
{
	m_lowerNumber  = 0L;
	m_higherNumber = 0L;
	m_firstNumber  = 0L;
	m_lastNumber   = 0L;

	for (long i = 0; i < m_count; i++)
	{
		if (m_entries[i].deleted)
			continue;
		long n = m_entries[i].receipt.Number;
		if (m_firstNumber == 0L || n < m_lowerNumber)
			m_lowerNumber = n;
		if (m_higherNumber == 0L || n > m_higherNumber)
			m_higherNumber = n;
	}
	// first and last are the min/max by sequence order
	for (long j = 0; j < m_count; j++)
	{
		if (!m_entries[j].deleted)
		{
			m_firstNumber = m_entries[j].receipt.Number;
			break;
		}
	}
	for (long k = m_count - 1; k >= 0; k--)
	{
		if (!m_entries[k].deleted)
		{
			m_lastNumber = m_entries[k].receipt.Number;
			break;
		}
	}
}

long CsvStorage::GetEntries() const
{
	long n = 0L;
	for (long i = 0; i < m_count; i++)
	{
		if (!m_entries[i].deleted)
			n++;
	}
	return n;
}

long CsvStorage::GetLowerNumber()  const { return m_lowerNumber; }
long CsvStorage::GetHigherNumber() const { return m_higherNumber; }
long CsvStorage::GetFirstNumber()  const { return m_firstNumber; }
long CsvStorage::GetLastNumber()   const { return m_lastNumber; }

BOOL CsvStorage::Exist(long number, int boothNumber) const
{
	return Find(number, boothNumber) >= 0;
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

void CsvStorage::Flush()
{
	if (m_readOnly || !m_dirty)
		return;
	Save();
	m_dirty = FALSE;
}

BOOL CsvStorage::Archive()
{
	// Rename the data file (append .bak) and clear the in-memory store.
	if (m_readOnly)
		return FALSE;
	Flush();

	char oldName[256 + 80];
	char bakName[256 + 80];
	if (m_path[0])
		sprintf(oldName, "%s%s.csv", m_path, m_filename);
	else
		sprintf(oldName, "%s.csv", m_filename);
	sprintf(bakName, "%s.bak", oldName);
	remove(bakName);
	rename(oldName, bakName);

	// Reset in-memory state
	if (m_entries)
		free(m_entries);
	m_entries    = NULL;
	m_count      = 0L;
	m_capacity   = 0L;
	m_dirty      = FALSE;
	m_status     = OK;
	RebuildMeta();
	return TRUE;
}

BOOL CsvStorage::Repair()
{
	if (m_readOnly)
		return FALSE;
	// Rebuild meta from the in-memory array (which is the source of truth).
	// Compact: remove deleted entries.
	long writeIdx = 0L;
	for (long i = 0L; i < m_count; i++)
	{
		if (!m_entries[i].deleted)
		{
			if (writeIdx != i)
				m_entries[writeIdx] = m_entries[i];
			writeIdx++;
		}
	}
	m_count = writeIdx;
	m_dirty = TRUE;
	RebuildMeta();
	Flush(); // persist compaction
	return TRUE;
}

// ---------------------------------------------------------------------------
// Persistence
// ---------------------------------------------------------------------------

void CsvStorage::Load()
{
	char fullName[256 + 80];
	if (m_path[0])
		sprintf(fullName, "%s%s.csv", m_path, m_filename);
	else
		sprintf(fullName, "%s.csv", m_filename);

	FILE *fp = fopen(fullName, "rt");
	if (!fp)
	{
		m_status = NO_DATA_FILE;
		return;
	}

	// Skip header line
	STR512 header;
	fgets(header, sizeof(header), fp);

	Receipt r;
	while (readReceipt(fp, r))
	{
		if (m_count >= m_capacity && !Grow())
			break;
		m_entries[m_count].receipt = r;
		m_entries[m_count].deleted = FALSE;
		m_count++;
	}
	fclose(fp);
	m_status = OK;
	RebuildMeta();
	m_dirty = FALSE;
}

void CsvStorage::Save()
{
	char fullName[256 + 80];
	if (m_path[0])
		sprintf(fullName, "%s%s.csv", m_path, m_filename);
	else
		sprintf(fullName, "%s.csv", m_filename);

	FILE *fp = fopen(fullName, "wt");
	if (!fp)
		return;

	// Header row
	fprintf(fp, "Number,MagicNumber,Tag,BoothNumber,Date,Time,City,Phone,Amount,"
		"ElapsedTime,ValuePerMin,CeilMin,Percent,Value,Tax,Tax2,DDummy,nStat,"
		"bExtendedStat\n");

	for (long i = 0L; i < m_count; i++)
	{
		if (!m_entries[i].deleted)
			writeReceipt(fp, m_entries[i].receipt);
	}
	fclose(fp);
	m_status = OK;
}
