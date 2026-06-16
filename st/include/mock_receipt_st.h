#ifndef __MOCK_RECEIPT_ST_H
#define __MOCK_RECEIPT_ST_H

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

#if !defined(__RECEIPT_H)
#include <receipt.h>
#endif

#include <ireceipt.h>

#include <string.h>

static const WORD MOCK_MAX_RECEIPTS = 1000;
static const UINT MOCK_MAGIC_NUMBER = 0x6719;

struct MockReceiptStorage : public IReceiptStorage
{
    Receipt receipts[MOCK_MAX_RECEIPTS];
    WORD count;
    int status;

    MockReceiptStorage();
    virtual ~MockReceiptStorage() {}

    virtual int  GetStatus();
    virtual BOOL IsReadOnly();

    virtual BOOL IsCorrectNumber(long number);
    virtual BOOL IsValid(Receipt const & receipt);

    virtual BOOL Exist(long number, int boothNumber) const;

    virtual BOOL Get(Receipt & receipt, long number, int boothNumber = -1);
    virtual BOOL Add(const Receipt & receipt);
    virtual BOOL Update(const Receipt & receipt);
    virtual BOOL Delete(long number, int boothNumber = -1);

    virtual long GetLowerNumber() const;
    virtual long GetHigherNumber() const;
    virtual long GetEntries() const;
    virtual long GetFirstNumber() const;
    virtual long GetLastNumber() const;

    virtual void EnumReceipts(CallbackFnPtr callback);
    virtual void Flush();

    virtual BOOL Archive();
    virtual BOOL Repair();
};

inline MockReceiptStorage::MockReceiptStorage()
    : count(0), status(0)
{
    memset(receipts, 0, sizeof(receipts));
}

inline int MockReceiptStorage::GetStatus()
{
    return status;
}

inline BOOL MockReceiptStorage::IsReadOnly()
{
    return FALSE;
}

inline BOOL MockReceiptStorage::IsCorrectNumber(long number)
{
    return number > 0L;
}

inline BOOL MockReceiptStorage::IsValid(Receipt const & receipt)
{
    return receipt.MagicNumber == MOCK_MAGIC_NUMBER;
}

inline BOOL MockReceiptStorage::Exist(long number, int boothNumber) const
{
    for (WORD i = 0; i < count; i++)
    {
        if (receipts[i].Number == number &&
            (boothNumber == -1 || receipts[i].BoothNumber == boothNumber))
        {
            return TRUE;
        }
    }
    return FALSE;
}

inline BOOL MockReceiptStorage::Get(Receipt & receipt, long number, int boothNumber)
{
    for (WORD i = 0; i < count; i++)
    {
        if (receipts[i].Number == number &&
            (boothNumber == -1 || receipts[i].BoothNumber == boothNumber))
        {
            receipt = receipts[i];
            return TRUE;
        }
    }
    return FALSE;
}

inline BOOL MockReceiptStorage::Add(const Receipt & receipt)
{
    if (count >= MOCK_MAX_RECEIPTS)
        return FALSE;
    receipts[count++] = receipt;
    return TRUE;
}

inline BOOL MockReceiptStorage::Update(const Receipt & receipt)
{
    for (WORD i = 0; i < count; i++)
    {
        if (receipts[i].Number == receipt.Number &&
            receipts[i].BoothNumber == receipt.BoothNumber)
        {
            receipts[i] = receipt;
            return TRUE;
        }
    }
    return FALSE;
}

inline BOOL MockReceiptStorage::Delete(long number, int boothNumber)
{
    for (WORD i = 0; i < count; i++)
    {
        if (receipts[i].Number == number &&
            (boothNumber == -1 || receipts[i].BoothNumber == boothNumber))
        {
            receipts[i].Stat.Deleted = TRUE;
            return TRUE;
        }
    }
    return FALSE;
}

inline long MockReceiptStorage::GetLowerNumber() const
{
    long lower = 0L;
    for (WORD i = 0; i < count; i++)
    {
        if (!receipts[i].Stat.Deleted)
        {
            if (lower == 0L || receipts[i].Number < lower)
                lower = receipts[i].Number;
        }
    }
    return lower;
}

inline long MockReceiptStorage::GetHigherNumber() const
{
    long higher = 0L;
    for (WORD i = 0; i < count; i++)
    {
        if (!receipts[i].Stat.Deleted && receipts[i].Number > higher)
            higher = receipts[i].Number;
    }
    return higher;
}

inline long MockReceiptStorage::GetEntries() const
{
    long entries = 0L;
    for (WORD i = 0; i < count; i++)
    {
        if (!receipts[i].Stat.Deleted)
            entries++;
    }
    return entries;
}

inline long MockReceiptStorage::GetFirstNumber() const
{
    return GetLowerNumber();
}

inline long MockReceiptStorage::GetLastNumber() const
{
    return GetHigherNumber();
}

inline void MockReceiptStorage::EnumReceipts(CallbackFnPtr callback)
{
    for (WORD i = 0; i < count; i++)
    {
        if (!receipts[i].Stat.Deleted)
        {
            if (!callback(receipts[i]))
                return;
        }
    }
}

inline void MockReceiptStorage::Flush()
{
}

inline BOOL MockReceiptStorage::Archive()
{
    return TRUE;
}

inline BOOL MockReceiptStorage::Repair()
{
    return TRUE;
}

#endif // __MOCK_RECEIPT_ST_H
