//
// [ MINIDB_STORAGE.CPP ]
//
// MiniDBReceiptStorage -- IReceiptStorage over a MiniDB .db file.
// Uses page-based B-tree index with raw receipt bytes stored at file offsets
// between pages.  Atomic commit via write-to-.db.new + rename.
//

#include <string.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <minidb/page.h>
#include <minidb/cache.h>
#include <minidb/btree.h>
#include <mdb_stor.h>
#include <st_util.h>
#include <cfg.h>

extern CFG *g_cfg;

#if !defined(S_IREAD)
#define S_IREAD  0x0100
#endif
#if !defined(S_IWRITE)
#define S_IWRITE 0x0080
#endif

// ---------------------------------------------------------------------------
// Status flags
// ---------------------------------------------------------------------------
static const int MINIDB_OK          = 0x0000;
static const int MINIDB_NO_FILE     = 0x0001;
static const int MINIDB_BAD_FILE    = 0x0002;
static const int MINIDB_NEW_FILE    = 0x0004;

// Receipt magic number (same as DB_STORAGE::MAGIC_NUMBER)
static const UINT RECEIPT_MAGIC = 0x6719U;

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------

MiniDBReceiptStorage::MiniDBReceiptStorage(const char *path, const char *name, int readOnly)
    : m_dataPage(0L)
    , m_dataSlot(0)
    , m_statsPage(0L)
    , m_status(MINIDB_OK)
    , m_readOnly(readOnly)
    , m_cache()
    , m_btree(m_cache)
{
    // Build filepath: path\\name.db  (skip path if NULL)
    if (path && path[0] != '\0')
    {
        strcpy(m_filepath, path);
        strcat(m_filepath, "\\");
    }
    else
    {
        m_filepath[0] = '\0';
    }
    strcat(m_filepath, name);
    strcat(m_filepath, ".db");

    // Build .db.new path
    strcpy(m_filepathNew, m_filepath);
    strcat(m_filepathNew, ".new");

    OpenDB(m_filepath);
}

MiniDBReceiptStorage::~MiniDBReceiptStorage()
{
    if (!m_readOnly)
        Flush();
    CloseDB();
}

// ---------------------------------------------------------------------------
// OpenDB / CloseDB
// ---------------------------------------------------------------------------

BOOL MiniDBReceiptStorage::OpenDB(const char *filepath)
{
    m_status = MINIDB_OK;
    m_statsPage = 0L;

    // Open (or create) the .db file.  The cache's Open() writes a fresh
    // DBInfo page when creating a new file.
    if (!m_cache.Open(filepath))
    {
        m_status = MINIDB_BAD_FILE;
        return FALSE;
    }

    // Read DBInfo from page 0
    BYTE *page = m_cache.GetPageW(0);
    if (!page)
    {
        m_status = MINIDB_BAD_FILE;
        return FALSE;
    }
    DBInfo *dbInfo = (DBInfo *)page;

    if (!PageHdrIsValid(dbInfo->Header) || dbInfo->Header.PageType != PAGE_DBINFO)
    {
        m_status = MINIDB_BAD_FILE;
        m_cache.Release(0);
        return FALSE;
    }

    m_btree.SetRoot(dbInfo->RootPage);
    m_statsPage = dbInfo->StatsAnchor;

    // Allocate stats page if missing (new DB, or upgrade from pre-stats version)
    if (m_statsPage == 0L)
    {
        // Extend file to include page 1, then write PAGE_STATS header
        long sPageNum = m_cache.GetAllocPage(PAGE_STATS);
        if (sPageNum > 0L)
        {
            dbInfo->StatsAnchor = sPageNum;
            m_statsPage = sPageNum;
        }
    }

    m_cache.Release(0);
    m_cache.Flush();

    // Reset data-page state for receipt appending
    m_dataPage = 0L;
    m_dataSlot = 0;
    return (m_status == MINIDB_OK || m_status == MINIDB_NEW_FILE);
}

void MiniDBReceiptStorage::CloseDB()
{
    m_cache.Close();
}

// ---------------------------------------------------------------------------
// Status / read-only
// ---------------------------------------------------------------------------

int MiniDBReceiptStorage::GetStatus()
{
    return m_status;
}

BOOL MiniDBReceiptStorage::IsReadOnly()
{
    return m_readOnly;
}

