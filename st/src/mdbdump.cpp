//
// lsmdb — list MiniDB .db file contents.
// Usage: lsmdb [path\]RX.db
// Compiles under BCC 3.1 16-bit DOS (Phar Lap).
//

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>

#include <minidb/page.h>

// ---------------------------------------------------------------------------
// Receipt struct (mirrors the 111-byte BCC 3.1 layout from receipt.h,
// duplicated here to avoid pulling in the full app header chain.)
// ---------------------------------------------------------------------------

static const UINT RECEIPT_MAGIC = 0x6719U;

#pragma pack(1)
struct ReceiptDump
{
    UINT  MagicNumber;          // 0
    long  Number;               // 2
    UINT  Tag;                  // 6
    UINT  nStat;                // 8
    UCHAR bExtendedStat;        // 10
    int   Date;                 // 11
    int   Time;                 // 13
    int   BoothNumber;          // 15
    char  City[21];             // 17
    char  Phone[17];            // 38
    int   Amount;               // 55
    long  ElapsedTime;          // 57
    double ValuePerMin;         // 61
    double CeilMin;             // 69
    int   Percent;              // 77
    double Value;               // 79
    double Tax;                 // 87
    double Tax2;                // 95
    double DDummy;              // 103
}; // total 111 bytes
#pragma pack()

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static const char *PageTypeName(UINT pt)
{
    switch (pt)
    {
    case PAGE_FREE:    return "FREE";
    case PAGE_DBINFO:  return "DBINFO";
    case PAGE_BTREE_I: return "BTREE_I";
    case PAGE_BTREE_L: return "BTREE_L";
    case PAGE_DATA:    return "DATA";
    case PAGE_STATS:   return "STATS";
    default:           return "???";
    }
}

static const char *PeriodName(int p)
{
    static const char *names[] = { "YEAR", "MONTH", "WEEK", "DAY", "TURN" };
    if (p >= 0 && p < 5) return names[p];
    return "?";
}

static const char *SvcName(int s)
{
    static const char *names[] = { "TEL", "SPECIAL_TEL", "FAX", "TELEX", "CARD", "OTHER" };
    if (s >= 0 && s < 6) return names[s];
    return "?";
}

// ---------------------------------------------------------------------------
// Page readers
// ---------------------------------------------------------------------------

static void DumpDBInfo(const BYTE *page)
{
    const PageHeader *hdr = (const PageHeader *)page;
    const DBInfo *info = (const DBInfo *)page;

    printf("  Version=%u  RootPage=%ld  Sequence=%ld\n",
           info->Version, info->RootPage, info->Sequence);
    printf("  NumReceipts=%ld  NumArchived=%ld  StatsAnchor=%ld  FreeHead=%ld\n",
           info->NumReceipts, info->NumArchived, info->StatsAnchor, info->FreeHead);
}

static void DumpLeaf(const BYTE *page, long pageNum)
{
    const PageHeader *hdr = (const PageHeader *)page;
    UINT nk = hdr->NumKeys;
    if (nk == 0) return;

    printf("  NumKeys=%u  RightSibling=%ld\n", nk, hdr->RightSibling);
    const BTreeLeafEntry *entries = (const BTreeLeafEntry *)(page + MINIDB_HDR_SIZE);
    for (UINT i = 0; i < nk; i++)
    {
        long pn = entries[i].DataSeek >> 8;
        int  sl = (int)(entries[i].DataSeek & 0x3);
        printf("  [%u] num=%ld booth=%d page=%ld slot=%d%s\n",
               i, entries[i].Number, entries[i].BoothNumber,
               pn, sl,
               (entries[i].Flags & 0x0001) ? " (del)" : "");
    }
}

