#ifndef __DXS_ENTR_H
#define __DXS_ENTR_H

#if !defined(__RECEIPT_H)
#include <receipt.h>
#endif

struct DXS_NON_CRITICAL_ENTRY
{
    void Init(void)
    {
        memset(this, 0, sizeof(*this));
    }
    struct ENTRY
    {
        char   Text[0x15];
        WORD   Time;
        WORD   Date;
        double Value;
    };
    ENTRY Credits[3];
    ENTRY Debits[3];
    ENTRY Others[3];
};

struct DXS_CRITICAL_ENTRY
{
    struct CALL_ENTRY
    {
        void Init(void)
        {
            memset(this, 0, sizeof(*this));
        }
		CALL_ENTRY& operator +=(Receipt& receipt)
        {
            this->NumOfCalls++;
            this->Cost += receipt.Value;
            this->Tax  += receipt.Tax;
            return *this;
        }
		CALL_ENTRY& operator -=(Receipt& receipt)
        {
            this->NumOfCalls--;
            this->Cost -= receipt.Value;
            this->Tax  -= receipt.Tax;
            return *this;
        }
        CALL_ENTRY& operator +=(CALL_ENTRY& entry)
        {
            this->NumOfCalls += entry.NumOfCalls;
            this->Cost += entry.Cost;
            this->Tax  += entry.Tax;
            return *this;
        }
        CALL_ENTRY& operator -=(CALL_ENTRY& entry)
        {
            this->NumOfCalls -= entry.NumOfCalls;
            this->Cost -= entry.Cost;
            this->Tax  -= entry.Tax;
            return *this;
        }
        //
        WORD   NumOfCalls;
        double Cost;
        double Tax;
    };

    struct ONLINE_ENTRY
    {
        void Init(void)
        {
            FirstReceipt = 0;
            DDN.Init();
            DDI.Init();
        }
		ONLINE_ENTRY& operator +=(Receipt& receipt)
        {
            if (!this->FirstReceipt)
                this->FirstReceipt = receipt.Number;
            if (receipt.Stat.CallAttr & INTERNATIONAL_CALL_MASK)
                DDI += receipt;
            else
                DDN += receipt;
            return *this;
        }

		ONLINE_ENTRY& operator -=(Receipt& receipt)
        {
            if (receipt.Stat.CallAttr & INTERNATIONAL_CALL_MASK)
                DDI -= receipt;
            else
                DDN -= receipt;
            return *this;
        }
        //
        DWORD      FirstReceipt;
        CALL_ENTRY DDN;
        CALL_ENTRY DDI;
    };

    struct STORED_ENTRY
    { // statistics from inactived extensions.
        void Init(void)
        {
            memset(this, 0, sizeof(*this));
        }
        CALL_ENTRY DDN;
        CALL_ENTRY DDI;
        double     Install;
        double     Line;
        double     Credits;
        double     Debits;
        double     Others;
    };
    void Init(void)
    {
        memset(this, 0, sizeof(*this));
    }
    ONLINE_ENTRY Online[MAX_BOOTH];
    STORED_ENTRY Stored;
};

#endif // __DXS_ENTR_H