#ifndef __MINIDB_STORAGE_H
#define __MINIDB_STORAGE_H

#if !defined(__IRECEIPT_H)
#include <ireceipt.h>
#endif

#if !defined(__MINIDB_CACHE_H)
#include <minidb/cache.h>
#endif

#if !defined(__MINIDB_BTREE_H)
#include <minidb/btree.h>
#endif

// ---------------------------------------------------------------------------
// MiniDBReceiptStorage — IReceiptStorage over a MiniDB .db file.
// Each receipt is stored at a file offset within the .db.  The B-tree index
// maps (number, boothNumber) → seek position.  Atomic commit via
// write-to-.db-new + rename.
// ---------------------------------------------------------------------------

class MiniDBReceiptStorage : public IReceiptStorage
{
public:
    MiniDBReceiptStorage(const char *path, const char *name, int readOnly = TRUE);
    virtual ~MiniDBReceiptStorage();

    virtual int  GetStatus();
    virtual BOOL IsReadOnly();

    virtual BOOL IsCorrectNumber(long number);
    // Expose the shared cache so MiniDBStatistics can use the same handle
    MiniDBCache &GetCache() { return m_cache; }

    // Returns the page number of the statistics block (0 = none).
    long GetStatsPage() const { return m_statsPage; }

    virtual BOOL IsValid(Receipt const &receipt);

    virtual BOOL Exist(long number, int boothNumber) const;
    virtual BOOL Get(Receipt &receipt, long number, int boothNumber = -1);
    virtual BOOL Add(const Receipt &receipt);
    virtual BOOL Update(const Receipt &receipt);
    virtual BOOL Delete(long number, int boothNumber = -1);

    virtual long GetLowerNumber() const;
    virtual long GetHigherNumber() const;
    virtual long GetEntries() const;
    virtual long GetFirstNumber() const;
    virtual long GetLastNumber() const;

    virtual BOOL Archive();
    virtual BOOL Repair();

    virtual void EnumReceipts(CallbackFnPtr callback);
    virtual void Flush();

private:
    BOOL VerifyEntry(long number, int boothNumber, long dataSeek);
    BOOL OpenDB(const char *filepath);
    void CloseDB();

    // Current seek position for appending new receipts
    long      m_nextSeek;
    int       m_status;
    BOOL      m_readOnly;

    MiniDBCache  m_cache;
    MiniDBBTree  m_btree;
    char         m_filepath[80];
    long       m_statsPage;       // page number of stats block (0 = none)
    char         m_filepathNew[80]; // .db.new path
};

#endif // __MINIDB_STORAGE_H
