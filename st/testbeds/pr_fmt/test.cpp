//
// [TEST.CPP]
//
#if !defined(__CFG_H)
#include <cfg.h>
#endif

#if !defined(__PRN_FMT_H)
#include <prn_fmt.h>
#endif

#include <stdio.h>

void main(void)
{
    PrnFormatter *prnFormatter = new PrnFormatter(CFG::SR_80);
    // use
    char *format;
    if (prnFormatter->stGetTurnTitleFmt(&format))
    {
        puts(format);
    }
    //
    prnFormatter->unload();
    /*
    prnFormatter->load(CFG::DR_80);
    if (prnFormatter->get(&format)) {
      puts(format);
}
    */
    delete prnFormatter;
}

