//
// mdbdump — list MiniDB .db file contents.
// Usage: mdbdump [options] [path]
// Compiles under BCC 3.1 16-bit DOS (Phar Lap).
//

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>

#include <minidb/page.h>

static const UINT RECEIPT_MAGIC = 0x6719U;

#pragma pack(1)
struct ReceiptDump
{
    UINT  MagicNumber;
    long  Number;
    UINT  Tag;
    UINT  nStat;
    UCHAR bExtendedStat;
    int   Date;
    int   Time;
    int   BoothNumber;
    char  City[21];
    char  Phone[17];
    int   Amount;
    long  ElapsedTime;
    double ValuePerMin;
    double CeilMin;
    int   Percent;
    double Value;
    double Tax;
    double Tax2;
    double DDummy;
};
#pragma pack()

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
    return (p >= 0 && p < 5) ? names[p] : "?";
}

static const char *SvcName(int s)
{
    static const char *names[] = { "TEL", "SPECIAL_TEL", "FAX", "TELEX", "CARD", "OTHER" };
    return (s >= 0 && s < 6) ? names[s] : "?";
}

// ---------------------------------------------------------------------------
// Page readers
// ---------------------------------------------------------------------------

static void DumpDBInfo(const BYTE *page, FILE *out)
{
    const DBInfo *info = (const DBInfo *)page;
    fprintf(out, "  Version=%u  RootPage=%ld  Sequence=%ld\n",
           info->Version, info->RootPage, info->Sequence);
    fprintf(out, "  NumReceipts=%ld  NumArchived=%ld  StatsAnchor=%ld  FreeHead=%ld\n",
           info->NumReceipts, info->NumArchived, info->StatsAnchor, info->FreeHead);
}

static void DumpLeaf(const BYTE *page, FILE *out)
{
    const PageHeader *hdr = (const PageHeader *)page;
    UINT nk = hdr->NumKeys;
    if (nk == 0) return;
    fprintf(out, "  NumKeys=%u  RightSibling=%ld\n", nk, hdr->RightSibling);
    const BTreeLeafEntry *e = (const BTreeLeafEntry *)(page + MINIDB_HDR_SIZE);
    for (UINT i = 0; i < nk; i++)
        fprintf(out, "  [%u] num=%ld booth=%d page=%ld slot=%d%s\n",
               i, e[i].Number, e[i].BoothNumber,
               e[i].DataSeek >> 8, (int)(e[i].DataSeek & 3),
               (e[i].Flags & 0x0001) ? " (del)" : "");
}

static void DumpData(const BYTE *page, FILE *out)
{
    const PageHeader *hdr = (const PageHeader *)page;
    UINT nk = hdr->NumKeys;
    if (nk == 0) return;
    fprintf(out, "  NumKeys=%u\n", nk);
    for (UINT sl = 0; sl < nk; sl++)
    {
        const ReceiptDump *r = (const ReceiptDump *)(page + MINIDB_HDR_SIZE + sl * 111);
        if (r->MagicNumber != RECEIPT_MAGIC) { fprintf(out, "  [slot %u] BAD\n", sl); continue; }
        fprintf(out, "  [slot %u] #%ld booth=%d date=%d time=%d\n",
               sl, r->Number, r->BoothNumber, r->Date, r->Time);
        fprintf(out, "           city=%-21.21s phone=%-17.17s amount=%d elapsed=%lds\n",
               r->City, r->Phone, r->Amount, r->ElapsedTime);
        fprintf(out, "           val=%.2f tax=%.2f vpm=%.4f ceil=%.1f pct=%d\n",
               r->Value, r->Tax, r->ValuePerMin, r->CeilMin, r->Percent);
    }
}

static void DumpStats(const BYTE *page, FILE *out)
{
    const BYTE *pay = page + MINIDB_HDR_SIZE;
    for (int p = 0; p < 5; p++)
    {
        fprintf(out, "  --- %s ---\n", PeriodName(p));
        for (int s = 0; s < 6; s++)
        {
            int off = (p * 6 + s) * 34;
            if (off + 34 > (int)MINIDB_PAYLOAD) break;
            int  rec   = *(const int *)(pay + off);
            double tmin = *(const double *)(pay + off + 2);
            double pmin = *(const double *)(pay + off + 10);
            double val  = *(const double *)(pay + off + 18);
            double tx   = *(const double *)(pay + off + 26);
            if (rec || val != 0.0)
                fprintf(out, "    %-12s rec=%d talk=%.2f paid=%.2f val=%.2f tax=%.2f\n",
                       SvcName(s), rec, tmin, pmin, val, tx);
        }
    }
}

