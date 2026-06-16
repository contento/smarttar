//
// MiniDB standalone test — compiles and runs inside DOSBox-X.
// Build: bcc286 -c +st.cfg -D__DEMO__;NDEBUG -obuild\test_mdb.obj src\db\minidb\test_mdb.cpp
// Link: tlink /c /C /s /L..\vendor\bc\lib;..\vendor\pharlap\lib;..\vendor\zinc\lib;lib build\test_mdb.obj,bin\test_mdb.exe,,cs
//
// Run:   cd bin && test_mdb
//

#include <stdio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <st_defs.h>
#include <receipt.h>
#include <minidb/page.h>
#include <minidb/cache.h>
#include <minidb/btree.h>
#include <mdb_stor.h>

int main(int argc, char *argv[])
{
    printf("MiniDB standalone test\n");
    printf("=====================\n\n");

    // Delete any previous test db
    unlink("t.db");
    unlink("t.db.new");

    // Create storage — this creates the .db file
    MiniDBReceiptStorage *st = new MiniDBReceiptStorage(".", "t", FALSE);

    printf("Status: %d\n", st->GetStatus());
    printf("File size: %ld\n", st->GetEntries());

    // Check .db file size
    int fd = open("t.db", O_RDONLY | O_BINARY);
    if (fd >= 0)
    {
        long sz = lseek(fd, 0, SEEK_END);
        close(fd);
        printf("File on disk: %ld bytes (%ld pages)\n", sz, sz / 512);
    }
    else
    {
        printf("ERROR: t.db not found!\n");
        delete st;
        return 1;
    }

    // Add a receipt
    Receipt r;
    memset(&r, 0, sizeof(r));
    r.MagicNumber = 0x6719;
    r.Number = 1;
    r.BoothNumber = 1;
    r.Value = 100.0;
    r.Date = 20260616;
    r.Time = 120000;

    printf("\nAdding receipt #1... ");
    BOOL ok = st->Add(r);
    printf("%s\n", ok ? "OK" : "FAIL");
    st->Flush();

    // Check file after add
    fd = open("t.db", O_RDONLY | O_BINARY);
    long sz = lseek(fd, 0, SEEK_END);
    close(fd);
    printf("File after add: %ld bytes (%ld pages)\n", sz, sz / 512);

    // Read back
    Receipt r2;
    memset(&r2, 0, sizeof(r2));
    ok = st->Get(r2, 1, 1);
    if (ok)
    {
        printf("Read: num=%ld booth=%d val=%.2f magic=0x%04X\n",
               r2.Number, r2.BoothNumber, r2.Value, r2.MagicNumber);
    }
    else
    {
        printf("Read: FAILED!\n");
    }

    printf("Stats: entries=%ld first=%ld last=%ld\n",
           st->GetEntries(), st->GetFirstNumber(), st->GetLastNumber());

    // Add a second receipt and verify ordering
    r.Number = 2;
    r.BoothNumber = 2;
    r.Value = 200.0;
    printf("Adding receipt #2... ");
    ok = st->Add(r);
    printf("%s\n", ok ? "OK" : "FAIL");
    st->Flush();

    printf("Stats after #2: entries=%ld first=%ld last=%ld\n",
           st->GetEntries(), st->GetFirstNumber(), st->GetLastNumber());

    fd = open("t.db", O_RDONLY | O_BINARY);
    sz = lseek(fd, 0, SEEK_END);
    close(fd);
    printf("File after 2 adds: %ld bytes (%ld pages)\n", sz, sz / 512);

    // Debug dump the DBInfo page
    BYTE buf[512];
    fd = open("t.db", O_RDONLY | O_BINARY);
    read(fd, buf, 512);
    close(fd);
    DBInfo *dbInfo = (DBInfo *)buf;
    printf("\nDBInfo: version=%u root=%ld seq=%ld rec=%ld archived=%ld stats=%ld free=%ld\n",
           dbInfo->Version, dbInfo->RootPage, dbInfo->Sequence,
           dbInfo->NumReceipts, dbInfo->NumArchived,
           dbInfo->StatsAnchor, dbInfo->FreeHead);

    // Dump page 1 (leaf page — first B-tree node)
    if (sz > 512)
    {
        read(fd = open("t.db", O_RDONLY | O_BINARY), buf, 512);
        read(fd, buf, 512);
        close(fd);
        PageHeader *ph = (PageHeader *)buf;
        printf("Page 1: magic=0x%04X type=%u numkeys=%u right=%ld\n",
               ph->Magic, ph->PageType, ph->NumKeys, ph->RightSibling);
        if (ph->PageType == PAGE_BTREE_L && ph->NumKeys > 0)
        {
            BTreeLeafEntry *e = (BTreeLeafEntry *)(buf + 16);
            for (UINT i = 0; i < ph->NumKeys && i < 10; i++)
                printf("  [%u] num=%ld booth=%d seek=%ld flags=%u\n",
                       i, e[i].Number, e[i].BoothNumber,
                       e[i].DataSeek, e[i].Flags);
        }
    }

    delete st;
    unlink("t.db");
    printf("\n=== OK ===\n");
    return 0;
}
