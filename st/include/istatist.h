#ifndef __ISTATIST_H
#define __ISTATIST_H

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

#if !defined(__RECEIPT_H)
#include <receipt.h>
#endif

#if !defined(__DS_ENTRY_H)
#include <ds_entry.h>
#endif

class IReceiptStorage;

class IStatisticsStorage
{
public:
    virtual ~IStatisticsStorage() {}

    virtual void Flush()                                                 = 0;
    virtual BOOL Archive()                                               = 0;
    virtual BOOL Repair(class IReceiptStorage *receiptStorage,
                        BOOL all = FALSE)                               = 0;
    virtual DS_ENTRY          *operator[](WORD type)                     = 0;
    virtual DS_DOUBLEPRNENTRY *GetDoublePrnEntry(WORD ofs)              = 0;
    virtual DS_CELLULARENTRY  *GetCellularEntry(WORD type)              = 0;
    virtual BOOL Add(Receipt& receipt, BOOL isNew = TRUE)               = 0;
    virtual BOOL Subtract(Receipt& receipt)                              = 0;
    virtual BOOL Update()                                                = 0;
    virtual inline void SetErrors(WORD dialErrors, WORD commErrors)      = 0;
    virtual inline long GetTelEntries()                                  = 0;
    virtual WORD GetStatus()                                             = 0;
    virtual BOOL IsReadOnly()                                            = 0;
};

#endif // __ISTATIST_H
