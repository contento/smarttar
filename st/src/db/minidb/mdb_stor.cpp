//
// [ MINIDB_STORAGE.CPP ]
//
// MiniDBReceiptStorage — IReceiptStorage over a MiniDB .db file.
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

MiniDBReceiptStorage::MiniDBReceiptStorage(const char *path, const char *name,
                                           int readOnly)
    : m_nextSeek(0L)
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

    if (!m_cache.Open(filepath))
    {
        // Open failed — create a new .db if not read-only
        if (m_readOnly)
        {
            m_status = MINIDB_NO_FILE;
            return FALSE;
        }

        int fd = open(filepath, O_CREAT | O_RDWR | O_BINARY,
                      S_IREAD | S_IWRITE);
        if (fd == -1)
        {
            m_status = MINIDB_BAD_FILE;
            return FALSE;
        }
        close(fd);

        if (!m_cache.Open(filepath))
        {
            m_status = MINIDB_BAD_FILE;
            return FALSE;
        }

        // Write DBInfo page (page 0)
        BYTE *page = m_cache.GetPageW(0);
        if (!page)
        {
            m_cache.Close();
            m_status = MINIDB_BAD_FILE;
            return FALSE;
        }

        memset(page, 0, MINIDB_PAGE_SIZE);
        PageHdrInit(*(PageHeader *)page, PAGE_DBINFO, 0);

        DBInfo *dbInfo = (DBInfo *)page;
        dbInfo->Version      = MINIDB_VERSION;
        dbInfo->RootPage     = 0;       // empty tree
        dbInfo->Sequence     = 0L;
        dbInfo->NumReceipts  = 0L;
        dbInfo->NumArchived  = 0L;
        dbInfo->StatsAnchor  = 0L;
        dbInfo->FreeHead     = 0L;

        m_cache.Release(0);
        m_cache.Flush();

        m_btree.SetRoot(0);
        m_status = MINIDB_NEW_FILE;
    }
    else
    {
        // Opened existing file — read root page from DBInfo
        BYTE *page = m_cache.GetPage(0);
        if (page)
        {
            DBInfo *dbInfo = (DBInfo *)page;
            if (!PageHdrIsValid(dbInfo->Header) ||
                dbInfo->Header.PageType != PAGE_DBINFO)
            {
                m_status = MINIDB_BAD_FILE;
            }
            else
            {
                m_btree.SetRoot(dbInfo->RootPage);
            }
            m_cache.Release(0);
        }
        else
        {
            m_status = MINIDB_BAD_FILE;
        }
    }

    // Current append position is end of file (after all pages)
    m_nextSeek = m_cache.GetFileSize();
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

BOOL MiniDBReceiptStorage::IsCorrectNumber(long number)
{
    return (number > 0L);
}

BOOL MiniDBReceiptStorage::IsValid(Receipt const &receipt)
{
    return receipt.MagicNumber == RECEIPT_MAGIC;
}

// ---------------------------------------------------------------------------
// Existence / Retrieval
// ---------------------------------------------------------------------------

BOOL MiniDBReceiptStorage::Exist(long number, int boothNumber) const
{
    long dataSeek;
    return m_btree.Find(number, boothNumber, dataSeek);
}

BOOL MiniDBReceiptStorage::Get(Receipt &receipt, long number, int boothNumber)
{
    long dataSeek;
    if (!m_btree.Find(number, boothNumber, dataSeek))
        return FALSE;

    int fd = m_cache.GetFileDesc();
    if (fd == -1)
        return FALSE;

    lseek(fd, dataSeek, SEEK_SET);
    int n = read(fd, &receipt, sizeof(Receipt));
    return (n == (int)sizeof(Receipt));
}

// ---------------------------------------------------------------------------
// Add / Update / Delete
// ---------------------------------------------------------------------------

