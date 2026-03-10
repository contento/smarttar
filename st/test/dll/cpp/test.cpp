//
// [TEST.CPP]
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

class Prn
{
public:
    enum Printers {
        FX_EPSON,
        TM_EPSON
    };
    Prn(int code)
    {
        dllHandle = 0;
        load(code);
    }
    ~Prn()
    {
        unload();
    }
    //
    int load(int code);
    int unload(void);
    //
    int getFormat(char **format);
private:
    HMODULE dllHandle;
};

int Prn::load(int code)
{
    int ok = 1;
    const char *PRN_PREFIX = "PR_";
    char prnDLLName[9];
    strcpy(prnDLLName, PRN_PREFIX);
    switch (code)
    {
    case FX_EPSON:
        strcat(prnDLLName, "FX");
        break;
    case TM_EPSON:
        strcat(prnDLLName, "TM");
        break;
    default:
        ok = 0;
        break;
    }
    ok &= (DosLoadModule(NULL, 0, prnDLLName, &dllHandle) == 0);
    if (!ok)
        dllHandle = 0;
    return ok;
}

int Prn::unload(void)
{
    int ok = (dllHandle != 0);
    if (ok)
    {
        ok &= DosFreeModule(dllHandle);
    }
    return ok;
}

int Prn::getFormat(char **format)
{
    int ok = (dllHandle != 0);
    int (far *dllGetFormat)(char **format);
    if (ok)
        ok = DosGetProcAddr(dllHandle, "GETFORMAT", (PPFN)&dllGetFormat) == 0;
    if (ok)
        ok = dllGetFormat(format);
    return ok;
}

void main(void)
{
    const char *MESSAGE = " lo formateo.\n";
    Prn *prn = new Prn(Prn::FX_EPSON);
    // use
    char *format;
    if (prn->getFormat(&format))
        printf(format, MESSAGE);
    else
        printf("No se pudo conseguir el formato.\n");
    //
    delete prn;
}

