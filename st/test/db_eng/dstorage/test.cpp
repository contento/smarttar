//
// [ TEST.CPP ]
//
// Feb 20 1995
// GCC/gcc
//

#if !defined(__STRING_H)
#include <string.h>
#endif

#if !defined(__CTYPE_H)
#include <ctype.h>
#endif

#if !defined(__IOSTREAM_H)
#include <iostream.h>
#endif

#if !defined(__CONSTREA_H)
#include <constream.h>
#endif

#if !defined(__DSTORAGE_H)
#include <dstorage.h>
#endif


static char GetOption(const char *options);

RECEIPT _receipt;

int main(void)
{
    _receipt.Tag = RECEIPT::TEL;
    strcpy(_receipt.City, "Armenia (Qui)");
    strcpy(_receipt.Phone, "3414648");
    _receipt.ElapsedTime = 10L;
    //
    BOOL ok = TRUE, doPause = TRUE;
    DB_STORAGE dbStorage(NULL, "RX", FALSE);
    //
    _receipt.Number = dbStorage.GetHigherNumber();
    do
    {
        clrscr();
        cout
        << "[ SmartTar DBStorage Test ]"   << endl
        << endl
        << "   Size: " << sizeof(RECEIPT) << " bytes per record."<< endl
        << "Records: " << dbStorage.GetEntries() << endl
        << "  Range: " << dbStorage.GetLowerNumber() << ".." << dbStorage.GetHigherNumber() << endl
        << endl
        << "[I]nsert 1000 records" << endl
        << "[G]et a record"        << endl
        << "[M]odify a record"     << endl
        << "[F]lush"               << endl
        << "[R]epair Database"     << endl
        << "Reinde[x]"             << endl
        << "[L]ist records"        << endl
        << "[Q]uit (Esc)"          << endl
        << endl
        << "Option: "
        ;
        switch (GetOption("XIGFLMRQ\x1B"))
        {
        case 'I':
            {
                for (WORD i=0; i < 1000; i++)
                {
                    _receipt.MagicNumber = dbStorage.MAGIC_NUMBER;
                    _receipt.Stat.Paid   = RECEIPT::PAID;
                    _receipt.Number++;
                    dbStorage + _receipt;
                }
                doPause = FALSE;
                break;
            }
        case 'G':
            {
                long number;
                cout << endl << "Record: ";
                cin >> number;
                RECEIPT *receipt = dbStorage[number];
                if (receipt)
                {
                    cout << receipt->Number << ' ' << receipt->City << ' ' << receipt->ElapsedTime << endl;
                }
                else
                {
                    cout << number << " Not found ..." << endl;
                }
                break;
            }
        case 'M':
            {
                long number;
                cout << endl << "Record: ";
                cin >> number;
                RECEIPT *receipt = dbStorage[number];
                if (receipt)
                {
                    cout << receipt->Number << ' ' << receipt->City << ' ' << receipt->ElapsedTime << endl;
                    cout << "City: ";
                    RECEIPT tmpReceipt;
                    tmpReceipt = *receipt;
                    cin >> tmpReceipt.City;
                    dbStorage << tmpReceipt;
                }
                else
                {
                    cout << number << " Not found ..." << endl;
                }
                break;
            }
        case 'F':
            {
                cout << endl << "Flushing ...";
                dbStorage.Flush();
                doPause = FALSE;
                break;
            }
        case 'R':
            {
                cout << endl << "Repairing ...";
                if (!dbStorage.Repair())
                    cerr << "Error repairing database ..." << endl;
                else
                    doPause = FALSE;
                break;
            }
        case 'X':
            {
                cout << endl << "Reindexing ...";
                if (!dbStorage.RepairIndexFile())
                    cerr << "Error reindexing database ..." << endl;
                else
                    doPause = FALSE;
                break;
            }
        case 'L':
            {
                clrscr();
                DB_STORAGE_CURSOR dbCursor(&dbStorage);
                RECEIPT *receipt;
                while (dbCursor)
                {
                    receipt = dbCursor.GetCurrent();
                    if (receipt)
                        cout << receipt->Number << ' ' << receipt->City << ' ' << receipt->ElapsedTime << endl;
                    dbCursor++;
                }
                cout << "No More ...";
                break;
            }
        case '\x1B':
        case 'Q' :
            ok = FALSE;
            doPause = FALSE;
            break;
        }
        if (doPause)
        {
            cout << endl << endl << "Press any key to continue..." ;
            getch();
        }
        doPause = TRUE;
    }
    while (ok);
    return (0);
}

char GetOption(const char *options)
{
    char byte;
    do
    {
        byte = toupper(getch());
    }
    while (!strchr(options, byte));
    return byte;
}
