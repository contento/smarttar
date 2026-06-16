// ---------------------------------------------------------------------------
// MiniDB B-tree index implementation.
// B+tree: internal nodes route by separator keys (receipt numbers only),
// leaf nodes hold BTreeLeafEntry arrays sorted by (Number, BoothNumber).
// ---------------------------------------------------------------------------

#include <minidb/btree.h>
#include <minidb/cache.h>
#include <string.h>


// Forward declarations for static helpers used by MiniDBBTree methods
static void SplitLeafRaw(BYTE *srcData, BYTE *dstData, long dstPage,
                         BTreeLeafEntry &promotedKey, long &promotedChild);
// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// Compare (number, booth) against a single leaf entry.
// Returns -1 / 0 / +1.
static int CompareEntry(long number, int boothNumber, BTreeLeafEntry const &e)
{
    if (number < e.Number) return -1;
    if (number > e.Number) return 1;
    if (boothNumber < e.BoothNumber) return -1;
    if (boothNumber > e.BoothNumber) return 1;
    return 0;
}

// Binary-search a leaf page for (number, booth).
// Returns TRUE and *outSlot if found; FALSE with *outSlot = insertion point.
static BOOL SearchLeafArray(BTreeLeafEntry const *entries, UINT n,
                            long number, int boothNumber, int &outSlot)
{
    int lo = 0, hi = n;
    while (lo < hi)
    {
        int mid = (lo + hi) / 2;
        int cmp = CompareEntry(number, boothNumber, entries[mid]);
        if (cmp < 0)
            hi = mid;
        else if (cmp > 0)
            lo = mid + 1;
        else
        {
            outSlot = mid;
            return TRUE;
        }
    }
    outSlot = lo;
    return FALSE;
}

// ---------------------------------------------------------------------------
// MiniDBBTree — construction / destruction
// ---------------------------------------------------------------------------

MiniDBBTree::MiniDBBTree(MiniDBCache &cache)
    : m_cache(cache)
    , m_rootPage(0)
{
}

MiniDBBTree::~MiniDBBTree()
{
}

// ---------------------------------------------------------------------------
// AllocPage / FreePage
// ---------------------------------------------------------------------------

long MiniDBBTree::AllocPage(UINT pageType)
{
    return m_cache.GetAllocPage(pageType);
}

void MiniDBBTree::FreePage(long pageNum)
{
    m_cache.FreeListAdd(pageNum);
}

// ---------------------------------------------------------------------------
// SearchInternal — navigate through internal nodes to the correct leaf page.
// Returns the leaf page number in outLeafPage.
// ---------------------------------------------------------------------------

BOOL MiniDBBTree::SearchInternal(long pageNum, long number,
                                 long &outLeafPage, int & /*outSlot*/)
{
    BYTE *page = m_cache.GetPage(pageNum);
    if (!page) return FALSE;

    PageHeader *hdr = (PageHeader *)page;
    while (hdr->PageType != PAGE_BTREE_L)
    {
        long *ptr = (long *)(page + MINIDB_HDR_SIZE);
        UINT n = hdr->NumKeys;

        // Linear scan to find child (at most 61 keys)
        int i;
        for (i = 0; i < n; i++)
        {
            if (number < ptr[INTERNAL_KEYS_PER_PAGE + 1 + i])
                break;
        }
        long child = ptr[i];
        m_cache.Release(pageNum);
        pageNum = child;

        page = m_cache.GetPage(pageNum);
        if (!page) return FALSE;
        hdr = (PageHeader *)page;
    }

    outLeafPage = pageNum;
    m_cache.Release(pageNum);
    return TRUE;
}

// ---------------------------------------------------------------------------
// SearchLeaf — binary search a specific leaf page.
// ---------------------------------------------------------------------------

BOOL MiniDBBTree::SearchLeaf(long pageNum, long number, int boothNumber,
                             long &outDataSeek, long &outLeafPage, int &outSlot)
{
    BYTE *page = m_cache.GetPage(pageNum);
    if (!page) return FALSE;

    PageHeader *hdr = (PageHeader *)page;
    BTreeLeafEntry *entries = (BTreeLeafEntry *)(page + MINIDB_HDR_SIZE);
    UINT n = hdr->NumKeys;

    int slot;
    BOOL found = SearchLeafArray(entries, n, number, boothNumber, slot);

    if (found)
        outDataSeek = entries[slot].DataSeek;

    outLeafPage = pageNum;
    outSlot = slot;
    m_cache.Release(pageNum);
    return found;
}

