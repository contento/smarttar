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

const UINT UP   = 0xFF48;
const UINT DOWN = 0xFF50;
const UINT ESC  = 0x001B;

static UINT GetScanCode(void)
{
    UINT scancode;
    scancode = getch();
    if (!scancode)
        return (0xFF00 | getch());
    else
        return scancode;
}

int main(void)
{
    cout << "DBStorage Cursor" << endl;
    cout << "Receipt: ";
    int x = wherex();
    int y = wherey();
    DB_STORAGE dbStorage(NULL, "RX");
    DB_STORAGE_CURSOR dbCursor(&dbStorage);
    RECEIPT *receipt;
    BOOL ok = TRUE;
    while (ok)
    {
        receipt = NULL;
        switch (GetScanCode())
        {
        case UP  :
            dbCursor--;
            receipt = dbCursor.GetCurrent();
            break;
        case DOWN:
            dbCursor++;
            receipt = dbCursor.GetCurrent();
            break;
        case ESC :
            ok = FALSE;
            break;
        }
        gotoxy(x, y);
        if (receipt)
            cprintf("%5ld", receipt->Number);
        if (!dbCursor)
            cprintf("%5c", '!');
    }
    return 0;
}
