//
// [ CHSERIAL.CPP ]
//

#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <conio.h>
//
#include <info.h>
#include <util.h>

int main(void)
{
    cout << "CHSERIAL version 1.0 Copyright (c) 1996 MicroDise¤o Ltda." << endl;
    cout << "C¢digo de acceso: ";
    STR32 password;
    _ReadPassword(password, sizeof(STR32)-1);
    if (!strlen(password))
    {
        return 1;
    }
    if (strcmp(password, "CER"))
    {
        cerr << "Error en acceso !" << endl;
        return 2;
    }
    FILE_NAME filename;
    cout << "Ruta y nombre de archivo de ST (ej. c:\\st\\st.exe): ";
    cin >> filename;
    extern SUPER_APP_INFO _superAppInfo;
    int result = _GetPatchFromFile(filename, _superAppInfo.Key, &_superAppInfo.Attr, sizeof(WORD)+sizeof(APP_INFO));
    if (result != PATCH_OK)
    {
        cerr << "El serial no pudo ser hallado ..." << endl;
        return 3;
    }
    if (_superAppInfo.Attr.Serialized)
        _Decrypt(&_superAppInfo.Data, sizeof(APP_INFO));
    SERIAL_NUMBER serial;
    cout << "Serial: " << _superAppInfo.Data.Serial << endl;
    cout << "Nuevo serial: ";
    cin >> serial;
    // put the new patch
    strcpy(_superAppInfo.Data.Serial, serial);
    strncpy(_superAppInfo.Data.ShortSerial, &_superAppInfo.Data.Serial[3], 4);
    //
    if (_superAppInfo.Attr.Serialized)
        _Encrypt(&_superAppInfo.Data, sizeof(APP_INFO));
    result = _PatchFile(filename, _superAppInfo.Key, &_superAppInfo.Attr, sizeof(WORD)+sizeof(APP_INFO));
    if (result != PATCH_OK)
    {
        cerr << "El serial no pudo ser cambiado ..." << endl;
        return 4;
    }
    return 0;
}