// ---------------------------------------------------------------------------
// SplitLeaf — split a full leaf page.
// Takes already-pinned buffer pointers (caller handles acquire/release).
// ---------------------------------------------------------------------------

void MiniDBBTree::SplitLeaf(long srcPage, long dstPage,
                            BTreeLeafEntry &promotedKey, long &promotedChild)
{
    BYTE *src = m_cache.GetPageW(srcPage);
    BYTE *dst = m_cache.GetPageW(dstPage);
    PageHdrInit(*(PageHeader *)dst, PAGE_BTREE_L, dstPage);
    SplitLeafRaw(src, dst, dstPage, promotedKey, promotedChild);
    m_cache.MarkDirty(srcPage);
    m_cache.MarkDirty(dstPage);
    m_cache.Release(dstPage);
    m_cache.Release(srcPage);
}

static void SplitLeafRaw(BYTE *srcData, BYTE *dstData, long dstPage,
                         BTreeLeafEntry &promotedKey, long &promotedChild)
{
    PageHeader *srcHdr = (PageHeader *)srcData;
    PageHeader *dstHdr = (PageHeader *)dstData;

    BTreeLeafEntry *srcEntries = (BTreeLeafEntry *)(srcData + MINIDB_HDR_SIZE);
    BTreeLeafEntry *dstEntries = (BTreeLeafEntry *)(dstData + MINIDB_HDR_SIZE);

    UINT splitAt = LEAF_ENTRIES_PER_PAGE / 2;  // 20
    UINT srcN    = srcHdr->NumKeys;             // 42
    UINT dstN    = srcN - splitAt;              // 22

    memcpy(dstEntries, srcEntries + splitAt, dstN * sizeof(BTreeLeafEntry));

    srcHdr->NumKeys = splitAt;
    dstHdr->NumKeys = dstN;

    dstHdr->RightSibling = srcHdr->RightSibling;
    srcHdr->RightSibling = dstPage;

    promotedKey   = dstEntries[0];
    promotedChild = dstPage;
}

// ---------------------------------------------------------------------------
// InsertIntoPage — recursive insert; returns TRUE if a split occurred.
// ---------------------------------------------------------------------------

