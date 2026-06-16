//
// [ CACHE.CPP ]
//
// MiniDB page cache — fixed-size LRU with clock-hand eviction.
// All .db page I/O goes through this cache.
//

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <minidb/page.h>
#include <minidb/cache.h>

// ============================================================================
// Construction / destruction
// ============================================================================

MiniDBCache::MiniDBCache()
    : m_fd(-1)
    , m_fileSize(0)
    , m_freeHead(0)
    , m_lruClock(0)
{
    for (UINT i = 0; i < MINIDB_CACHE_SIZE; i++)
    {
        m_slots[i].PageNum = -1;
        m_slots[i].Flags   = 0;
        m_slots[i].Buffer  = (BYTE *)malloc(MINIDB_PAGE_SIZE);
    }
}

MiniDBCache::~MiniDBCache()
{
    if (m_fd >= 0)
    {
        Close();
    }
    else
    {
        // if never opened (or already closed), free remaining buffers
        for (UINT i = 0; i < MINIDB_CACHE_SIZE; i++)
        {
            if (m_slots[i].Buffer)
                free(m_slots[i].Buffer);
            m_slots[i].Buffer = NULL;
        }
    }
}

// ============================================================================
// Open / Close / Flush
// ============================================================================

BOOL MiniDBCache::Open(const char *filepath)
{
    // Try opening existing file
    m_fd = ::open(filepath, O_RDWR | O_BINARY);
    if (m_fd < 0)
    {
        // File doesn't exist — create it
        m_fd = ::open(filepath, O_CREAT | O_TRUNC | O_RDWR | O_BINARY,
                      S_IWRITE | S_IREAD);
        if (m_fd < 0)
            return FALSE;

        // Write a fresh DBInfo page (page 0)
        BYTE *buf = (BYTE *)malloc(MINIDB_PAGE_SIZE);
        if (!buf)
        {
            ::close(m_fd);
            m_fd = -1;
            return FALSE;
        }
        memset(buf, 0, MINIDB_PAGE_SIZE);

        PageHeader *hdr  = (PageHeader *)buf;
        DBInfo     *info = (DBInfo *)buf;

        PageHdrInit(*hdr, PAGE_DBINFO, 0);
        info->Version     = MINIDB_VERSION;
        info->RootPage    = 0;
        info->Sequence    = 0;
        info->NumReceipts = 0;
        info->NumArchived = 0;
        info->StatsAnchor = 0;
        info->FreeHead    = 0;

        if (::lseek(m_fd, 0, SEEK_SET) < 0 ||
            ::write(m_fd, buf, MINIDB_PAGE_SIZE) < (int)MINIDB_PAGE_SIZE)
        {
            free(buf);
            ::close(m_fd);
            m_fd = -1;
            return FALSE;
        }
        free(buf);
    }

    // Read the DBInfo page to verify and extract free-head
    BYTE pageBuf[MINIDB_PAGE_SIZE];
    if (::lseek(m_fd, 0, SEEK_SET) < 0 ||
        ::read(m_fd, pageBuf, MINIDB_PAGE_SIZE) < (int)MINIDB_PAGE_SIZE)
    {
        ::close(m_fd);
        m_fd = -1;
        return FALSE;
    }

    PageHeader *hdr = (PageHeader *)pageBuf;
    if (hdr->Magic != MINIDB_MAGIC || hdr->PageType != PAGE_DBINFO)
    {
        ::close(m_fd);
        m_fd = -1;
        return FALSE;
    }

    DBInfo *info = (DBInfo *)pageBuf;
    m_freeHead = info->FreeHead;

    // Cache file size
    m_fileSize = ::lseek(m_fd, 0, SEEK_END);
    if (m_fileSize < 0)
    {
        ::close(m_fd);
        m_fd = -1;
        return FALSE;
    }

    return TRUE;
}