static void DumpData(const BYTE *page, long pageNum)
{
    const PageHeader *hdr = (const PageHeader *)page;
    UINT nk = hdr->NumKeys;
    if (nk == 0) return;
    printf("  NumKeys=%u\n", nk);

    for (UINT sl = 0; sl < nk; sl++)
    {
        const ReceiptDump *r = (const ReceiptDump *)(page + MINIDB_HDR_SIZE + sl * 111);
        if (r->MagicNumber != RECEIPT_MAGIC)
        {
            printf("  [slot %u] *** BAD magic=0x%04X ***\n", sl, r->MagicNumber);
            continue;
        }
        printf("  [slot %u] #%ld booth=%d date=%d time=%d\n",
               sl, r->Number, r->BoothNumber, r->Date, r->Time);
        printf("           city=%-21.21s phone=%-17.17s amount=%d elapsed=%lds\n",
               r->City, r->Phone, r->Amount, r->ElapsedTime);
        printf("           val=%.2f tax=%.2f vpm=%.4f ceil=%.1f pct=%d\n",
               r->Value, r->Tax, r->ValuePerMin, r->CeilMin, r->Percent);
    }
}

static void DumpStats(const BYTE *page, long pageNum)
{
    // Fast forward: read DS_ENTRY[5][6] as raw bytes.
    // DS_ENTRY has ITEM[6], each ITEM has receipts(int), talkMin,paidMin,value,tax (double each).
    const BYTE *pay = page + MINIDB_HDR_SIZE;

    for (int p = 0; p < 5; p++)
    {
        printf("  --- %s ---\n", PeriodName(p));
        for (int s = 0; s < 6; s++)
        {
            int off = (p * 6 + s) * (2 + 8 + 8 + 8 + 8); // = 34
            if (off + 34 > (int)(MINIDB_PAYLOAD)) break;

            int  rec   = *(const int *)(pay + off);
            double tmin = *(const double *)(pay + off + 2);
            double pmin = *(const double *)(pay + off + 10);
            double val  = *(const double *)(pay + off + 18);
            double tx   = *(const double *)(pay + off + 26);

            if (rec || val != 0.0)
                printf("    %-12s rec=%d talk=%.2f paid=%.2f val=%.2f tax=%.2f\n",
                       SvcName(s), rec, tmin, pmin, val, tx);
        }
    }
}

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const char *path = "RX.db";
    if (argc > 1) path = argv[1];

    int fd = open(path, O_RDONLY | O_BINARY);
    if (fd == -1)
    {
        printf("ERROR: cannot open '%s'\n", path);
        return 1;
    }

    long fileSize = lseek(fd, 0L, SEEK_END);
    lseek(fd, 0L, SEEK_SET);

    int nPages = (int)(fileSize / MINIDB_PAGE_SIZE);
    printf("File: %s  (%ld bytes, %d pages)\n\n", path, fileSize, nPages);

    int dataPages = 0, leafPages = 0, totalEntries = 0;

    for (int pn = 0; pn < nPages; pn++)
    {
        BYTE page[MINIDB_PAGE_SIZE];
        int n = read(fd, page, sizeof(page));
        if (n != (int)sizeof(page))
        {
            printf("ERROR: short read on page %d\n", pn);
            break;
        }

        const PageHeader *hdr = (const PageHeader *)page;
        if (hdr->Magic != MINIDB_MAGIC)
        {
            printf("Page %d [BAD]  magic=0x%04X (expected 0x%04X)\n",
                   pn, hdr->Magic, MINIDB_MAGIC);
            continue;
        }

        const char *pname = PageTypeName(hdr->PageType);
        printf("============================================================\n");
        printf("Page %d  [%s]  offset=0x%04lx\n", pn, pname, (long)pn * MINIDB_PAGE_SIZE);
        printf("============================================================\n");

        switch (hdr->PageType)
        {
        case PAGE_DBINFO:
            DumpDBInfo(page);
            break;
        case PAGE_BTREE_L:
            DumpLeaf(page, pn);
            leafPages++;
            totalEntries += (int)hdr->NumKeys;
            break;
        case PAGE_DATA:
            DumpData(page, pn);
            dataPages++;
            break;
        case PAGE_STATS:
            DumpStats(page, pn);
            break;
        case PAGE_FREE:
            printf("  (free page, rightSibling=%ld)\n", hdr->RightSibling);
            break;
        default:
            printf("  (type %u)\n", hdr->PageType);
            break;
        }
        putchar('\n');
    }

    close(fd);

    printf("============================================================\n");
    printf("Summary: %d data pages, %d leaf pages, ~%d entries in %d pages (%ld bytes)\n",
           dataPages, leafPages, totalEntries, nPages, fileSize);
    return 0;
}