BOOL MiniDBBTree::InsertIntoPage(long pageNum, BTreeLeafEntry const &entry,
                                 BTreeLeafEntry &splitKey, long &splitChild)
{
    BYTE *page = m_cache.GetPageW(pageNum);
    if (!page) return FALSE;

    PageHeader *hdr = (PageHeader *)page;

    // ---- Leaf page --------------------------------------------------------
    if (hdr->PageType == PAGE_BTREE_L)
    {
        BTreeLeafEntry *entries = (BTreeLeafEntry *)(page + MINIDB_HDR_SIZE);
        UINT n = hdr->NumKeys;

        // Check for duplicate
        int slot;
        if (SearchLeafArray(entries, n, entry.Number, entry.BoothNumber, slot))
        {
            // Duplicate — reject
            m_cache.Release(pageNum);
            return FALSE;
        }

        // Insert at slot, shift remaining entries right
        if (slot < n)
        {
            memmove(&entries[slot + 1], &entries[slot],
                    (n - slot) * sizeof(BTreeLeafEntry));
        }
        entries[slot] = entry;
        n++;
        hdr->NumKeys = n;
        m_cache.MarkDirty(pageNum);

        // Check for overflow
        if (n > LEAF_ENTRIES_PER_PAGE)
        {
            // Split
            long newPage = AllocPage(PAGE_BTREE_L);
            if (!newPage)
            {
                m_cache.Release(pageNum);
                return FALSE;
            }

            // Acquire new page (AllocPage doesn't pin)
            BYTE *newData = m_cache.GetPageW(newPage);
            if (!newData)
            {
                m_cache.Release(pageNum);
                return FALSE;
            }
            PageHdrInit(*(PageHeader *)newData, PAGE_BTREE_L, newPage);

            // Split using raw buffers (no double-acquire of src)
            SplitLeafRaw(page, newData, newPage, splitKey, splitChild);

            m_cache.MarkDirty(newPage);
            m_cache.MarkDirty(pageNum);
            m_cache.Release(newPage);
            m_cache.Release(pageNum);
            return TRUE;
        }

        m_cache.Release(pageNum);
        return FALSE;
    }

    // ---- Internal page ----------------------------------------------------
    long *ptr    = (long *)(page + MINIDB_HDR_SIZE);
    UINT  n      = hdr->NumKeys;
    long  number = entry.Number;

    // Find child to descend into
    int pos;
    for (pos = 0; pos < n; pos++)
    {
        if (number < ptr[INTERNAL_KEYS_PER_PAGE + 1 + pos])
            break;
    }
    long childPage = ptr[pos];

    // Release this page before recursing (re-acquire if child splits)
    m_cache.Release(pageNum);

    // Recurse into child
    BTreeLeafEntry childSplitKey;
    long          childSplitChild;
    BOOL          childSplit = InsertIntoPage(childPage, entry,
                                              childSplitKey, childSplitChild);

    if (childSplit)
    {
        // Re-acquire this page
        page = m_cache.GetPageW(pageNum);
        if (!page) return FALSE;
        hdr = (PageHeader *)page;
        ptr = (long *)(page + MINIDB_HDR_SIZE);
        n   = hdr->NumKeys;

        // Insert promoted separator key at position pos,
        // and promoted child at position pos+1.
        if (pos < n)
        {
            // Shift keys[pos..n-1] right
            memmove(&ptr[INTERNAL_KEYS_PER_PAGE + 1 + pos + 1],
                    &ptr[INTERNAL_KEYS_PER_PAGE + 1 + pos],
                    (n - pos) * sizeof(long));
            // Shift children[pos+1..n] right
            memmove(&ptr[pos + 2], &ptr[pos + 1],
                    (n - pos) * sizeof(long));
        }

        ptr[INTERNAL_KEYS_PER_PAGE + 1 + pos] = childSplitKey.Number;
        ptr[pos + 1] = childSplitChild;
        n++;
        hdr->NumKeys = n;
        m_cache.MarkDirty(pageNum);

        // Check for overflow
        if (n > INTERNAL_KEYS_PER_PAGE)
        {
            // Split this internal page
            // We can't use SplitLeaf — handle inline.
            UINT mid = INTERNAL_KEYS_PER_PAGE / 2;    // 30

            long newPage = AllocPage(PAGE_BTREE_I);
            if (!newPage)
            {
                m_cache.Release(pageNum);
                return FALSE;
            }
            BYTE *newData = m_cache.GetPageW(newPage);
            if (!newData)
            {
                m_cache.Release(pageNum);
                return FALSE;
            }
            PageHeader *newHdr = (PageHeader *)newData;
            PageHdrInit(*newHdr, PAGE_BTREE_I, newPage);
            newHdr->FreeOff = 0;

            long *newPtr   = (long *)(newData + MINIDB_HDR_SIZE);
            UINT  newN     = n - mid - 1;             // keys mid+1..n-1

            // Copy keys to new page
            memcpy(&newPtr[INTERNAL_KEYS_PER_PAGE + 1],
                   &ptr[INTERNAL_KEYS_PER_PAGE + 1 + mid + 1],
                   newN * sizeof(long));
            // Copy children to new page
            memcpy(newPtr, &ptr[mid + 1],
                   (newN + 1) * sizeof(long));

            newHdr->NumKeys       = newN;
            newHdr->RightSibling  = hdr->RightSibling;

            // Fix old page
            hdr->NumKeys         = mid;
            hdr->RightSibling    = newPage;

            // Promoted key = keys[mid] (separator between left and right)
            splitKey.Number      = ptr[INTERNAL_KEYS_PER_PAGE + 1 + mid];
            splitChild           = newPage;

            m_cache.MarkDirty(newPage);
            m_cache.Release(newPage);
            m_cache.Release(pageNum);
            return TRUE;
        }

        m_cache.Release(pageNum);
        return FALSE;
    }

    // No split propagation needed
    return FALSE;
}

// ---------------------------------------------------------------------------
// Insert
// ---------------------------------------------------------------------------

