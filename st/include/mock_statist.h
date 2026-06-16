#ifndef __MOCK_STATIST_H
#define __MOCK_STATIST_H

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

#if !defined(__DS_ENTRY_H)
#include <ds_entry.h>
#endif

#if !defined(__ISTATIST_H)
#include <istatist.h>
#endif

#include <string.h>

struct MockStatisticsStorage : public IStatisticsStorage
{
    DS_ENTRY          Entries[DS_MAXENTRIES];
    DS_DOUBLEPRNENTRY DoublePRNEntries[DS_MAXDOUBLEPRNENTRIES];
    DS_CELLULARENTRY  CellularEntries[DS_MAXCELLULARENTRIES];
    WORD status;
    WORD dialErrors;
    WORD commErrors;
    BOOL readOnly;

    MockStatisticsStorage();
    virtual ~MockStatisticsStorage() {}

    virtual WORD GetStatus();
    virtual BOOL IsReadOnly();
    virtual void Flush();
    virtual BOOL Archive();
    virtual BOOL Repair(IReceiptStorage *receiptStorage, BOOL all = FALSE);

    virtual DS_ENTRY          *operator[](WORD type);
    virtual DS_DOUBLEPRNENTRY *GetDoublePrnEntry(WORD ofs);
    virtual DS_CELLULARENTRY  *GetCellularEntry(WORD type);

    virtual BOOL Add(Receipt & receipt, BOOL isNew = TRUE);
    virtual BOOL Subtract(Receipt & receipt);
    virtual BOOL Update();

    virtual void SetErrors(WORD newDialErrors, WORD newCommErrors);
    virtual long GetTelEntries();
};

inline MockStatisticsStorage::MockStatisticsStorage()
    : status(0), dialErrors(0), commErrors(0), readOnly(FALSE)
{
    memset(Entries,           0, sizeof(Entries));
    memset(DoublePRNEntries,  0, sizeof(DoublePRNEntries));
    memset(CellularEntries,   0, sizeof(CellularEntries));
}

inline WORD MockStatisticsStorage::GetStatus()
{
    return status;
}

inline BOOL MockStatisticsStorage::IsReadOnly()
{
    return readOnly;
}

inline void MockStatisticsStorage::Flush()
{
}

inline BOOL MockStatisticsStorage::Archive()
{
    return TRUE;
}

inline BOOL MockStatisticsStorage::Repair(IReceiptStorage *, BOOL)
{
    return TRUE;
}

inline DS_ENTRY *MockStatisticsStorage::operator[](WORD type)
{
    return &Entries[type];
}

inline DS_DOUBLEPRNENTRY *MockStatisticsStorage::GetDoublePrnEntry(WORD ofs)
{
    return &DoublePRNEntries[ofs];
}

inline DS_CELLULARENTRY *MockStatisticsStorage::GetCellularEntry(WORD type)
{
    return &CellularEntries[type];
}

inline BOOL MockStatisticsStorage::Add(Receipt &, BOOL)
{
    return TRUE;
}

inline BOOL MockStatisticsStorage::Subtract(Receipt &)
{
    return TRUE;
}

inline BOOL MockStatisticsStorage::Update()
{
    return TRUE;
}

inline void MockStatisticsStorage::SetErrors(WORD newDialErrors, WORD newCommErrors)
{
    dialErrors = newDialErrors;
    commErrors = newCommErrors;
}

inline long MockStatisticsStorage::GetTelEntries()
{
    long total = 0L;
    for (UINT i = 0; i < DS_MAXENTRIES; i++)
    {
        total += Entries[i].Tel.Nal.Receipts;
        total += Entries[i].Tel.Inter.Receipts;
    }
    return total;
}

#endif // __MOCK_STATIST_H
