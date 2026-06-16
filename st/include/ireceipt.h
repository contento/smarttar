#ifndef __IRECEIPT_ST_H
#define __IRECEIPT_ST_H

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

#if !defined(__RECEIPT_H)
#include <receipt.h>
#endif

class IReceiptStorage
{
public:
    virtual ~IReceiptStorage() {}

    virtual int  GetStatus()            = 0;
    virtual BOOL IsReadOnly()           = 0;
    virtual BOOL IsCorrectNumber(long number) = 0;
    virtual BOOL IsValid(Receipt const &)      = 0;
    virtual BOOL Exist(long number, int boothNumber) const = 0;
    virtual BOOL Get(Receipt&, long number, int boothNumber = -1) = 0;
    virtual BOOL Add(const Receipt&)   = 0;
    virtual BOOL Update(const Receipt&)= 0;
    virtual BOOL Delete(long number, int boothNumber = -1) = 0;
    virtual long GetLowerNumber() const = 0;
    virtual long GetHigherNumber() const = 0;
    virtual long GetEntries() const     = 0;
    virtual long GetFirstNumber() const = 0;
    virtual long GetLastNumber() const  = 0;
    virtual BOOL Archive()             = 0;
    virtual BOOL Repair()              = 0;

    typedef BOOL (*CallbackFnPtr)(Receipt const &receipt);

    virtual void EnumReceipts(CallbackFnPtr callback) = 0;
    virtual void Flush()              = 0;

    // Returns a concrete DB_STORAGE pointer if the implementation is
    // FlatFile-based (DB_STORAGE).  Returns NULL for MiniDB or mock
    // backends.  Callers that need DB_STORAGE::Iterator etc. should
    // check this before casting.
    virtual class DB_STORAGE *GetConcreteStorage() { return NULL; }
};

#endif // __IRECEIPT_ST_H
