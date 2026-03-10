#ifndef __DS_ENTRY_H
#define __DS_ENTRY_H

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

#if !defined(__RECEIPT_H)
#include <receipt.h>
#endif

struct DS_ENTRY
{
    DS_ENTRY(void)
    {
        Init();
    }

	struct RANGETAG
	{
		UINT Time;
		UINT Date;
		long Number;
	};

	struct ITEM
	{
		WORD Receipts;
		union
		{
			double TalkMin;
			double Minutes; // 2.30 internet
			double Amount;
			// if Cards exceeds the size of this it will overwrite DummyCards
			WORD   Cards[sizeof(double)/sizeof(WORD)]; // !!!
		};
		union {
			double PaidMin;
			WORD   DummyCards[sizeof(double)/sizeof(WORD)]; // !!!
		};
		double Value;  // include Tax
		double Tax;    // just Tax, not necesarilly equals to Value*Tax_PERCENT/100 !!!
		// an interesting use of overloadding operators
		ITEM& operator +=(const ITEM& item)
		{
			Receipts += item.Receipts;
			TalkMin  += item.TalkMin;
			PaidMin  += item.PaidMin;
			Value    += item.Value;
			Tax      += item.Tax;
			return *this;
		}
		// special case for CARD
		ITEM& Add(const ITEM& item)
#pragma warn -inl
		{
			Receipts += item.Receipts;
			// special case for CARD
			for (int i=0; i< MAX_MAGNETIC_CARDS; i++)
				Cards[i] += item.Cards[i];
			//
			PaidMin  += item.PaidMin;
			Value    += item.Value;
			Tax      += item.Tax;
			return *this;
		}
	};
	// ---
	RANGETAG From;
	RANGETAG To;
	// ---
	struct
	{
		ITEM Nal;
#if defined(__EDA__)
		ITEM EDA2EDA;
		ITEM EDA2EPM;
		ITEM EDA2TEL;
#endif
		ITEM Inter; // it's the same InterEDA2TEL
	}  Tel;
	// ---
	struct
	{
		ITEM Nal;
#if defined(__EDA__)
		ITEM EDA2EDA;
		ITEM EDA2EPM;
		ITEM EDA2TEL;
#endif
		ITEM Inter; // it's the same InterEDA2TEL
	} SpecialTel;
	// ---
	struct
	{
		ITEM Nal;
#if defined(__EDA__)
		ITEM EDA2EDA;
		ITEM EDA2EPM;
		ITEM EDA2TEL;
#endif
		ITEM Inter; // it's the same InterEDA2TEL
	} Fax;
	// ---
	struct
	{
		ITEM Nal;
#if defined(__EDA__)
		ITEM EDA2EDA;
		ITEM EDA2EPM;
		ITEM EDA2TEL;
#endif
		ITEM NA_Inter; // it's the same InterEDA2TEL
	} Internet;
	// ---
	ITEM Cards;
	// ---
	ITEM Other;
	//
	// --- Totals ---
	//
	WORD DialErrors;
	WORD ComErrors;
	// ---
	ITEM NotPaid;  // (NC)
	ITEM TollFree; // (PR)
	struct
	{
		double Tel;     // normal calls
		double Special; // special tel + telex + fax  + mcards + other
#if defined(__EDA__)
		double EDA2EDA;
		double EDA2EPM;
		double EDA2TEL;
#endif
		double NotPaid; // (NC)+(PR)
		double General;
	} Total; // include Tax
	struct
	{
		double Tel;
		double Special;
#if defined(__EDA__)
		double EDA2EDA;
		double EDA2EPM;
		double EDA2TEL;
#endif
		double General;
	} Tax; // just Tax, not necesarily the Total*Tax_PERCENT/100 !!!
	//
	void Init(void)
	{
		memset(this, 0, sizeof(*this));
	}
	//
	// an interesting use of overloadding operators
	//
	DS_ENTRY& operator +=(const DS_ENTRY& entry)
	{
		if (!From.Number)
		{
			From.Date   = entry.From.Date;
			From.Time   = entry.From.Time;
			From.Number = entry.From.Number;
		}
		if (entry.To.Number)
		{
			To.Date   = entry.To.Date;
			To.Time   = entry.To.Time;
			To.Number = entry.To.Number;
		}
		//
		// --- TEL
		//
		Tel.Nal += entry.Tel.Nal;
#if defined(__EDA__)
		Tel.EDA2EDA += entry.Tel.EDA2EDA;
		Tel.EDA2EPM += entry.Tel.EDA2EPM;
		Tel.EDA2TEL += entry.Tel.EDA2TEL;
		// remember inter is the same for both !!!
#endif
		Tel.Inter += entry.Tel.Inter;
		//
		// --- SPECIAL_TEL
		//
		SpecialTel.Nal += entry.SpecialTel.Nal;
#if defined(__EDA__)
		SpecialTel.EDA2EDA += entry.SpecialTel.EDA2EDA;
		SpecialTel.EDA2EPM += entry.SpecialTel.EDA2EPM;
		SpecialTel.EDA2TEL += entry.SpecialTel.EDA2TEL;
#endif
		SpecialTel.Inter += entry.SpecialTel.Inter;
		//
		// --- FAX
		//
		Fax.Nal += entry.Fax.Nal;
#if defined(__EDA__)
		Fax.EDA2EDA += entry.Fax.EDA2EDA;
		Fax.EDA2EPM += entry.Fax.EDA2EPM;
		Fax.EDA2TEL += entry.Fax.EDA2TEL;
#endif
		Fax.Inter += entry.Fax.Inter;
		//
		// --- TELEX
		//
		Internet.Nal += entry.Internet.Nal;
#if defined(__EDA__)
		Internet.EDA2EDA += entry.Internet.EDA2EDA;
		Internet.EDA2EPM += entry.Internet.EDA2EPM;
		Internet.EDA2TEL += entry.Internet.EDA2TEL;
#endif
		//
		// --- CARD
		//
		// special case for CARD
		Cards.Add(entry.Cards);
		//
		// --- OTHER
		//
		Other += entry.Other;
		// --- Totals
		// errores
		DialErrors += entry.DialErrors;
		ComErrors  += entry.ComErrors;
		// not paid and toll free
		NotPaid  += entry.NotPaid;
		TollFree += entry.TollFree;
		// total
		Total.Tel     += entry.Total.Tel;
		Total.Special += entry.Total.Special;
#if defined(__EDA__)
		Total.EDA2EDA += entry.Total.EDA2EDA;
		Total.EDA2EPM += entry.Total.EDA2EPM;
		Total.EDA2TEL += entry.Total.EDA2TEL;
#endif
		Total.NotPaid += entry.Total.NotPaid;
		Total.General += entry.Total.General;
		// Tax
		Tax.Tel     += entry.Tax.Tel;
		Tax.Special += entry.Tax.Special;
#if defined(__EDA__)
		Tax.EDA2EDA += entry.Tax.EDA2EDA;
		Tax.EDA2EPM += entry.Tax.EDA2EPM;
		Tax.EDA2TEL += entry.Tax.EDA2TEL;
#endif
		Tax.General += entry.Tax.General;

		return *this;
	}
};

