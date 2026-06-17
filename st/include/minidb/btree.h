#ifndef __MINIDB_BTREE_H
#define __MINIDB_BTREE_H

#if !defined(__MINIDB_PAGE_H)
#include <minidb/page.h>
#endif

class MiniDBCache;

// ---------------------------------------------------------------------------
// MiniDB B-tree index.
// Maps (receipt number, booth number) -> file seek position.
// Pages are 512-byte fixed-size indexed by page number (0-based).
// Page 0 is always the DBInfo page.  Root page starts at page 1.
// ---------------------------------------------------------------------------

class MiniDBBTree
{
public:
    // btree owns the cache for its lifetime
    MiniDBBTree(MiniDBCache &cache);
    ~MiniDBBTree();

    // ---- Core operations ------------------------------------------------

    // Insert a mapping.  Returns TRUE on success.
    // If an entry for (number, boothNumber) already exists, returns FALSE.
    BOOL Insert(long number, int boothNumber, long dataSeek);

    // Find the seek position for (number, boothNumber).  Returns TRUE if found.
    BOOL Find(long number, int boothNumber, long &dataSeek);

    // Mark an entry as deleted.  Returns TRUE if found and deleted.
    BOOL Delete(long number, int boothNumber);

    // Update the seek position for an existing entry.
    BOOL Update(long number, int boothNumber, long newDataSeek);

    // ---- Enumeration ----------------------------------------------------

    // Return the first receipt number in the tree (0 if empty).
    long GetFirstNumber();

    // Return the last receipt number in the tree (0 if empty).
    long GetLastNumber();

    // Return the lowest receipt number (0 if empty).
    long GetLowerNumber();

    // Return the highest receipt number (0 if empty).
    long GetHigherNumber();

    // Return total number of entries.
    long GetEntryCount();

    // ---- Page management ------------------------------------------------

    // Set the root page number.  Called during open/create.
    void SetRoot(long pageNum) { m_rootPage = pageNum; }
    long GetRoot() const       { return m_rootPage; }
    // Return the first leaf page number (leftmost leaf; 0 if empty).
    long GetFirstLeaf();
    // Return the last leaf page number (rightmost leaf; 0 if empty).
    long GetLastLeaf();


    // Allocate a new page from the free list or by appending to the file.
    long AllocPage(UINT pageType);

    // Free a page (add to free list).
    void FreePage(long pageNum);

private:
    // Search helpers
    BOOL SearchLeaf(long pageNum, long number, int boothNumber,
                    long &outDataSeek, long &outLeafPage, int &outSlot);

    BOOL SearchInternal(long pageNum, long number,
                        long &outLeafPage, int &outSlot);

    // Insert helper -- returns TRUE if page split was needed
    BOOL InsertIntoPage(long pageNum, BTreeLeafEntry const &entry,
                        BTreeLeafEntry &splitKey, long &splitChild);

    // Split a leaf page
    void SplitLeaf(long srcPage, long dstPage, BTreeLeafEntry &promotedKey,
                   long &promotedChild);

    // Mid-point for splitting
    static UINT SplitMid() { return LEAF_ENTRIES_PER_PAGE / 2; }

    MiniDBCache &m_cache;
    long         m_rootPage;   // page number of root (0 = empty)
};

#endif // __MINIDB_BTREE_H