BOOL MiniDBBTree::Insert(long number, int boothNumber, long dataSeek)
{
    // Build the leaf entry
    BTreeLeafEntry entry;
    entry.Number      = number;
    entry.BoothNumber = boothNumber;
    entry.DataSeek    = dataSeek;
    entry.Flags       = 0;

    // Empty tree — create root leaf page
    if (m_rootPage == 0)
    {
        m_rootPage = AllocPage(PAGE_BTREE_L);
        if (!m_rootPage) return FALSE;

        BYTE *page = m_cache.GetPageW(m_rootPage);
        if (!page) return FALSE;

        PageHeader *hdr  = (PageHeader *)page;
        PageHdrInit(*hdr, PAGE_BTREE_L, m_rootPage);

        BTreeLeafEntry *entries = (BTreeLeafEntry *)(page + MINIDB_HDR_SIZE);
        entries[0] = entry;
        hdr->NumKeys = 1;

        m_cache.Release(m_rootPage);
        return TRUE;
    }

    // Insert into existing tree
    BTreeLeafEntry splitKey;
    long          splitChild;
    BOOL          split = InsertIntoPage(m_rootPage, entry, splitKey, splitChild);

    if (split)
    {
        // Root split — create a new internal page as the new root
        long newRoot = AllocPage(PAGE_BTREE_I);
        if (!newRoot) return FALSE;

        BYTE *rootData = m_cache.GetPageW(newRoot);
        if (!rootData) return FALSE;

        PageHeader *rootHdr = (PageHeader *)rootData;
        PageHdrInit(*rootHdr, PAGE_BTREE_I, newRoot);
        rootHdr->FreeOff = 0;

        long *rootPtr = (long *)(rootData + MINIDB_HDR_SIZE);

        // children[0] = old root, children[1] = split child
        rootPtr[0] = m_rootPage;
        rootPtr[1] = splitChild;
        rootPtr[INTERNAL_KEYS_PER_PAGE + 1] = splitKey.Number;
        rootHdr->NumKeys = 1;

        m_rootPage = newRoot;
        m_cache.Release(newRoot);
    }

    return TRUE;
}

// ---------------------------------------------------------------------------
// Find
// ---------------------------------------------------------------------------

BOOL MiniDBBTree::Find(long number, int boothNumber, long &dataSeek)
{
    if (m_rootPage == 0) return FALSE;

    // Navigate to correct leaf
    long leafPage;
    int  slot;
    if (!SearchInternal(m_rootPage, number, leafPage, slot))
        return FALSE;

    // Search within the leaf
    long foundPage;
    return SearchLeaf(leafPage, number, boothNumber, dataSeek,
                      foundPage, slot);
}

// ---------------------------------------------------------------------------
// Delete — set the deleted flag (bit 0) on the matching entry.
// ---------------------------------------------------------------------------

BOOL MiniDBBTree::Delete(long number, int boothNumber)
{
    if (m_rootPage == 0) return FALSE;

    // Navigate to correct leaf
    long leafPage;
    int  slot;
    if (!SearchInternal(m_rootPage, number, leafPage, slot))
        return FALSE;

    // Search within the leaf
    BYTE *page = m_cache.GetPageW(leafPage);
    if (!page) return FALSE;

    PageHeader    *hdr     = (PageHeader *)page;
    BTreeLeafEntry *entries = (BTreeLeafEntry *)(page + MINIDB_HDR_SIZE);
    UINT n = hdr->NumKeys;

    int foundSlot;
    if (!SearchLeafArray(entries, n, number, boothNumber, foundSlot))
    {
        m_cache.Release(leafPage);
        return FALSE;
    }

    // Set deleted flag
    entries[foundSlot].Flags |= 1;
    m_cache.MarkDirty(leafPage);
    m_cache.Release(leafPage);
    return TRUE;
}

// ---------------------------------------------------------------------------
// Update — change the data seek position for an existing entry.
// ---------------------------------------------------------------------------

BOOL MiniDBBTree::Update(long number, int boothNumber, long newDataSeek)
{
    if (m_rootPage == 0) return FALSE;

    long leafPage;
    int  slot;
    if (!SearchInternal(m_rootPage, number, leafPage, slot))
        return FALSE;

    BYTE *page = m_cache.GetPageW(leafPage);
    if (!page) return FALSE;

    PageHeader    *hdr     = (PageHeader *)page;
    BTreeLeafEntry *entries = (BTreeLeafEntry *)(page + MINIDB_HDR_SIZE);

    int foundSlot;
    if (!SearchLeafArray(entries, hdr->NumKeys, number, boothNumber, foundSlot))
    {
        m_cache.Release(leafPage);
        return FALSE;
    }

    entries[foundSlot].DataSeek = newDataSeek;
    m_cache.MarkDirty(leafPage);
    m_cache.Release(leafPage);
    return TRUE;
}

