//
// [PRN_FMT.CPP]
//

#include "stdst.h"

#include <prn_fmt.h>

BOOL PrnFormatter::load(WORD code)
{
#pragma warn -ucp
    moduleOk = TRUE;
    const unsigned char *PrnFormatter_PREFIX = "PR_";
    unsigned char PrnFormatterDLLName[9];
    strcpy(PrnFormatterDLLName, PrnFormatter_PREFIX);
    switch (code)
    {
    case CFG::DR_80:
        strcat(PrnFormatterDLLName, "DR80");
        break;
    case CFG::DR_PRE:
        strcat(PrnFormatterDLLName, "DRPRE");
        break;
    case CFG::LINEAL_80:
        strcat(PrnFormatterDLLName, "LIN80");
        break;
    case CFG::SR_80:
        strcat(PrnFormatterDLLName, "SR80");
        break;
    case CFG::DR_40:
        strcat(PrnFormatterDLLName, "DR40");
        break;
    case CFG::SR_40:
        strcat(PrnFormatterDLLName, "SR40");
        break;
    case CFG::DR_18:
        strcat(PrnFormatterDLLName, "DR18");
        break;
    case CFG::SR_28:
        strcat(PrnFormatterDLLName, "SR28");
        break;
    case CFG::DR_EME:
        strcat(PrnFormatterDLLName, "DREME");
        break;
    case CFG::DR_HALF:
        strcat(PrnFormatterDLLName, "DRHAL");
        break;
    default:
        moduleOk = FALSE;
        break;
    }
    moduleOk &= (DosLoadModule(NULL, 0, PrnFormatterDLLName, &moduleHandle) == 0);
    if (moduleOk)
    {
        // config and others
        DosGetProcAddr(moduleHandle, "GETCONFIGFMT", (PPFN)&getConfigFmt);
        DosGetProcAddr(moduleHandle, "GETHEADERFMT", (PPFN)&getHeaderFmt);
        DosGetProcAddr(moduleHandle, "GETSHORTHEADERFMT", (PPFN)&getShortHeaderFmt);
        DosGetProcAddr(moduleHandle, "GETPREPAIDFMT", (PPFN)&getPrePaidFmt);
        DosGetProcAddr(moduleHandle, "GETREMFMT", (PPFN)&getRemFmt);
        DosGetProcAddr(moduleHandle, "GETSUMMARYFMT", (PPFN)&getSummaryFmt);
        DosGetProcAddr(moduleHandle, "GETFOOTERFMT", (PPFN)&getFooterFmt);
        DosGetProcAddr(moduleHandle, "GETFFFMT", (PPFN)&getFFFmt);
        DosGetProcAddr(moduleHandle, "GETLFFMT", (PPFN)&getLFFmt);
        DosGetProcAddr(moduleHandle, "GETLOGTITLEFMT", (PPFN)&getLogTitleFmt);
        // normal receipts
        DosGetProcAddr(moduleHandle, "NRGETFMT", (PPFN)&nrGetFmt);
        // special receipts
        DosGetProcAddr(moduleHandle, "SRGETTELFMT", (PPFN)&srGetTelFmt);
        DosGetProcAddr(moduleHandle, "SRGETFAXFMT", (PPFN)&srGetFaxFmt);
        DosGetProcAddr(moduleHandle, "SRGETTELEXFMT", (PPFN)&srGetTelexFmt);
        DosGetProcAddr(moduleHandle, "SRGETMAGNETICCARDFMT", (PPFN)&srGetMagneticCardFmt);
        DosGetProcAddr(moduleHandle, "SRGETOTHERFMT", (PPFN)&srGetOtherFmt);
        // statistics
        DosGetProcAddr(moduleHandle, "STGETTURNTITLEFMT", (PPFN)&stGetTurnTitleFmt);
        DosGetProcAddr(moduleHandle, "STGETNOTTURNTITLEFMT", (PPFN)&stGetNotTurnTitleFmt);
        DosGetProcAddr(moduleHandle, "STGETNOTPAIDFMT", (PPFN)&stGetNotPaidFmt);
        DosGetProcAddr(moduleHandle, "STGETNORMALFMT", (PPFN)&stGetNormalFmt);
        DosGetProcAddr(moduleHandle, "STGETNALSPECIALFMT", (PPFN)&stGetNalSpecialFmt);
        DosGetProcAddr(moduleHandle, "STGETINTERSPECIALFMT", (PPFN)&stGetInterSpecialFmt);
        DosGetProcAddr(moduleHandle, "STGETOTHERSPECIALFMT", (PPFN)&stGetOtherSpecialFmt);
        DosGetProcAddr(moduleHandle, "STGETSPECIALTOTALFMT", (PPFN)&stGetSpecialTotalFmt);
        DosGetProcAddr(moduleHandle, "STGETEDATOTALFMT", (PPFN)&stGetEDATotalFmt);
        DosGetProcAddr(moduleHandle, "STGETTOTALFMT", (PPFN)&stGetTotalFmt);
        DosGetProcAddr(moduleHandle, "STGETDOUBLEPRNFMT", (PPFN)&stGetDoublePrnFmt);
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