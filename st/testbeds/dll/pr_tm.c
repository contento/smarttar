//
// [PR_TM.C]
//
#include "dll_defs.h"

int PASCAL_EXPORT getFormat(char far **format)
{
    int ok = 1;
    *format = "TM %s";
    return ok;
}

