//
// [PR_FX.C]
//
#include "dll_defs.h"

int PASCAL_EXPORT getFormat(char far **format)
{
    int ok = 1;
    *format = "FX %s";
    return ok;
}