void MiniDBCache::Close()
{
    if (m_fd < 0)
        return;

    Flush();

    ::close(m_fd);
    m_fd = -1;

    for (UINT i = 0; i < MINIDB_CACHE_SIZE; i++)
    {
        if (m_slots[i].Buffer)
            free(m_slots[i].Buffer);
        m_slots[i].Buffer  = NULL;
        m_slots[i].PageNum = -1;
        m_slots[i].Flags   = 0;
    }
}

BOOL MiniDBCache::Flush()
{
    if (m_fd < 0)
        return FALSE;

    BOOL ok = TRUE;
    for (UINT i = 0; i < MINIDB_CACHE_SIZE; i++)
    {
        if (m_slots[i].PageNum >= 0 && (m_slots[i].Flags & 1))
        {
            if (!WritePage(m_slots[i].PageNum, m_slots[i].Buffer))
                ok = FALSE;
            m_slots[i].Flags &= ~1;   // clear dirty
        }
    }
    return ok;
}

// ============================================================================
// Page access
// ============================================================================

BYTE *MiniDBCache::GetPage(long pageNum)
{
    if (m_fd < 0 || pageNum < 0)
        return NULL;

    // Scan for hit
    for (UINT i = 0; i < MINIDB_CACHE_SIZE; i++)
    {
        if (m_slots[i].PageNum == pageNum)
        {
            m_slots[i].Flags |= 2;           // pin
            m_lruClock = (i + 1) % MINIDB_CACHE_SIZE;
            return m_slots[i].Buffer;
        }
    }

    // Miss — evict, then load
    long slotIdx = Evict();
    if (slotIdx < 0)
        return NULL;
    LoadPage(slotIdx, pageNum);
    m_slots[slotIdx].Flags |= 2;             // pin
    m_lruClock = (slotIdx + 1) % MINIDB_CACHE_SIZE;
    return m_slots[slotIdx].Buffer;
}

BYTE *MiniDBCache::GetPageW(long pageNum)
{
    BYTE *buf = GetPage(pageNum);
    if (buf)
        MarkDirty(pageNum);
    return buf;
}

void MiniDBCache::MarkDirty(long pageNum)
{
    for (UINT i = 0; i < MINIDB_CACHE_SIZE; i++)
    {
        if (m_slots[i].PageNum == pageNum)
        {
            m_slots[i].Flags |= 1;           // dirty
            return;
        }
    }
}

void MiniDBCache::Release(long pageNum)
{
    for (UINT i = 0; i < MINIDB_CACHE_SIZE; i++)
    {
        if (m_slots[i].PageNum == pageNum)
        {
            m_slots[i].Flags &= ~2;          // clear pin
            return;
        }
    }
}

// ============================================================================
// File size
// ============================================================================

long MiniDBCache::GetFileSize()
{
    if (m_fd >= 0)
    {
        long sz = ::lseek(m_fd, 0, SEEK_END);
        if (sz >= 0)
            m_fileSize = sz;
    }
    return m_fileSize;
}

// ============================================================================
// Free-list management
// ============================================================================

long MiniDBCache::GetAllocPage(UINT pageType)
{
    // Try free list first
    long newPageNum = FreeListAlloc();
    if (newPageNum != 0)
    {
        // Page already zeroed by FreeListAlloc — just set the header
        BYTE *buf = GetPageW(newPageNum);
        if (!buf)
            return 0;
        memset(buf, 0, MINIDB_PAGE_SIZE);
        PageHdrInit(*(PageHeader *)buf, pageType, (UINT)newPageNum);
        Release(newPageNum);
        MarkDirty(newPageNum);
        return newPageNum;
    }

    // Append a new page past the current end
    newPageNum = m_fileSize / MINIDB_PAGE_SIZE;

    BYTE *buf = (BYTE *)malloc(MINIDB_PAGE_SIZE);
    if (!buf)
        return 0;
    memset(buf, 0, MINIDB_PAGE_SIZE);
    PageHdrInit(*(PageHeader *)buf, pageType, (UINT)newPageNum);

    if (::lseek(m_fd, newPageNum * MINIDB_PAGE_SIZE, SEEK_SET) < 0 ||
        ::write(m_fd, buf, MINIDB_PAGE_SIZE) < (int)MINIDB_PAGE_SIZE)
    {
        free(buf);
        m_fd = -1;
        return 0;
    }
    free(buf);
    m_fileSize += MINIDB_PAGE_SIZE;
    return newPageNum;
}

