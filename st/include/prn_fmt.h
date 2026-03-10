#ifndef __PRN_FMT_H
#define __PRN_FMT_H

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

#ifdef DOSX286
#include <phapi.h>
#endif

class PrnFormatter
{
public:
    PrnFormatter(WORD code)
    {
        load(code);
    }
    ~PrnFormatter()
    {
        unload();
    }
    //
    BOOL load(WORD code);
    BOOL unload(void);
    BOOL isLoaded ()
    {
        return moduleOk;
    }
    //
    // services
    //
    // config and others
    BOOL (APIENTRY *getConfigFmt)(char far **format);
    BOOL (APIENTRY *getHeaderFmt)(char far **format);
    BOOL (APIENTRY *getShortHeaderFmt)(char far **format);
    BOOL (APIENTRY *getPrePaidFmt)(char far **format);
    BOOL (APIENTRY *getRemFmt)(char far **format);
    BOOL (APIENTRY *getSummaryFmt)(char far **format);
    BOOL (APIENTRY *getFooterFmt)(char far **format);
    BOOL (APIENTRY *getFFFmt)(char far **format);
    BOOL (APIENTRY *getLFFmt)(char far **format);
    BOOL (APIENTRY *getLogTitleFmt)(char far **format);
    // normal receipts
    BOOL (APIENTRY *nrGetFmt)(char far **format);
    // special receipts
    BOOL (APIENTRY *srGetTelFmt)(char far **format);
    BOOL (APIENTRY *srGetFaxFmt)(char far **format);
    BOOL (APIENTRY *srGetTelexFmt)(char far **format);
    BOOL (APIENTRY *srGetMagneticCardFmt)(char far **format);
    BOOL (APIENTRY *srGetOtherFmt)(char far **format);
    // statistics
    BOOL (APIENTRY *stGetTurnTitleFmt)(char far **format);
    BOOL (APIENTRY *stGetNotTurnTitleFmt)(char far **format);
    BOOL (APIENTRY *stGetNotPaidFmt)(char far **format);
    BOOL (APIENTRY *stGetNormalFmt)(char far **format);
    BOOL (APIENTRY *stGetNalSpecialFmt)(char far **format);
    BOOL (APIENTRY *stGetInterSpecialFmt)(char far **format);
    BOOL (APIENTRY *stGetOtherSpecialFmt)(char far **format);
    BOOL (APIENTRY *stGetSpecialTotalFmt)(char far **format);
    BOOL (APIENTRY *stGetEDATotalFmt)(char far **format);
    BOOL (APIENTRY *stGetTotalFmt)(char far **format);
    BOOL (APIENTRY *stGetDoublePrnFmt)(char far **format);
private:
    HMODULE moduleHandle;
    BOOL     moduleOk;
};

#endif // __PRN_FMT_H
