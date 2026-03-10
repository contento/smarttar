//
// [TEST.CPP]
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>

#define FALSE 0
#define TRUE  1

class PrnFormatter
{
public:
    enum Printers {
        FX_EPSON,
        TM_EPSON
    };
    PrnFormatter(int code)
    {
        load(code);
    }
    ~PrnFormatter()
    {
        unload();
    }
    //
    BOOL load(int code);
    BOOL unload(void);
    BOOL isLoaded ()
    {
        return moduleOk;
    }
    // services
    BOOL (APIENTRY *get)(char far **format);
private:
    HMODULE moduleHandle;
    BOOL     moduleOk;
};

BOOL PrnFormatter::load(int code)
{
    moduleOk = TRUE;
    const char *PrnFormatter_PREFIX = "PR_";
    char PrnFormatterDLLName[9];
    strcpy(PrnFormatterDLLName, PrnFormatter_PREFIX);
    switch (code)
    {
    case FX_EPSON:
        strcat(PrnFormatterDLLName, "FX");
        break;
    case TM_EPSON:
        strcat(PrnFormatterDLLName, "TM");
        break;
    default:
        moduleOk = FALSE;
        break;
    }
    moduleOk &= (DosLoadModule(NULL, 0, PrnFormatterDLLName, &moduleHandle) == 0);
    if (moduleOk)
    {
        moduleOk = DosGetProcAddr(moduleHandle, "GETFORMAT", (PPFN)&get) == 0;
    }
    else
    {
        moduleHandle = 0;
    }
    return moduleOk;
}

BOOL PrnFormatter::unload(void)
{
    BOOL ok = moduleOk;
    if (ok)
        ok = DosFreeModule(moduleHandle);
    moduleOk = FALSE;
    return ok;
}

void main(void)
{
    const char *MESSAGE = " lo formateo.\n";
    PrnFormatter *prnFormatter = new PrnFormatter(PrnFormatter::FX_EPSON);
    // use
    char *format;
    if (prnFormatter->get(&format))
        printf(format, MESSAGE);
    else
        printf("No se pudo conseguir el formato.\n");
    //
    prnFormatter->unload();
    prnFormatter->load(PrnFormatter::TM_EPSON);
    if (prnFormatter->get(&format))
        printf(format, MESSAGE);
    else
        printf("No se pudo conseguir el formato.\n");
    //
    delete prnFormatter;
}

