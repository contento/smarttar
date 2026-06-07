//
// [ TEST.CPP ]
//

// Microlab Runs at 9200 after Assembly

#include <bdisplay.h>
#include <iostream.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>

int main(void)
{
    BoothDisplay boothDisplay;
    boothDisplay.install("Probando ...", SERIAL::COM2, SERIAL::S9600);
    if (boothDisplay.isInstalled())
    {
        cout
        << "Installed and sending..." << endl
        << "    M: Set default message" << endl
        << "    H: Send on-hook"    << endl
        << "    D: Send off-hook"   << endl
        << "    P: Send phone"      << endl
        << "    V: Send value"      << endl
        << "    L: Send Locked"     << endl
        << "    E: Send Dial Error" << endl
        << "    C: Send Comm Error" << endl
        << "    S: Send Spy"        << endl
        << "  ESC: Quit"            << endl
        ;
        //
        char booth = 1; // 0..31
        char key;
        BOOL ok = TRUE;
        while (ok)
        {
            if (kbhit())
            {
                key = toupper(getch());
                switch (key)
                {
                case 'M':
                    { // Colgar
                        cout << "Default mesasage sent" << endl;
                        boothDisplay.setDefaultMessage("Mario se mario");
                        break;
                    }
                case 'H':
                    { // Colgar
                        cout << "On-Hook sent" << endl;
                        boothDisplay.showDefaultMessage(booth);
                        break;
                    }
                case 'D':
                    { // descolgar
                        cout << "Off-Hook sent" << endl;
                        boothDisplay.showOffHook(booth);
                        break;
                    }
                case 'L':
                    { // locked
                        cout << "Locked sent" << endl;
                        boothDisplay.showLocked(booth);
                        break;
                    }
                case 'E':
                    { // dial err
                        cout << "Dial error sent" << endl;
                        boothDisplay.showDialErr(booth);
                        break;
                    }
                case 'C':
                    { // comm err
                        cout << "Comm error sent" << endl;
                        boothDisplay.showCommErr(booth);
                        break;
                    }
                case 'S':
                    { // spy
                        cout << "Spy sent" << endl;
                        boothDisplay.showSpy(booth);
                        break;
                    }
                case 'P':
                    { // phone
                        cout << "Phone sent" << endl;
                        PHONE phone;
                        strcpy(phone, "0943414648");
                        boothDisplay.showDialing(booth, phone);
                        break;
                    }
                case 'V':
                    { // Colgar
                        cout << "Value sent" << endl;
                        boothDisplay.showComm(booth, 8.6, 1967.0);
                        break;
                    }
                case '\x1B':
                    { // ESC
                        ok = FALSE;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        cout << "Booth display couldn't be installed" << endl;
    }
    return 0;
}
