//
// [ TEST.CPP ]
//
#include <iostream.h>
#include <serial.h>
#include <dos.h>

int main(void)
{
    SERIAL serial(SERIAL::COM2, SERIAL::S9600);
    if (serial.IsInstalled())
    {
        cout << "Installed and sending ..." << endl;
        //
        serial.Put('G');
        serial.Put('C');
        serial.Put('C');
        //
    }
    else
    {
        cout << "Serial port not installed on COM2" << endl;
    }
    return 0;
}
