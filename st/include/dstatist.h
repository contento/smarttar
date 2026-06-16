#ifndef __DSTATIST_H
#define __DSTATIST_H

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

#if !defined(__DS_ENTRY_H)
#include <ds_entry.h>
#endif

#if !defined(__DSTORAGE_H)
#include <dstorage.h>
#endif

#if !defined(__FILEHDR_H)
#include <filehdr.h>
#endif

#if !defined(__ISTATIST_H)
#include <istatist.h>
#endif

// ----------------------------------------------------------------------------
// [ DB_STATISTICS ]
// ----------------------------------------------------------------------------

class DB_STATISTICS : public IStatisticsStorage
{
public:
    DB_STATISTICS(const char *path, const char *name, WORD readOnly = TRUE);
    ~DB_STATISTICS();
    //
	enum STAT_TAG
	{
        OK        = 0x0000,
        NO_FILE   = 0x0001,
        BAD_FILE  = 0x0020,
        NEW       = 0x2000
    };
    enum TYPETAG { YEAR, MONTH, WEEK, DAY, TURN };
    //
    void Flush(void);
	BOOL Archive(void);
	BOOL Repair(IReceiptStorage *dBStorage, BOOL all = FALSE);
	DS_ENTRY *operator [](WORD type)
	{
		return &Entries[type];
	}
	DS_DOUBLEPRNENTRY *GetDoublePrnEntry(WORD ofs)
	{
		return &DoublePRNEntries[ofs];
	}
	DS_CELLULARENTRY  *GetCellularEntry (WORD type)
	{
		return &CellularEntries[type];
	}

	//  DB_STATISTICS& Add(Receipt& receipt, BOOL isNew = TRUE);
	//	DB_STATISTICS& Subtract(Receipt& receipt);
	BOOL Add	 (Receipt& receipt, BOOL isNew = TRUE);
	BOOL Subtract(Receipt& receipt);
	BOOL Update(void);

	inline void SetErrors(WORD dialErrors, WORD commErrors);
	inline long GetTelEntries(void);
	WORD  GetStatus(void)
	{
        return Status;
    }
    BOOL IsReadOnly(void)
    {
        return ReadOnly;
    }
private:
    struct HEADER
    {
        HEADER(void)
        {
            memset(Dummy, 0, sizeof(Dummy));
        }
        FILE_HEADER FileHeader;
        // for backward compatibility
        // keep record of changes. GCC/gcc.
        // v 1.0 : Size of Dummy = 0x20
        char Dummy[0x20];
    };
    FILE_NAME Filename;
    int  File;
    BOOL ReadOnly;
    WORD Status;
	// this is the order inside the file
    HEADER            *Header;
    DS_ENTRY          *Entries;
    DS_DOUBLEPRNENTRY *DoublePRNEntries;
    DS_CELLULARENTRY  *CellularEntries;
};

#endif // __DSTATIST_H
