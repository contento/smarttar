#ifndef __DB_ENG_H
#define __DB_ENG_H

#if !defined(__IRECEIPT_H)
#include <ireceipt.h>
#endif
#if !defined(__ISTATIST_H)
#include <istatist.h>
#endif

#if !defined(__DRECEIPT_H)
#include <dreceipt.h>
#endif

#if !defined(__DSTATIST_H)
#include <dstatist.h>
#endif

#if !defined(__D_EXT_ST_H)
#include <d_ext_st.h>
#endif

// ----------------------------------------------------------------------------
// [ DB_ENGINE ]
// ----------------------------------------------------------------------------

class DB_ENGINE
{
public:
    DB_ENGINE(void);
    ~DB_ENGINE();
    //
    void Flush(void);
    //
    // --- Current Turn ---
    //
    void EnumReceipts(IReceiptStorage::CallbackFnPtr callback)
    {
    	DBStorage->EnumReceipts(callback);
    }
    BOOL Repair(void);
    BOOL Archive(void);
    BOOL Update(long number, WORD modifier);

	// DBStorage

	IReceiptStorage const & GetDBStorage()
	{
		return *DBStorage;
	}

    BOOL RepairDBStorage(void)
    {
        return DBStorage->Repair();
    }
    long GetEntries()
    {
        return DBStorage->GetEntries();
    }
    long GetLowerNumber()
    {
        return DBStorage->GetLowerNumber();
    }
    long GetHigherNumber()
    {
        return DBStorage->GetHigherNumber();
    }
    long GetFirstNumber()
    {
        return DBStorage->GetFirstNumber();
	}
	long GetLastNumber()
    {
        return DBStorage->GetLastNumber();
    }

	BOOL Get(Receipt& receipt, long number, int boothNumber=-1)
    {
        return DBStorage->Get(receipt, number, boothNumber);
    }
	// DynamicReceipt
	/*
	BOOL operator  +(DynamicReceipt& dynReceipt);
	BOOL operator <<(DynamicReceipt& dynReceipt);
	*/
	// 2.21.8
	BOOL Add   (DynamicReceipt& dynReceipt);
	BOOL Update(DynamicReceipt& dynReceipt);

    // DBStatistics
    DS_ENTRY *operator [](WORD type) const
    {
        return (*DBStatistics)[type];
    }
    BOOL RepairDBStatistics(void)
    {
        return DBStatistics->Repair(DBStorage);
    }
    void SetErrors(WORD dialErrors, WORD commErrors)
    {
        DBStatistics->SetErrors(dialErrors, commErrors);
    }
    long GetTelEntries(void) const
    {
        return DBStatistics->GetTelEntries();
    }
    DS_DOUBLEPRNENTRY *GetDoublePrnEntry(WORD ofs)
    {
        return DBStatistics->GetDoublePrnEntry(ofs);
    }
    DS_CELLULARENTRY  *GetCellularEntry(WORD type)
    {
        return DBStatistics->GetCellularEntry(type);
    }
    //
    // --- Another Turn (archived) ---
    //
	IReceiptStorage const & GetArcDBStorage()
	{
		return *ArcDBStorage;
	}

	BOOL LoadArcDB(WORD date, WORD turn);
	void UnloadArcDB(void);
	BOOL GetArc(Receipt& receipt, long number, int boothNumber=-1)
	{
		return ArcDBStorage->Get(receipt, number, boothNumber);
	}
	long GetArcEntries     (void) const
    {
        return ArcDBStorage->GetEntries();
    }

    long GetArcLowerNumber()
    {
        return ArcDBStorage->GetLowerNumber();
    }
    long GetArcHigherNumber()
    {
        return ArcDBStorage->GetHigherNumber();
    }
    long GetArcFirstNumber (void) const
    {
        return ArcDBStorage->GetFirstNumber();
    }
	long GetArcLastNumber()
    {
        return ArcDBStorage->GetLastNumber();
    }

    DS_ENTRY *GetArcStatistics(WORD type) const
    {
        return (*ArcDBStatistics)[type];
    }
    long GetArcTelEntries(void) const
    {
        return ArcDBStatistics->GetTelEntries();
    }
    DS_CELLULARENTRY *GetArcCellularEntry(WORD type) const
    {
        return ArcDBStatistics->GetCellularEntry(type);
    }
	//
	// Extensions
	//
	DB_STORAGE const & ExtGetDBStorage()
	{
		return *DBExtStorage;
	}

	DXS_NON_CRITICAL_ENTRY *ExtGetNonCriticalEntry(WORD extNum = 0)
	{
		return DBExtStatistics->GetNonCriticalEntry(extNum);
	}
	BOOL ExtPutNonCriticalEntry(WORD extNum, DXS_NON_CRITICAL_ENTRY& entry)
	{
		return DBExtStatistics->PutNonCriticalEntry(extNum, entry);
	}
	DXS_CRITICAL_ENTRY *ExtGetCritical(void)
	{
        return DBExtStatistics->GetCritical();
    }
    BOOL ExtPutCritical(DXS_CRITICAL_ENTRY& critical)
    {
        return DBExtStatistics->PutCritical(critical);
    }
	BOOL ExtDelete(long number, short boothNum = -1)
    {
        return DBExtStorage->Delete(number, boothNum);
    }
    BOOL ExtStore(WORD extNum)
    {
        return DBExtStatistics->Store(extNum);
    }
    BOOL ExtFlush(void);
    BOOL ExtArchive(void);
    BOOL ExtRepair(void);
    long ExtGetEntries     (void) const
	{
        return DBExtStorage->GetEntries();
    }
    long ExtGetLowerNumber()
    {
        return DBExtStorage->GetLowerNumber();
    }
    long ExtGetHigherNumber()
    {
        return DBExtStorage->GetHigherNumber();
    }
    long ExtGetFirstNumber (void) const
    {
        return DBExtStorage->GetFirstNumber();
    }
    long ExtGetLastNumber (void) const
    {
        return DBExtStorage->GetLastNumber();
    }
	BOOL ExtGet(Receipt& receipt, long number, int boothNumber=-1)
    {
        return DBExtStorage->Get(receipt, number, boothNumber);
    }
private:
    // --- current turn
    IReceiptStorage   *DBStorage;
    // extensions
    IStatisticsStorage *DBStatistics;
    DB_STORAGE        *DBExtStorage;
    DB_EXT_STATISTICS *DBExtStatistics;
    //
    void Recover(void); // after an abnormal shutdown
    // --- another turn
    IReceiptStorage   *ArcDBStorage;
    DB_STATISTICS     *ArcDBStatistics;
};

#endif // __DB_ENG_H
