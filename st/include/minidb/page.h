#ifndef __MINIDB_PAGE_H
#define __MINIDB_PAGE_H

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif


#if !defined(__STRING_H)
#include <string.h>
#endif
// ---------------------------------------------------------------------------
// MiniDB page-level constants and structures.
// All multi-byte values are little-endian (native x86/DOS).
// ---------------------------------------------------------------------------

// .db file magic (first 2 bytes of every .db file)
static const UINT MINIDB_MAGIC     = 0xDBDBU;

// Current format version
static const UINT MINIDB_VERSION   = 0x0001U;

// Fixed page size (1 DOS sector)
static const UINT MINIDB_PAGE_SIZE = 512U;

// Page header size
static const UINT MINIDB_HDR_SIZE  = 16U;

// Maximum payload per page
static const UINT MINIDB_PAYLOAD   = MINIDB_PAGE_SIZE - MINIDB_HDR_SIZE; // 496

// ---------------------------------------------------------------------------
// Page types
// ---------------------------------------------------------------------------
enum MINIDB_PAGE_TYPE
{
    PAGE_FREE    = 0,    // unallocated / free list
    PAGE_DBINFO  = 1,    // database info (page 0 only)
    PAGE_BTREE_I = 2,    // B-tree internal node
    PAGE_BTREE_L = 3,    // B-tree leaf node
    PAGE_DATA    = 4,    // receipt data block
    PAGE_STATS   = 5,    // statistics block
};

// ---------------------------------------------------------------------------
// Page header (present on every page)
// ---------------------------------------------------------------------------
struct PageHeader
{
    UINT  Magic;          // MINIDB_MAGIC
    UINT  PageType;       // MINIDB_PAGE_TYPE
    UINT  NumKeys;        // number of entries on this page
    UINT  FreeOff;        // offset of free space start within page
    long  RightSibling;   // page number of right sibling (0 = none)
    BYTE  Reserved[4];    // future use
};

// ---------------------------------------------------------------------------
// DBInfo page (page 0)
// ---------------------------------------------------------------------------
struct DBInfo
{
    PageHeader Header;    // Magic=MINIDB_MAGIC, PageType=PAGE_DBINFO
    UINT  Version;        // MINIDB_VERSION
    long  RootPage;       // page number of B-tree root (0 = empty)
    long  Sequence;       // incrementing counter for receipt numbers
    long  NumReceipts;    // total receipt count
    long  NumArchived;    // total archived receipt count
    long  StatsAnchor;    // page number of statistics block
    long  FreeHead;       // free-list head page (0 = none)
    BYTE  Reserved2[MINIDB_PAYLOAD - 6*sizeof(long) - sizeof(UINT)];
};

// ---------------------------------------------------------------------------
// B-tree leaf entry: maps receipt number + booth to data seek position
// ---------------------------------------------------------------------------
struct BTreeLeafEntry
{
    long  Number;         // receipt number
    int   BoothNumber;    // booth number
    long  DataSeek;       // file offset of the Receipt struct in .db
    UINT  Flags;          // bit 0 = deleted
};

// Number of leaf entries per page
static const UINT LEAF_ENTRIES_PER_PAGE =
    MINIDB_PAYLOAD / sizeof(BTreeLeafEntry); // 496/12 = 41

// ---------------------------------------------------------------------------
// B-tree internal entry: separator key + child page number
// ---------------------------------------------------------------------------
struct BTreeInternalEntry
{
    long  Key;            // separator key (max key in left subtree)
    long  ChildPage;      // page number of child
};

// Maximum internal entries per page (each has an extra implicit child)
static const UINT INTERNAL_ENTRIES_PER_PAGE =
    (MINIDB_PAYLOAD - sizeof(long)) / (sizeof(BTreeInternalEntry) + sizeof(long));
// With N entries, there are N+1 children.  First child stored at offset 0,
// then (key, child) pairs.  (496 - 4) / (8 + 4) = 41 entries max.
// But the practical limit is lower to keep the last child slot.
// We use a simpler layout: entries[0..N-1] each have Key+ChildPage,
// and ExtraChild is stored last.  496 / (4+4) = 62 -> 61 entries + 62 children.
// Simpler: store keys[0..N-1] and children[0..N] as arrays.
// 496 / 4 = 124 max values.  N keys + N+1 children = 2N+1 <= 124 -> N=61.

static const UINT INTERNAL_KEYS_PER_PAGE = 61;

// ---------------------------------------------------------------------------
// Receipt data block (holds multiple fixed-size Receipt structs)
// ---------------------------------------------------------------------------
static const UINT DATA_RECEIPTS_PER_PAGE =
    MINIDB_PAYLOAD / 111U;   // 496 / 111 = 4

// ---------------------------------------------------------------------------
// Statistics block (fixed-location, holds DS_ENTRY arrays)
// ---------------------------------------------------------------------------
// The stats page stores DS_ENTRY[5], DS_DOUBLEPRNENTRY[2], DS_CELLULARENTRY[5]
// embedded as raw bytes.  The exact layout mirrors DB_STATISTICS' in-memory
// representation so Flush() is a simple memcpy.

struct StatsPage
{
    PageHeader Header;    // PageType=PAGE_STATS
    BYTE  DSData[MINIDB_PAYLOAD];
    // Cast DSData to DS_ENTRY/DS_DOUBLEPRNENTRY/DS_CELLULARENTRY as needed.
};

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

inline void PageHdrInit(PageHeader &hdr, UINT pageType, UINT pageNum)
{
    memset(&hdr, 0, sizeof(hdr));
    hdr.Magic    = MINIDB_MAGIC;
    hdr.PageType = pageType;
    hdr.NumKeys  = 0;
    hdr.FreeOff  = MINIDB_HDR_SIZE;
}

inline BOOL PageHdrIsValid(PageHeader const &hdr)
{
    return hdr.Magic == MINIDB_MAGIC;
}

#endif // __MINIDB_PAGE_H