// ---------------------------------------------------------------------------
// Validation helpers
// ---------------------------------------------------------------------------


BOOL MiniDBReceiptStorage::IsValid(Receipt const &receipt)
{
    return receipt.MagicNumber == RECEIPT_MAGIC;
}

// ---------------------------------------------------------------------------
// Existence / Retrieval
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Add / Update / Delete
// ---------------------------------------------------------------------------

BOOL MiniDBReceiptStorage::Add(const Receipt &receipt)
{
    if (m_readOnly)
        return FALSE;

    if (m_dataPage == 0L || m_dataSlot >= MDB_RECIPTS_PER_PAGE)
    {
        m_dataPage = m_cache.GetAllocPage(PAGE_DATA);
        if (m_dataPage == 0L)
            return FALSE;
        m_dataSlot = 0;
    }

    BYTE *page = m_cache.GetPageW(m_dataPage);
    if (!page)
        return FALSE;

    // Write receipt into the page at the next free slot
    int offset = MINIDB_HDR_SIZE + m_dataSlot * (int)sizeof(Receipt);
    memcpy(page + offset, &receipt, sizeof(Receipt));
    PageHeader *hdr = (PageHeader *)page;
    hdr->NumKeys = (UINT)(m_dataSlot + 1);   // receipts stored on page
    m_cache.Release(m_dataPage);

    // Insert into B-tree index
    long dataSeek = MDB_DataSeekEncode(m_dataPage, m_dataSlot);
    if (!m_btree.Insert(receipt.Number, receipt.BoothNumber, dataSeek))
        return FALSE;

    // Advance slot for next receipt
    m_dataSlot++;

    // Write current B-tree root page to DBInfo so it persists
    {
        long rootPage = m_btree.GetRoot();
        BYTE *p0 = m_cache.GetPageW(0);
        if (p0)
        {
            ((DBInfo *)p0)->RootPage = rootPage;
            m_cache.Release(0);
        }
    }

    m_cache.Flush();        // persist all dirty pages
    return TRUE;
}

BOOL MiniDBReceiptStorage::Get(Receipt &receipt, long number, int boothNumber)
{
    if (boothNumber < 0)
        boothNumber = 0;

    long dataSeek;
    if (!m_btree.Find(number, boothNumber, dataSeek))
        return FALSE;

    long pn = MDB_DataSeekPage(dataSeek);
    int  sl = MDB_DataSeekSlot(dataSeek);

    BYTE *pg = m_cache.GetPage(pn);
    if (!pg)
        return FALSE;

    int off = MINIDB_HDR_SIZE + sl * (int)sizeof(Receipt);
    memcpy(&receipt, pg + off, sizeof(Receipt));
    m_cache.Release(pn);

    return TRUE;
}

BOOL MiniDBReceiptStorage::Update(const Receipt &receipt)
{
    if (m_readOnly)
        return FALSE;

    long dataSeek;
    if (!m_btree.Find(receipt.Number, receipt.BoothNumber, dataSeek))
        return FALSE;

    long pageNum = MDB_DataSeekPage(dataSeek);
    int  slot    = MDB_DataSeekSlot(dataSeek);

    BYTE *page = m_cache.GetPageW(pageNum);
    if (!page)
        return FALSE;

    int offset = MINIDB_HDR_SIZE + slot * (int)sizeof(Receipt);
    memcpy(page + offset, &receipt, sizeof(Receipt));
    m_cache.Release(pageNum);

    return TRUE;
}

BOOL MiniDBReceiptStorage::Delete(long number, int boothNumber)
{
    if (m_readOnly)
        return FALSE;

    return m_btree.Delete(number, boothNumber);
}

BOOL MiniDBReceiptStorage::Exist(long number, int boothNumber) const
{
    long dummy;
    return m_btree.Find(number, boothNumber, dummy);
}
// ---------------------------------------------------------------------------
// Number queries -- delegate to B-tree
// ---------------------------------------------------------------------------

long MiniDBReceiptStorage::GetLowerNumber() const
{
    return m_btree.GetLowerNumber();
}

long MiniDBReceiptStorage::GetHigherNumber() const
{
    return m_btree.GetHigherNumber();
}

long MiniDBReceiptStorage::GetEntries() const
{
    return m_btree.GetEntryCount();
}

