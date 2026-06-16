//
// Quick smoke test for MiniDB — standalone, no Zinc UI.
// Build: bcc286 -c +st.cfg -D__DEMO__;NDEBUG -otest_minidb.obj src\db\minidb\test_minidb.cpp
// Link: tlink /c /s /Lvendor\bc\lib c0s test_minidb,test_minidb.exe,,cs
//

#include <stdio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <minidb/page.h>
#include <minidb/cache.h>
#include <minidb/btree.h>
#include <mdb_stor.h>

int main(int argc, char *argv[])
{
    const char *dbpath = "test_minidb.db";
    unlink(dbpath);
    unlink("test_minidb.db.new");

    printf("=== MiniDB smoke test ===\n");

    // Create storage
    MiniDBReceiptStorage *stor = new MiniDBReceiptStorage(".", "test_minidb", FALSE);
    printf("Status: %d\n", stor->GetStatus());
    printf("Entries: %ld\n", stor->GetEntries());
    printf("StatsPage: %ld\n", stor->GetStatsPage());

    // Check .db file size
    int fd = open(dbpath, O_RDONLY | O_BINARY);
    long sz = lseek(fd, 0, SEEK_END);
    close(fd);
    printf("File size: %ld bytes\n", sz);
    printf("Pages: %ld\n", sz / MINIDB_PAGE_SIZE);

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
    BOOL ok = stor->Add(r);
    printf("%s\n", ok ? "OK" : "FAIL");
    printf("Entries after add: %ld\n", stor->GetEntries());

    // Flush
    stor->Flush();

    // Check file size after adding
    fd = open(dbpath, O_RDONLY | O_BINARY);
    sz = lseek(fd, 0, SEEK_END);
    close(fd);
    printf("File size after add: %ld bytes (%ld pages)\n", sz, sz / MINIDB_PAGE_SIZE);

    // Read back
    Receipt r2;
    memset(&r2, 0, sizeof(r2));
    ok = stor->Get(r2, 1, 1);
    if (ok)
    {
        printf("Read back: Number=%ld Booth=%d Value=%.2f Magic=0x%04X\n",
               r2.Number, r2.BoothNumber, r2.Value, r2.MagicNumber);
    }
    else
    {
        printf("Read back: FAILED\n");
    }

    // Check first/last
    printf("First: %ld, Last: %ld, Lower: %ld, Higher: %ld\n",
           stor->GetFirstNumber(), stor->GetLastNumber(),
           stor->GetLowerNumber(), stor->GetHigherNumber());

    // Delete
    printf("Deleting receipt #1... ");
    ok = stor->Delete(1, 1);
    printf("%s\n", ok ? "OK" : "FAIL");
    printf("Entries after delete: %ld\n", stor->GetEntries());

    // Archive
    printf("\nArchiving... ");
    // Need _mkSysDateDir to work — set up pseudo-date first
    // (in real app these are defined)
    ok = stor->Archive();
    printf("%s\n", ok ? "OK" : "FAIL");

    // Check file gone
    fd = open(dbpath, O_RDONLY | O_BINARY);
    if (fd == -1)
        printf("File removed as expected\n");
    else
    {
        close(fd);
        printf("File still exists (unexpected)\n");
    }

    // Clean up
    delete stor;
    unlink(dbpath);

    printf("\n=== TEST COMPLETE ===\n");
    return 0;
}
