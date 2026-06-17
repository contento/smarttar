#ifndef __MINIDB_CACHE_H
#define __MINIDB_CACHE_H

#if !defined(__MINIDB_PAGE_H)
#include <minidb/page.h>
#endif

// ---------------------------------------------------------------------------
// Page cache -- fixed-size LRU over page buffers.
// ---------------------------------------------------------------------------

// Number of cached pages (128 pages x 512 bytes = 64 KB)
static const UINT MINIDB_CACHE_SIZE = 128;

struct CachedPage
{
    long  PageNum;        // -1 = slot empty
    UINT  Flags;          // bit 0 = dirty, bit 1 = pinned
    BYTE *Buffer;         // heap-allocated MINIDB_PAGE_SIZE bytes
};

class MiniDBCache
{
public:
    MiniDBCache();
    ~MiniDBCache();

    // Open/create the .db file.  Returns TRUE on success.
    BOOL Open(const char *filepath);

    // Close the .db file (flush dirty pages first).
    void Close();

    // Flush all dirty pages to disk.
    BOOL Flush();

    // ---- page access ---------------------------------------------------

    // Get a page for reading.  Returns pointer to page data.
    BYTE *GetPage(long pageNum);

    // Get a page for writing (marks dirty).
    BYTE *GetPageW(long pageNum);

    // Mark page as dirty.
    void MarkDirty(long pageNum);

    // Release a page (decrement pin count).
    void Release(long pageNum);

    // Return file descriptor (for low-level I/O).
    int  GetFileDesc() const { return m_fd; }

    // Return file size in bytes.
    long GetFileSize();

    // Free-list helpers
    long GetAllocPage(UINT pageType);
    void FreeListAdd(long pageNum);
    long FreeListAlloc();
    long GetFreeHead() const { return m_freeHead; }
    void SetFreeHead(long p) { m_freeHead = p; }

private:
    // Evict the least-recently-used unpinned page.
    long Evict();

    // Find a cached slot for pageNum, or load it.
    long LoadPage(long slotIdx, long pageNum);
    BOOL WritePage(long pageNum, BYTE *data);

    CachedPage m_slots[MINIDB_CACHE_SIZE];
    int        m_fd;              // file descriptor, -1 = closed
    long       m_fileSize;        // cached file size
    long       m_freeHead;        // free-list head page number (0 = none)
    long       m_lruClock;        // clock-hand index for eviction
};

#endif // __MINIDB_CACHE_H
