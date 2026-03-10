#ifndef __DRECEIPT_H
#define __DRECEIPT_H

#if !defined(__RECEIPT_H)
#include <receipt.h>
#endif

// --- DynamicReceipt --------------------------------------------------------
//
// this dynamic receipt is useful to process information but not to store.
//
// I prefered to use the isA approach rather than the hasA, to avoid
// dereferencing large identifiers.
//
class DynamicReceipt
{
public:

	DynamicReceipt()
		:
		nAttr_(0),
		Area_(0),
		Total_(0.0),
		PreValue_(0.0),
		MoneyBack_(0.0),
		bFromTurn(TRUE)
	{
	}

	DynamicReceipt(Receipt const & receipt)
		:
		nAttr_(0),
		Area_(0),
		Total_(0.0),
		PreValue_(0.0),
		MoneyBack_(0.0)
	{
		this->m_r = receipt;
	}

	Receipt m_r;
	// --- non-storable attributes with suffix underscore (_)
	struct ATTR_
	{
		UINT HeaderOn  : 1;
		UINT FooterOn  : 1;
		UINT SummaryOn : 1;
		UINT Countable : 1;
		UINT Storable  : 1;
		UINT Printable : 1;
	};
	union
	{
		ATTR_ Attr_;
		UINT  nAttr_;
	};
	// cookable items
	union
	{
		WORD Area_      ; // area a la cual pertenece la llamada
		WORD NumOfCalls_; // numero de llamadas, no incluye NCs y PRs for SUMMARY
	};
	union
	{
		double Total_;   // big total for summary receipts
		double DecTime_; // time in decimal
	};
	union
	{
		double PreValue_; // valor anticipado
	};
	union
	{
		double MoneyBack_; // devuelta
	};

	BOOL bFromTurn;

public:

	//
	// for small arrays (sorting and searching)
	//
	static int Compare(const void *left, const void *right)
	{
		return (((DynamicReceipt *)left)->m_r.Number == ((DynamicReceipt *)right)->m_r.Number)?0:(((DynamicReceipt *)left)->m_r.Number < ((DynamicReceipt *)right)->m_r.Number)?-1:1;
	}
	static void QSort(DynamicReceipt *items, size_t numOfItems)
	{
		qsort(items, numOfItems, sizeof(DynamicReceipt), Compare);
	}
	static DynamicReceipt *BSearch(long key, DynamicReceipt *items, size_t numOfItems)
	{
		DynamicReceipt dummy;
		dummy.m_r.Number = key;
		return (DynamicReceipt *)bsearch(&dummy, items, numOfItems, sizeof(DynamicReceipt), Compare);
	}
};

#endif // __DRECEIPT_H