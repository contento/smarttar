#include <stdlib.h>
#include <iostream.h>

typedef unsigned char   BYTE;
typedef          short  SHORT;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;
typedef int             BOOL;
typedef unsigned int  	UINT;
typedef unsigned char   UCHAR;
typedef          long   LONG;
typedef unsigned long   ULONG;
#define  FALSE        0
#define  TRUE         1

typedef char STR16 [16];
typedef char STR32 [32];
typedef char STR64 [64];
typedef char STR128[128];
typedef char STR256[256];
typedef char STR512[512];

typedef char COMPANY_NAME[31];
typedef char CALL_HEADER[5];        // access codes + first next digit.
typedef char CALL_ACCESS_HEADER[4]; // only access codes.
typedef WORD CALL_AREA_CODE;        // the digit after access codes
typedef WORD CALL_ATTR;
typedef char CITY_NAME[21];
typedef char PHONE[17];
typedef long PHONE_NUMBER;
typedef char FILE_NAME[80];
typedef char TEXT_FILE_LINE[512];
typedef char TITLE[0x20];
typedef char SERIAL_NUMBER[0x20];
typedef char KEY[0x20];
typedef DWORD RECEIPT_NUMBER;

const int MAX_HOLLYDAYS   = 4;
const int MAX_HOLLY_YEARS = 3;

const UINT CLUSTER_SIZE       = 8; // each cluster made up by 8 booths
const UINT MAX_CLUSTER        = 4; // max number of system cluster
const UINT MAX_BOOTH          = MAX_CLUSTER*CLUSTER_SIZE;
const UINT MAX_MAGNETIC_CARDS = 4;

struct RECEIPT_FOO
{
	//
	// --- Data members ---
	//
	// magic number to know if the record is valid
	UINT  MagicNumber;
	// common data
	long  Number;
	//
	enum TTag {
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
	}
	Stat; // estado del recibo
};

struct RECEIPT
{
	RECEIPT& CopyBase(const RECEIPT& receipt)
	{
		return *this = receipt;
	}
	RECEIPT& GetBase (void)
	{
		return *this;
    }
    //
    // --- Data members ---
    //
    // magic number to know if the record is valid
    UINT  MagicNumber;
    // common data
    long  Number;
    //
    enum TTag {
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
    }
    Stat; // estado del recibo
    struct EXTENDED_STAT
    { // for future uses
		BOOL nonProcessed : 1; // only for manual mode during recover
    }

	extendedStat;
    int Date; // packed date // fecha de la llamada
    int Time; // packed time // hora de la llamada
    // --- Non common members
    union {
    	int BoothNumber; // numero de cabina (not alias)
    };
    union {
        CITY_NAME City;
        CITY_NAME Motif;
    };
    union {
        PHONE Phone;             // Numero telefonico
        UINT  Cards[MAX_MAGNETIC_CARDS]; // cantidad de cada una de las tarjetas
    };
    union {
        int Amount;
        int Tariff;
    };
    union {
        long ElapsedTime;
    };
    union {
        double ValuePerMin;  // valor por minuto
        double UnitaryValue; // valor unitario
    };
    union {
        double CeilMin; // tiempo ajustado para las llamadas
        double Base;	  // cargo fijo en telex
    };
    union {
        int Percent; // porcentaje de cobro de la llamada
    };
    // --- Other Common values
    double Value; // to avoid re-calc
    double Tax;   // just Tax, not necesarily the Value*TAX_PERCENT/100 !!!
    // for future uses, don't change position but the identifier !!!
    int    IDummy1;
    int    IDummy2;
    long   LDummy;
    double DDummy;
    //
    // for small arrays (sorting and searching)
    //
    static int Compare(const void *left, const void *right)
    {
        return (((RECEIPT *)left)->Number == ((RECEIPT *)right)->Number)?0:(((RECEIPT *)left)->Number < ((RECEIPT *)right)->Number)?-1:1;
    }
    static void QSort(RECEIPT *items, size_t numOfItems)
    {
        qsort(items, numOfItems, sizeof(RECEIPT), Compare);
    }
    static RECEIPT *BSearch(long key, RECEIPT *items, size_t numOfItems)
    {
        RECEIPT dummy;
        dummy.Number = key;
        return (RECEIPT *)bsearch(&dummy, items, numOfItems, sizeof(RECEIPT), Compare);
    }
};

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
		IDummy1(0),
		IDummy2(0),
		LDummy(0L),
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
		BOOL nonProcessed : 1; // only for manual mode during recover
	};
	union
	{
		EXTENDED_STAT extendedStat;
		BOOL    	  bExtendedStat;
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
		PHONE Phone;             // Numero telefonico
		UINT  Cards[MAX_MAGNETIC_CARDS]; // cantidad de cada una de las tarjetas
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
	// for future uses, don't change position but the identifier !!!
	int    IDummy1;
	int    IDummy2;
	long   LDummy;
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

void main()
{
	Receipt rNew;
	RECEIPT rOld;

	cout << "New:" << endl;
	cout << sizeof(rNew) << endl;
	cout << int((char *)&rNew.bExtendedStat - (char *)&rNew) << endl;
	cout << sizeof(rNew.bExtendedStat) << endl;
	rNew.bExtendedStat = TRUE;
	cout << '\t'<< (int)(unsigned char) *(&rNew.bExtendedStat)   << endl;
	cout << '\t'<< (int)(unsigned char) *(&rNew.bExtendedStat+1) << endl;

	cout << "Old:" << endl;
	cout << sizeof(rOld) << endl;
	cout << int((char *)&rOld.extendedStat - (char *)&rOld) << endl;
	cout << sizeof(rOld.extendedStat) << endl;

	cout << "Before: " << sizeof(RECEIPT_FOO) << endl;
}