static void CsvData(const BYTE *page, FILE *out)
{
    const PageHeader *hdr = (const PageHeader *)page;
    UINT nk = hdr->NumKeys;
    for (UINT sl = 0; sl < nk; sl++)
    {
        const ReceiptDump *r = (const ReceiptDump *)(page + MINIDB_HDR_SIZE + sl * 111);
        if (r->MagicNumber != RECEIPT_MAGIC) continue;
        fprintf(out, "%ld,%d,%d,%d,%.21s,%.17s,%d,%ld,%.2f,%.2f,%.4f,%.1f,%d\n",
                r->Number, r->BoothNumber, r->Date, r->Time,
                r->City, r->Phone, r->Amount, r->ElapsedTime,
                r->Value, r->Tax, r->ValuePerMin, r->CeilMin, r->Percent);
    }
}

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------

static void usage(const char *prog)
{
    printf("Usage: %s [options] [path]\n", prog ? prog : "mdbdump");
    printf("  path           .db file (default: RX.db)\n");
    printf("  -h             Show this help\n");
    printf("  -b             Skip free pages\n");
    printf("  --csv-receipts Receipts as CSV\n");
    printf("  --csv-stats    Statistics as CSV\n");
}

int main(int argc, char *argv[])
{
    const char *path = "RX.db";
    int brief = 0, csvR = 0, csvS = 0;

    for (int i = 1; i < argc; i++)
    {
        if      (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) { usage(argv[0]); return 0; }
        else if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--brief") == 0) brief = 1;
        else if (strcmp(argv[i], "--csv-receipts") == 0) csvR = 1;
        else if (strcmp(argv[i], "--csv-stats") == 0) csvS = 1;
        else path = argv[i];
    }

    if (csvR) printf("number,booth,date,time,city,phone,amount,elapsed_s,value,tax,vpm,ceil_min,percent\n");
    if (csvS) printf("period,service,receipts,talk_min,paid_min,value,tax\n");

    int fd = open(path, O_RDONLY | O_BINARY);
    if (fd == -1) { fprintf(stderr, "ERROR: cannot open '%s'\n", path); return 1; }

    long fileSize = lseek(fd, 0L, SEEK_END);
    lseek(fd, 0L, SEEK_SET);
    int nPages = (int)(fileSize / MINIDB_PAGE_SIZE);

    for (int pn = 0; pn < nPages; pn++)
    {
        BYTE page[MINIDB_PAGE_SIZE];
        if (read(fd, page, sizeof(page)) != (int)sizeof(page)) break;
        const PageHeader *hdr = (const PageHeader *)page;
        if (hdr->Magic != MINIDB_MAGIC) continue;

        // CSV-only modes: print the requested data and skip everything else
        if (csvR)
        {
            if (hdr->PageType == PAGE_DATA) CsvData(page, stdout);
            continue;
        }
        if (csvS)
        {
            if (hdr->PageType == PAGE_STATS)
            {
                const BYTE *pay = page + MINIDB_HDR_SIZE;
                for (int p = 0; p < 5; p++)
                    for (int s = 0; s < 6; s++)
                    {
                        int off = (p * 6 + s) * 34;
                        if (off + 34 > (int)MINIDB_PAYLOAD) break;
                        int  rec = *(const int *)(pay + off);
                        double tmin = *(const double *)(pay + off + 2);
                        double pmin = *(const double *)(pay + off + 10);
                        double val = *(const double *)(pay + off + 18);
                        double tx = *(const double *)(pay + off + 26);
                        if (rec || val != 0.0)
                            printf("%s,%s,%d,%.2f,%.2f,%.2f,%.2f\n",
                                   PeriodName(p), SvcName(s), rec, tmin, pmin, val, tx);
                    }
            }
            continue;
        }

        if (brief && hdr->PageType == PAGE_FREE) continue;

        printf("============================================================\n");
        printf("Page %d  [%s]  offset=0x%04lx\n", pn, PageTypeName(hdr->PageType),
               (long)pn * MINIDB_PAGE_SIZE);
        printf("============================================================\n");

        switch (hdr->PageType)
        {
        case PAGE_DBINFO:  DumpDBInfo(page, stdout); break;
        case PAGE_BTREE_L: DumpLeaf(page, stdout); break;
        case PAGE_DATA:    DumpData(page, stdout); break;
        case PAGE_STATS:   DumpStats(page, stdout); break;
        case PAGE_FREE:    printf("  (free, rightSibling=%ld)\n", hdr->RightSibling); break;
        default:           printf("  (type %u)\n", hdr->PageType); break;
        }
        putchar('\n');
    }

    close(fd);
    if (!csvR && !csvS)
        printf("Summary: %d pages, %ld bytes\n", nPages, fileSize);
    return 0;
}