// ---------------------------------------------------------------------------
// GetFirstLeaf — navigate to leftmost leaf page.
// ---------------------------------------------------------------------------

long MiniDBBTree::GetFirstLeaf()
{
    if (m_rootPage == 0) return 0;

    long pageNum = m_rootPage;

    BYTE *page = m_cache.GetPage(pageNum);
    if (!page) return 0;

    PageHeader *hdr = (PageHeader *)page;
    while (hdr->PageType != PAGE_BTREE_L)
    {
        long *ptr = (long *)(page + MINIDB_HDR_SIZE);
        long child = ptr[0];  // leftmost child
        m_cache.Release(pageNum);
        pageNum = child;

        page = m_cache.GetPage(pageNum);
        if (!page) return 0;
        hdr = (PageHeader *)page;
    }

    long result = hdr->NumKeys > 0 ? pageNum : 0;
    m_cache.Release(pageNum);
    return result;
}

// ---------------------------------------------------------------------------
// GetLastLeaf — navigate to rightmost leaf page.
// ---------------------------------------------------------------------------

long MiniDBBTree::GetLastLeaf()
{
    if (m_rootPage == 0) return 0;

    long pageNum = m_rootPage;

    BYTE *page = m_cache.GetPage(pageNum);
    if (!page) return 0;

    PageHeader *hdr = (PageHeader *)page;
    while (hdr->PageType != PAGE_BTREE_L)
    {
        long *ptr = (long *)(page + MINIDB_HDR_SIZE);
        UINT n = hdr->NumKeys;
        long child = ptr[n];  // rightmost child
        m_cache.Release(pageNum);
        pageNum = child;

        page = m_cache.GetPage(pageNum);
        if (!page) return 0;
        hdr = (PageHeader *)page;
    }

    long result = hdr->NumKeys > 0 ? pageNum : 0;
    m_cache.Release(pageNum);
    return result;
}

// ---------------------------------------------------------------------------
// GetFirstNumber — first entry in the leftmost leaf.
// ---------------------------------------------------------------------------

long MiniDBBTree::GetFirstNumber()
{
    long leaf = GetFirstLeaf();
    if (leaf == 0) return 0;

    BYTE *page = m_cache.GetPage(leaf);
    if (!page) return 0;

    BTreeLeafEntry *entries = (BTreeLeafEntry *)(page + MINIDB_HDR_SIZE);
    long result = entries[0].Number;
    m_cache.Release(leaf);
    return result;
}

// ---------------------------------------------------------------------------
// GetLastNumber — last entry in the rightmost leaf.
// ---------------------------------------------------------------------------

long MiniDBBTree::GetLastNumber()
{
    long leaf = GetLastLeaf();
    if (leaf == 0) return 0;

    BYTE *page = m_cache.GetPage(leaf);
    if (!page) return 0;

    PageHeader    *hdr     = (PageHeader *)page;
    BTreeLeafEntry *entries = (BTreeLeafEntry *)(page + MINIDB_HDR_SIZE);
    long result = entries[hdr->NumKeys - 1].Number;
    m_cache.Release(leaf);
    return result;
}

// ---------------------------------------------------------------------------
// GetLowerNumber / GetHigherNumber — aliases for first/last.
// ---------------------------------------------------------------------------

long MiniDBBTree::GetLowerNumber()
{
    return GetFirstNumber();
}

long MiniDBBTree::GetHigherNumber()
{
    return GetLastNumber();
}

// ---------------------------------------------------------------------------
// GetEntryCount — read from DBInfo page 0.
// ---------------------------------------------------------------------------

long MiniDBBTree::GetEntryCount()
{
    BYTE *page = m_cache.GetPage(0);
    if (!page) return 0;

    DBInfo *info = (DBInfo *)page;
    long result = info->NumReceipts;
    m_cache.Release(0);
    return result;
}
