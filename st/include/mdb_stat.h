#ifndef __MINIDB_STATISTICS_H
#define __MINIDB_STATISTICS_H

#if !defined(__ISTATIST_H)
#include <istatist.h>
#endif

#if !defined(__DS_ENTRY_H)
#include <ds_entry.h>
#endif

#if !defined(__MINIDB_CACHE_H)
#include <minidb/cache.h>
#endif

// ---------------------------------------------------------------------------
// MiniDBStatistics — IStatisticsStorage backed by a MiniDB .db file.
// Statistics are stored on a dedicated stats page within the same .db used by
// MiniDBReceiptStorage.  Both classes operate on the same MiniDBCache;
// MiniDBStatistics expects to share the cache handle with its sibling.
// ---------------------------------------------------------------------------

class MiniDBStatistics : public IStatisticsStorage
{
public:
    // Construct with a shared cache and the stats anchor page number.
    // cache is not owned — caller keeps it alive.
    MiniDBStatistics(MiniDBCache &cache, long statsPage);
    virtual ~MiniDBStatistics();

    virtual void Flush();
    virtual BOOL Archive();
    virtual BOOL Repair(IReceiptStorage *receiptStorage, BOOL all = FALSE);

    virtual DS_ENTRY          *operator[](WORD type);
    virtual DS_DOUBLEPRNENTRY *GetDoublePrnEntry(WORD ofs);
    virtual DS_CELLULARENTRY  *GetCellularEntry(WORD type);

    virtual BOOL Add(Receipt &receipt, BOOL isNew = TRUE);
    virtual BOOL Subtract(Receipt &receipt);
    virtual BOOL Update();

    virtual void SetErrors(WORD dialErrors, WORD commErrors);
    virtual long GetTelEntries();
    virtual WORD GetStatus();
    virtual BOOL IsReadOnly();

private:
    MiniDBCache &m_cache;
    long         m_statsPage;    // page number of StatsPage
    WORD         m_status;
    BOOL         m_readOnly;

    // In-memory copies of the three statistics arrays (cached from stats page)
    DS_ENTRY          m_entries[DS_MAXENTRIES];
    DS_DOUBLEPRNENTRY m_doublePrn[DS_MAXDOUBLEPRNENTRIES];
    DS_CELLULARENTRY  m_cellular[DS_MAXCELLULARENTRIES];

    // Helpers
    void InitAll();
    static BOOL EnumAddHelper(Receipt const &receipt);
};

#endif // __MINIDB_STATISTICS_H