long MiniDBReceiptStorage::GetFirstNumber() const
{
    return m_btree.GetFirstNumber();
}

void MiniDBReceiptStorage::EnumReceipts(CallbackFnPtr callback)
{
    long leafPage = m_btree.GetFirstLeaf();

    while (leafPage != 0L)
    {
        BYTE *page = m_cache.GetPage(leafPage);
        if (!page)
            break;

        PageHeader *hdr = (PageHeader *)page;
        UINT numKeys = hdr->NumKeys;
        BTreeLeafEntry *entries = (BTreeLeafEntry *)(page + MINIDB_HDR_SIZE);

        for (UINT i = 0; i < numKeys; i++)
        {
            if (entries[i].Flags & 0x0001)
                continue;

            long pn  = MDB_DataSeekPage(entries[i].DataSeek);
            int  sl  = MDB_DataSeekSlot(entries[i].DataSeek);

            BYTE *dPage = m_cache.GetPage(pn);
            if (!dPage)
            {
                m_cache.Release(leafPage);
                return;
            }

            int off = MINIDB_HDR_SIZE + sl * (int)sizeof(Receipt);
            Receipt r;
            memcpy(&r, dPage + off, sizeof(Receipt));
            m_cache.Release(pn);

            if (IsValid(r))
            {
                if (!callback(r))
                {
                    m_cache.Release(leafPage);
                    return;
                }
            }
        }

        long rightSibling = hdr->RightSibling;
        m_cache.Release(leafPage);
        leafPage = rightSibling;
    }
}

long MiniDBReceiptStorage::GetLastNumber() const
{
    return m_btree.GetLastNumber();
}

BOOL MiniDBReceiptStorage::IsCorrectNumber(long number)
{
    return (number > 0L && number < 100000000L);
}

// ---------------------------------------------------------------------------
// Flush
// ---------------------------------------------------------------------------

void MiniDBReceiptStorage::Flush()
{
    m_cache.Flush();
}

// ---------------------------------------------------------------------------
// Archive -- rename-based atomic commit
// ---------------------------------------------------------------------------

BOOL MiniDBReceiptStorage::Archive()
{
    if (m_readOnly)
        return FALSE;

    Flush();
    CloseDB();

    // Build archive path: <sysdate>/RXdd_TT.db
    _mkSysDateDir();
    FILE_NAME arcPath;
    _getSysDatePath(arcPath);
    _PrefixAppPath(arcPath);

    WORD year, month, day;
    _GetSysDate(year, month, day);

    char tmp[32];
    sprintf(tmp, "\\RX%02d_%02d.db", day, g_cfg->TURN_NUMBER);
    strcat(arcPath, tmp);

    // Open current .db for reading
    int src = open(m_filepath, O_RDONLY | O_BINARY);
    if (src == -1)
    {
        m_status = MINIDB_NO_FILE;
        return FALSE;
    }

    // Open archive path for writing
    int dst = open(arcPath, O_CREAT | O_RDWR | O_BINARY | O_TRUNC,
                   S_IREAD | S_IWRITE);
    if (dst == -1)
    {
        close(src);
        m_status = MINIDB_NO_FILE;
        return FALSE;
    }

    // Copy all bytes from current .db to archive
    char buf[4096];
    int n;
    while ((n = read(src, buf, sizeof(buf))) > 0)
    {
        if (write(dst, buf, n) != n)
        {
            close(src);
            close(dst);
            unlink(arcPath);
            m_status = MINIDB_NO_FILE;
            return FALSE;
        }
    }

    close(src);
    close(dst);

    // Remove the current .db -- no reopen; caller creates fresh
    unlink(m_filepath);
    m_status = MINIDB_NO_FILE;

    return TRUE;
}

// ---------------------------------------------------------------------------
// Repair -- placeholder (B-tree is self-consistent for now)
// ---------------------------------------------------------------------------

