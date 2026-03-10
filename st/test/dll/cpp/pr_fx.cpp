//
// [PR_FX.C]
//
#include "dll_defs.h"

extern "C" int PASCAL_EXPORT getFormat(char far **format)
{
    int ok = 1;
    *format = "FX %s";
    return ok;
}