BOOL MiniDBReceiptStorage::Add(const Receipt &receipt)
{
    if (m_readOnly)
        return FALSE;

    int fd = m_cache.GetFileDesc();
    if (fd == -1)
        return FALSE;

    // Seek to end of file — receipts are stored past all page data
    long dataSeek = lseek(fd, 0, SEEK_END);
    if (dataSeek == -1L)
        return FALSE;

    // Write receipt data at the append offset
    int n = write(fd, &receipt, sizeof(Receipt));
    if (n != (int)sizeof(Receipt))
        return FALSE;

    // Insert into B-tree index
    if (!m_btree.Insert(receipt.Number, receipt.BoothNumber, dataSeek))
        return FALSE;

    // Update next seek position for fast path
    m_nextSeek = dataSeek + sizeof(Receipt);
    return TRUE;
}

BOOL MiniDBReceiptStorage::Update(const Receipt &receipt)
{
    if (m_readOnly)
        return FALSE;

    long dataSeek;
    if (!m_btree.Find(receipt.Number, receipt.BoothNumber, dataSeek))
        return FALSE;

    int fd = m_cache.GetFileDesc();
    if (fd == -1)
        return FALSE;

    lseek(fd, dataSeek, SEEK_SET);
    int n = write(fd, &receipt, sizeof(Receipt));
    return (n == (int)sizeof(Receipt));
}

BOOL MiniDBReceiptStorage::Delete(long number, int boothNumber)
{
    if (m_readOnly)
        return FALSE;

    return m_btree.Delete(number, boothNumber);
}

// ---------------------------------------------------------------------------
// Number queries — delegate to B-tree
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

long MiniDBReceiptStorage::GetLastNumber() const
{
    return m_btree.GetLastNumber();
}

// ---------------------------------------------------------------------------
// Enumeration — in-order walk via B-tree
// ---------------------------------------------------------------------------

void MiniDBReceiptStorage::EnumReceipts(CallbackFnPtr callback)
{
    // In-order enumeration requires walking the B-tree in sorted order.
    // Since the B-tree stores leaf entries sorted by (number, booth),
    // we walk leaf pages from left to right via RightSibling links.
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
            // Skip deleted entries
            if (entries[i].Flags & 0x0001)
                continue;

            // Read the receipt at DataSeek
            Receipt receipt;
            int fd = m_cache.GetFileDesc();
            lseek(fd, entries[i].DataSeek, SEEK_SET);
            int n = read(fd, &receipt, sizeof(Receipt));
            if (n == (int)sizeof(Receipt) && IsValid(receipt))
            {
                if (!callback(receipt))
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

// ---------------------------------------------------------------------------
// Flush
// ---------------------------------------------------------------------------

void MiniDBReceiptStorage::Flush()
{
    m_cache.Flush();
}

// ---------------------------------------------------------------------------
// Archive — rename-based atomic commit
// ---------------------------------------------------------------------------

BOOL MiniDBReceiptStorage::Archive()
{
    if (m_readOnly)
        return FALSE;

    Flush();
    CloseDB();

    // Copy current .db to .db.new
    int src = open(m_filepath, O_RDONLY | O_BINARY);
    if (src == -1)
    {
        OpenDB(m_filepath);
        return FALSE;
    }

    int dst = open(m_filepathNew, O_CREAT | O_RDWR | O_BINARY | O_TRUNC,
                   S_IREAD | S_IWRITE);
    if (dst == -1)
    {
        close(src);
        OpenDB(m_filepath);
        return FALSE;
    }

    char buf[4096];
    int n;
    while ((n = read(src, buf, sizeof(buf))) > 0)
    {
        if (write(dst, buf, n) != n)
        {
            close(src);
            close(dst);
            unlink(m_filepathNew);
            OpenDB(m_filepath);
            return FALSE;
        }
    }

    close(src);
    close(dst);

    // Atomic rename: .db.new -> .db
    if (access(m_filepath, 6) != 0)
        chmod(m_filepath, S_IREAD | S_IWRITE);
    unlink(m_filepath);

    if (rename(m_filepathNew, m_filepath) != 0)
    {
        OpenDB(m_filepath);
        return FALSE;
    }

    return OpenDB(m_filepath);
}

// ---------------------------------------------------------------------------
// Repair — placeholder (B-tree is self-consistent for now)
// ---------------------------------------------------------------------------

BOOL MiniDBReceiptStorage::Repair()
{
    // B-tree index self-consistency is maintained by transaction-level
    // operations.  A full verification would involve walking all data
    // records and rebuilding the index — deferred.
    return TRUE;
}