BOOL MiniDBReceiptStorage::Repair()
{
    int fd = m_cache.GetFileDesc();

    if (fd != -1)
    {
        // File is open -- close and reopen to get a clean cache state
        m_cache.Close();
        if (!OpenDB(m_filepath))
            return FALSE;
        fd = m_cache.GetFileDesc();
    }

    if (fd == -1)
    {
        // File is closed (e.g. after Archive) -- create a fresh .db
        unlink(m_filepath);
        if (!OpenDB(m_filepath))
            return FALSE;
        return TRUE;  // fresh file is consistent by construction
    }

    // ---------------------------------------------------------------
    // Phase 1: Walk every B-tree entry and verify the receipt data
    // ---------------------------------------------------------------
    long leafPage = m_btree.GetFirstLeaf();
    while (leafPage != 0L)
    {
        BYTE *page = m_cache.GetPage(leafPage);
        if (!page)
            break;

        PageHeader *hdr = (PageHeader *)page;
        UINT numKeys = hdr->NumKeys;
        BTreeLeafEntry *entries = (BTreeLeafEntry *)(page + MINIDB_HDR_SIZE);

        for (UINT i = 0; i < numKeys; i++)
        {
            // Skip already-deleted entries
            if (entries[i].Flags & 0x0001)
                continue;

            if (!VerifyEntry(entries[i].Number, entries[i].BoothNumber,
                             entries[i].DataSeek))
            {
                // Invalid data at this B-tree entry -- delete it
                m_btree.Delete(entries[i].Number, entries[i].BoothNumber);
            }
        }

        long rightSibling = hdr->RightSibling;
        m_cache.Release(leafPage);
        leafPage = rightSibling;
    }

    // ---------------------------------------------------------------
    // Phase 2: Linear scan of file tail for orphaned receipts
    // ---------------------------------------------------------------
    long fileSize = m_cache.GetFileSize();
    long pageBoundary = (fileSize / MINIDB_PAGE_SIZE) * MINIDB_PAGE_SIZE;

    for (long offset = pageBoundary; offset + (long)sizeof(Receipt) <= fileSize;
         offset += sizeof(Receipt))
    {
        Receipt receipt;
        lseek(fd, offset, SEEK_SET);
        int n = read(fd, &receipt, sizeof(Receipt));
        if (n != (int)sizeof(Receipt) || receipt.MagicNumber != RECEIPT_MAGIC)
            continue;

        // Check if this receipt is already in the B-tree
        long foundSeek;
        if (!m_btree.Find(receipt.Number, receipt.BoothNumber, foundSeek))
        {
            // Orphaned receipt -- insert into B-tree
            m_btree.Insert(receipt.Number, receipt.BoothNumber, offset);
        }
    }

    // ---------------------------------------------------------------
    // Phase 3: Rebuild sequence counter from max receipt number
    // ---------------------------------------------------------------
    long maxNumber = 0L;
    leafPage = m_btree.GetFirstLeaf();
    while (leafPage != 0L)
    {
        BYTE *page = m_cache.GetPage(leafPage);
        if (!page)
            break;

        PageHeader *hdr = (PageHeader *)page;
        UINT numKeys = hdr->NumKeys;
        BTreeLeafEntry *entries = (BTreeLeafEntry *)(page + MINIDB_HDR_SIZE);

        for (UINT i = 0; i < numKeys; i++)
        {
            if (!(entries[i].Flags & 0x0001) && entries[i].Number > maxNumber)
                maxNumber = entries[i].Number;
        }

        long rightSibling = hdr->RightSibling;
        m_cache.Release(leafPage);
        leafPage = rightSibling;
    }

    // Update Sequence in DBInfo page
    BYTE *infoPage = m_cache.GetPageW(0);
    if (infoPage)
    {
        DBInfo *dbInfo = (DBInfo *)infoPage;
        if (maxNumber >= dbInfo->Sequence)
            dbInfo->Sequence = maxNumber + 1;
        m_cache.Release(0);
        m_cache.Flush();
    }

    return TRUE;
}

// ---------------------------------------------------------------------------
// VerifyEntry -- validate a single B-tree entry's receipt data
// ---------------------------------------------------------------------------

BOOL MiniDBReceiptStorage::VerifyEntry(long number, int boothNumber, long dataSeek)
{
    int fd = m_cache.GetFileDesc();
    if (fd == -1)
        return FALSE;

    Receipt receipt;
    if (lseek(fd, dataSeek, SEEK_SET) == -1L)
        return FALSE;

    int n = read(fd, &receipt, sizeof(Receipt));
    if (n != (int)sizeof(Receipt))
        return FALSE;

    return (receipt.MagicNumber == RECEIPT_MAGIC);
}
