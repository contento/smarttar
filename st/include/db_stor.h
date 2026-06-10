#ifndef __DB_STOR_H
#define __DB_STOR_H

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

#if !defined(__RECEIPT_H)
#include <receipt.h>
#endif

// ----------------------------------------------------------------------------
// [ DB_STORAGE_BACKEND ]
//
// Abstract base class for receipt-storage backends.  A backend stores,
// retrieves, and enumerates Receipt records.  Two concrete implementations
// exist: BinStorage (the legacy binary RX.DAT/RX.IDX format) and CsvStorage
// (human-readable CSV).  See MINI_SMARTTAR_PLAN § 2.1.
// ----------------------------------------------------------------------------

class DB_STORAGE_BACKEND
{
public:
    // Callback for EnumReceipts — called for each receipt in iteration.
    // Return TRUE to continue, FALSE to stop early.
    typedef BOOL (*CallbackFnPtr)(Receipt const &receipt);

    virtual ~DB_STORAGE_BACKEND() {}

    // --- Core CRUD ----------------------------------------------------------

    virtual BOOL Get(Receipt& receipt, long number, int boothNumber = -1) = 0;
    virtual BOOL Add(const Receipt& receipt) = 0;
    virtual BOOL Update(const Receipt& receipt) = 0;
    virtual BOOL Delete(long number, int boothNumber = -1) = 0;
    virtual void EnumReceipts(CallbackFnPtr callback) = 0;

    // --- Metadata -----------------------------------------------------------

    virtual long GetEntries()      const = 0;
    virtual long GetLowerNumber()  const = 0;
    virtual long GetHigherNumber() const = 0;
    virtual long GetFirstNumber()  const = 0;
    virtual long GetLastNumber()   const = 0;
    virtual BOOL Exist(long number, int boothNumber = -1) const = 0;

    // --- Lifecycle ----------------------------------------------------------

    virtual void Flush()   = 0;
    virtual BOOL Archive() = 0;
    virtual BOOL Repair()  = 0;

    // --- Status -------------------------------------------------------------

    virtual int  GetStatus()   = 0;
    virtual BOOL IsReadOnly()  = 0;

    // Receipt-number ceiling shared by all backends (logical wrap point).
    static const long MAX_RECEIPTS;
};

#endif // __DB_STOR_H