// cellular entries appeared by may 1996 but to keep compatibility
// we have to put at the end of file and not as part of the entry.
struct DS_CELLULARENTRY
{
	DS_CELLULARENTRY(void)
	{
		Init();
	}
	struct ITEM
	{
		WORD   Receipts;
		double TalkMin;
		double PaidMin;
		double Value;  // include Tax
		double Tax;    // just Tax, not necesarilly equals to Value*Tax_PERCENT/100 !!!
		// an interesting use of overloadding operators
		ITEM& operator +=(const ITEM& item)
		{
			Receipts += item.Receipts;
			TalkMin  += item.TalkMin;
			PaidMin  += item.PaidMin;
			Value    += item.Value;
			Tax      += item.Tax;
			return *this;
		}
	};
	ITEM Tel;
	ITEM SpecialTel;
	void Init(void)
	{
		memset(this, 0, sizeof(*this));
	}
	DS_CELLULARENTRY& operator +=(const DS_CELLULARENTRY& entry)
	{
		//
		// in the beginning we just use Total
		//
		Tel.Receipts += entry.Tel.Receipts;
		Tel.TalkMin  += entry.Tel.TalkMin;
		Tel.PaidMin  += entry.Tel.PaidMin;
		Tel.Value    += entry.Tel.Value;
		Tel.Tax      += entry.Tel.Tax;
		//
		SpecialTel.Receipts += entry.SpecialTel.Receipts;
		SpecialTel.TalkMin  += entry.SpecialTel.TalkMin;
		SpecialTel.PaidMin  += entry.SpecialTel.PaidMin;
		SpecialTel.Value    += entry.SpecialTel.Value;
		SpecialTel.Tax      += entry.SpecialTel.Tax;

		return *this;
	}
};
// this special entry is useful for double prn (IBAGUE)
struct DS_DOUBLEPRNENTRY
{
	DS_DOUBLEPRNENTRY(void)
	{
		Init();
	}
	//
	WORD   Receipts;
	double Value;
	// for future
	double notused1;
	double notused2;
	//
	void Init(void)
	{
		memset(this, 0, sizeof(*this));
	}
	DS_DOUBLEPRNENTRY& operator +=(const Receipt& receipt)
	{
		this->Receipts++;
		if (receipt.Stat.Paid == PAID_CALL)
			this->Value += receipt.Value;
		return *this;
	}
	DS_DOUBLEPRNENTRY& operator -=(const Receipt& receipt)
	{
		this->Receipts--;
		if (receipt.Stat.Paid == PAID_CALL)
			this->Value -= receipt.Value;
		return *this;
	}
};

#endif // __DS_ENTRY_H
