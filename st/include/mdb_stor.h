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
// MiniDBReceiptStorage -- IReceiptStorage over a MiniDB .db file.
// Receipts are stored in PAGE_DATA pages (4 per page) within the .db.
// The B-tree index maps (number, boothNumber) -> (pageNum << 8) | slot.
// ---------------------------------------------------------------------------

// Receipts per data page
static const int MDB_RECIPTS_PER_PAGE = 4;

// Helpers to encode/decode the DataSeek field
inline long MDB_DataSeekEncode(long pageNum, int slot)
    { return (pageNum << 8) | (slot & 0x3); }
inline long MDB_DataSeekPage(long ds)
    { return ds >> 8; }
inline int  MDB_DataSeekSlot(long ds)
    { return (int)(ds & 0x3); }

class MiniDBReceiptStorage : public IReceiptStorage
{
public:
    MiniDBReceiptStorage(const char *path, const char *name, int readOnly = TRUE);
    virtual ~MiniDBReceiptStorage();

    virtual int  GetStatus();
    virtual BOOL IsReadOnly();

    virtual BOOL IsCorrectNumber(long number);
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

    // Expose the shared cache so MiniDBStatistics can use the same handle
    MiniDBCache &GetCache() { return m_cache; }
    long GetStatsAnchor() const { return m_statsAnchor; }

private:
    BOOL VerifyEntry(long number, int boothNumber, long dataSeek);
    BOOL OpenDB(const char *filepath);
    void CloseDB();

    MiniDBCache  m_cache;
    MiniDBBTree  m_btree;
    char         m_filepath[80];
    char         m_filepathNew[80];  // .db.new path

    long      m_dataPage;       // current open data page for appends
    int       m_dataSlot;       // next free slot on m_dataPage
    long      m_statsAnchor;    // page number of stats block anchor (start of DS_ENTRY[0])
    int       m_status;
    BOOL      m_readOnly;
};

#endif // __MINIDB_STORAGE_H
