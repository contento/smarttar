//
// [ PATCH.CPP ]
//

#include <iostream.h>
#include <string.h>
#include <info.h>
#include <util.h>

int main(int argc, char *argv[])
{
    cout << "Patch version 2.0 Copyright (c) 1995 MicroDise¤o Ltda." << endl;
    if (argc < 3)
    {
        cerr << "Usage: PATCH Serial /2|/4" << endl;
        return 1;
    }
    if (strlen(argv[1]) != 19)
    {
        cerr << "Bad serial number: " << argv[1] << endl;
        return 2;
    }
    if (strcmp(argv[2], "/2") && strcmp(argv[2], "/4"))
    {
        cerr << "Bad option: " << argv[2] << endl;
        return 2;
    }
    extern TSuperAppInfo _superAppInfo;
    _superAppInfo.Serialized = TRUE;
    _superAppInfo.Data.MaxClusters = argv[2][1]-'0';
    if (_superAppInfo.Data.MaxClusters == 2)
        strcpy(_superAppInfo.Data.Title, "SmartTar ST16");
    strcpy(_superAppInfo.Data.Serial, argv[1]);
    strncpy(_superAppInfo.Data.ShortSerial, &_superAppInfo.Data.Serial[3], 4);
    _Encrypt(&_superAppInfo.Data, sizeof(TAppInfo));
    //
    cout << "Patching...";
    // look out: from Serialized !!!
    int result = _PatchFile("ST.EXE", _superAppInfo.Key, &_superAppInfo.Serialized, sizeof(BOOL)+sizeof(TAppInfo));
    char *msg = "Ok: file patched.";
    int retVal = 3;
    switch (result)
    {
    case PATCH_OK            :
        retVal = 0;
        break;
    case PATCH_ID_NOT_FOUND  :
        msg = "Error: Id not found.";
        break;
    case PATCH_FILE_NOT_FOUND:
        msg = "Error: File not found.";
        break;
    case PATCH_GEN_FAIL      :
        msg = "Error: General fail.";
        break;
    }
    cout << endl << msg << endl;
    return retVal;
}