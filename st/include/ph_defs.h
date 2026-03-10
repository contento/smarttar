#ifndef __PH_DEFS_H
#define __PH_DEFS_H

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

// ---
// Warning:
// Since the new standar (international standar).
// stablish 0 as the access code and the atol conversion doesn't distinguish
// between 0n an n we have to use the most significant bits, in such a way
// we keep the maximum of 9 digits.
// GCC/gcc. May 9, 1995.
// ---
const DWORD RANGE_INDICATOR_MASK = 0x80000000UL;

const WORD MAX_DDN_TARIFF   = 16;
const WORD MAX_DDI_TARIFF   = 20;

const WORD MAX_DDN_SCHEDULE = 5;
const WORD MAX_DDI_SCHEDULE = 6;

const WORD MAX_DDN_DAY_TYPE = 3;
const WORD MAX_DDI_DAY_TYPE = 3;

const WORD MAX_INFO_SLOTS       = 10;
const WORD MAX_LOCKED_NUMBERS   = 20;
const WORD MAX_NUMBERS_PER_LINE = 50;

#endif // __PH_DEFS_H