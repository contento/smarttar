//
// [ RWEEPROM.CPP ]
//
#include <iostream.h>
#include <stdio.h>
#include <string.h>

#include <st_util.h>
#include <eeprom.h>

int main(int argc, char *argv[])
{
    cout
    << "RWEEPROM 1.0 (" << APP_VER_NAME << ')' << endl
    << APP_COPYRIGHT << endl
    << "  rweeprom [/w]"    << endl
    << "    /w para grabar" << endl
    << endl
    ;
    STR32 password;
    cout << "Presione Esc para abortar operaci¢n." << endl;
    cout << "C¢digo de acceso: ";
    _ReadPassword(password, sizeof(password)-1);
    if (!_isDatePassword(password))
    {
        cout << "Lo siento, acceso negado." << endl;
        return 1;
    }
    memset(password, 0, sizeof(password)); // don't watch password.
    BOOL writing;
    writing = (argc > 1) && (!strcmp(argv[1], "/W") || !strcmp(argv[1], "/w"));
    EEPROM eeprom;
    const int nBytes = 80;
    BYTE bytes[nBytes];
    if (writing)
    {
        cout << "  Bytes: ";
        cin.getline(bytes, nBytes);
        cout << "  Escribiendo [" << bytes << "] ..." << endl;
        eeprom.write(bytes, strlen(bytes)+1); // incluido ASCIIZ
        // FEEDBACK
        cout << "  Leyendo ..." << endl;
        eeprom.read(bytes, nBytes);
        cout << "  Bytes escritos: [" << bytes << "]" << endl;
    }
    else
    {
        cout << "  Leyendo ..." << endl;
        eeprom.read(bytes, nBytes);
        cout << "  Bytes leidos: [" << bytes << "]" << endl;
    }
    //
    return 0;
}