void MiniDBCache::FreeListAdd(long pageNum)
{
    BYTE *buf = GetPageW(pageNum);
    if (!buf)
        return;

    memset(buf, 0, MINIDB_PAGE_SIZE);
    PageHeader *hdr = (PageHeader *)buf;
    PageHdrInit(*hdr, PAGE_FREE, (UINT)pageNum);
    hdr->RightSibling = m_freeHead;

    MarkDirty(pageNum);
    Release(pageNum);

    m_freeHead = pageNum;
}

long MiniDBCache::FreeListAlloc()
{
    if (m_freeHead == 0)
        return 0;

    long head = m_freeHead;

    BYTE *buf = GetPage(head);
    if (!buf)
    {
        // page unreachable — empty the free list
        m_freeHead = 0;
        return 0;
    }

    PageHeader *hdr = (PageHeader *)buf;
    m_freeHead = hdr->RightSibling;

    Release(head);

    // Clear the page (caller will set the type header)
    buf = GetPageW(head);
    if (buf)
    {
        memset(buf, 0, MINIDB_PAGE_SIZE);
        MarkDirty(head);
        Release(head);
    }

    return head;
}

// ============================================================================
// Internal helpers
// ============================================================================

long MiniDBCache::Evict()
{
    for (UINT i = 0; i < MINIDB_CACHE_SIZE * 2; i++)
    {
        CachedPage *slot = &m_slots[m_lruClock];

        if (slot->PageNum < 0)
        {
            // Already empty
            long idx = m_lruClock;
            m_lruClock = (m_lruClock + 1) % MINIDB_CACHE_SIZE;
            return idx;
        }

        if (!(slot->Flags & 2))          // not pinned
        {
            long idx = m_lruClock;
            if (slot->Flags & 1)         // dirty
            {
                if (!WritePage(slot->PageNum, slot->Buffer))
                    return -1;
            }
            slot->PageNum = -1;
            slot->Flags   = 0;
            m_lruClock = (m_lruClock + 1) % MINIDB_CACHE_SIZE;
            return idx;
        }

        m_lruClock = (m_lruClock + 1) % MINIDB_CACHE_SIZE;
    }
    return -1;   // all pages pinned
}

long MiniDBCache::LoadPage(long slotIdx, long pageNum)
{
    if (m_fd < 0)
        return -1;

    CachedPage *slot = &m_slots[slotIdx];

    if (::lseek(m_fd, pageNum * MINIDB_PAGE_SIZE, SEEK_SET) < 0)
        return -1;

    int n = ::read(m_fd, slot->Buffer, MINIDB_PAGE_SIZE);
    if (n <= 0)
    {
        // Beyond EOF or empty file — zero-fill and treat as fresh page.
        // The caller will write valid data into it via GetPageW().
        memset(slot->Buffer, 0, MINIDB_PAGE_SIZE);
    }
    else if (n < (int)MINIDB_PAGE_SIZE)
    {
        // Partial read at end of file — zero the remainder.
        memset(slot->Buffer + n, 0, MINIDB_PAGE_SIZE - n);
    }

    slot->PageNum = pageNum;
    slot->Flags   = 0;
    return slotIdx;
}

BOOL MiniDBCache::WritePage(long pageNum, BYTE *data)
{
    if (m_fd < 0)
        return FALSE;

    if (::lseek(m_fd, pageNum * MINIDB_PAGE_SIZE, SEEK_SET) < 0)
    {
        m_fd = -1;
        return FALSE;
    }

    if (::write(m_fd, data, MINIDB_PAGE_SIZE) < (int)MINIDB_PAGE_SIZE)
    {
        m_fd = -1;
        return FALSE;
    }

    return TRUE;
}
