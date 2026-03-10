#ifndef __RECEIPT_H
#define __RECEIPT_H

#if !defined(__STDLIB_H)
#include <stdlib.h>
#endif

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

// --- Receipt --------------------------------------------------------------
//
// Generic receipt.
// I try to capture all the important abstractions of normal and special receipts.
// GCC/gcc.
//

class Receipt
{
public:
	Receipt()
		:
		MagicNumber(0),
		Number(0L),
		Tag(TEL),
		nStat(0),
		bExtendedStat(FALSE),
		Date(0),
		Time(0),
		BoothNumber(0),
		Amount(0),
		ElapsedTime(0L),
		ValuePerMin(0.0),
		CeilMin(0.0),
		Percent(0),
		Value(0.0),
		Tax(0.0),
		Tax2(0.0),
		DDummy(0.0)
	{
		memset(City, 0, sizeof(CITY_NAME));
		memset(Phone, 0, sizeof(PHONE));
	}
	//
	// --- Data members ---
	//
	// magic number to know if the record is valid
	UINT  MagicNumber;
	// common data
	long  Number;
	//
	enum TTag
	{
		TEL, SPECIAL_TEL, TELEX, FAX, CARD, OTHER
	} Tag;

	struct STAT
	{
		BOOL Cooked   : 1; // the engine put the raw receipt, the controller cook
		BOOL Manual   : 1;
		BOOL Printed  : 1;
		BOOL Archived : 1;
		UINT Paid     : 4; //
		UINT CallAttr : 6; // NOT_INCLUDED etc
		UINT Extension: 1; // Booth/Extension
		UINT Deleted  : 1;
	};
	union
	{
		STAT Stat; // estado del recibo
		UINT  nStat;
	};

	struct EXTENDED_STAT
	{ // for future uses
		UCHAR nonProcessed : 1; // only for manual mode during recover
	};
	union
	{
		EXTENDED_STAT extendedStat;
		UCHAR     	  bExtendedStat; //
	};

	int Date; // packed date // fecha de la llamada
	int Time; // packed time // hora de la llamada
	// --- Non common members
	union
	{
		int BoothNumber; // numero de cabina (not alias)
	};

	union
	{
		CITY_NAME City;
		CITY_NAME Motif;
	};

	union
	{
		PHONE 	Phone;             // Numero telefonico
		UINT  	Cards[MAX_MAGNETIC_CARDS]; // cantidad de cada una de las tarjetas
		double  Minutes; // Internet 2.30
	};

	union
	{
		int Amount;
		int Tariff;
	};
	union
	{
		long ElapsedTime;
	};
	union
	{
		double ValuePerMin;  // valor por minuto
		double UnitaryValue; // valor unitario
	};
	union
	{
		double CeilMin; // tiempo ajustado para las llamadas
		double Base;	  // cargo fijo en telex
	};
	union
	{
		int Percent; // porcentaje de cobro de la llamada
	};
	// --- Other Common values
	double Value; // to avoid re-calc
	double Tax;   // just Tax, not necesarily the Value*TAX_PERCENT/100 !!!
	// Version 2.30 introduces Tax2
	// IDummy1(0),
	// IDummy2(0),
	// LDummy(0L),
	double Tax2;   // just Tax, not necesarily the Value*TAX_PERCENT/100 !!!
	// for future uses, don't change position but the identifier !!!
	double DDummy;
public:
	//
	// for small arrays (sorting and searching)
	//
	static int Compare(const void *left, const void *right)
	{
		return (((Receipt *)left)->Number == ((Receipt *)right)->Number)?0:(((Receipt *)left)->Number < ((Receipt *)right)->Number)?-1:1;
	}
	static void QSort(Receipt *items, size_t numOfItems)
	{
		qsort(items, numOfItems, sizeof(Receipt), Compare);
	}
	static Receipt *BSearch(long key, Receipt *items, size_t numOfItems)
	{
		Receipt dummy;
		dummy.Number = key;
		return (Receipt *)bsearch(&dummy, items, numOfItems, sizeof(Receipt), Compare);
	}
};

#if sizeof(Receipt) != 111
#error "Receipt has changed its original size of 111 bytes."
#endif

#endif // __RECEIPT_H
