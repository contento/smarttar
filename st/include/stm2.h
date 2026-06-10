#ifndef __STM2_H
#define __STM2_H

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

#if !defined(__RECEIPT_H)
#include <receipt.h>
#endif

#if !defined(__BCLUSTER_H)
#include <bcluster.h>
#endif

#if !defined(__DS_ENTRY_H)
#include <ds_entry.h>
#endif

#if !defined(__DXS_ENTR_H)
#include <dxs_entr.h>
#endif

const WORD STM2_BANKSIZE       = 0x8000;
const WORD STM2_MAXRECEIPTS    = RT_MAXRECEIPTS*3;
const WORD STM2_EXITSTRINGSIZE = 0x10;

//
// STM2: abstract base for the session / booth NVRAM store.  All the
// generic record logic (login, get/put, prepare, the receipt circular
// queue, ...) lives here and is built on the virtual read/write/check
// seam.  The concrete backend is chosen by MakeStm2() (stm2fact.h):
//   BankStm2 (real_dos)  -- battery-backed NVRAM via port I/O
//   NullStm2 (demo_dos)  -- RAM-backed, no hardware (mini-smarttar demo)
// Callers hold an STM2* (g_STM2) and never see the concrete.  Do not
// instantiate STM2 directly -- it is abstract; use MakeStm2().
//
class STM2
{
public:
    STM2 (void) {}
    virtual ~STM2(void) {}

	void GetDumpData(WORD offset, void *buffer, WORD bufSize)
	{
		dump(offset, buffer, bufSize);
	}

	void SetDumpData(WORD offset, void *buffer, WORD bufSize)
	{
		replace(offset, buffer, bufSize);
	}

	enum STATUS { NONE, OK, BAD_SHUTDOWN, GARBAGE };
	//
	WORD login    (void);
	void logout   (void);
	WORD getStatus(void)
	{
		return status;
	}
	void dump     (WORD offset, void *buffer, WORD bufSize);
	void replace  (WORD offset, void *buffer, WORD bufSize);

    void forceOk       (void);
    void emptyReceipts (void);
    //
    BOOL get(WORD id,       void *buffer);
    BOOL put(WORD id, const void *buffer);
    //
	enum Id
	{
		EXITSTRING,
		SERIAL,
		DATE,
		TIME,
		LOGINDATE,
		LOGINTIME,
		LOGOUTDATE,
		LOGOUTTIME,
		RECEIPTNUMBER,
		EXTENSIONRECEIPTNUMBER,
		TRIES,
		//
		STATISTICSENTRIES,
		STATISTICSDOUBLEPRNENTRIES,
		STATISTICSCELLULARENTRIES,
		//
		EXTENSIONCRITICALSTATISTICS,
		//
		BOOTHCLUSTERS,
		RECEIPTSCOUNT,
		RECEIPTSFRONT,
		RECEIPTSREAR,
		RECEIPTS
	};

protected:
	enum {INPUT, OUTPUT};
	void setSerial(void);
	void setExit  (const char *str);
	BOOL prepare  (WORD id, WORD& offset, WORD& bufSize, WORD direction);
	//
	// Hardware seam -- implemented by the concrete backend.  check() runs
	// from the derived ctor (NOT here) so the right override is in place.
	virtual void check(void)                                          = 0;
	virtual WORD write(WORD offset, const void *buffer, WORD bufSize) = 0;
	virtual WORD read (WORD offset,       void *buffer, WORD bufSize) = 0;
	//
	WORD banks;
	WORD status;
	//
	WORD receiptsFront;
	WORD receiptsRear;
	WORD receiptsCount;

public:
	struct Data
	{ // for mapping purposes !
		char exitString[STM2_EXITSTRINGSIZE];

		struct
		{
			SERIAL_NUMBER  serial;
			WORD           date;
			WORD           time;
			WORD           loginDate;  // log v.219
			WORD           loginTime;
			WORD           logoutDate;
			WORD           logoutTime;
			RECEIPT_NUMBER receiptNumber;
			WORD           extensionReceiptNumber;
			WORD           tries;
		} sysInfo;

		struct
		{
			DS_ENTRY          entries[DS_MAXENTRIES];
			DS_DOUBLEPRNENTRY doublePRNEntries[DS_MAXDOUBLEPRNENTRIES];
			DS_CELLULARENTRY  cellularEntries[DS_MAXCELLULARENTRIES];
		} statistics;

		BoothCluster boothClusters[MAX_CLUSTER];

		struct
		{
			WORD    count;
			WORD    front;
			WORD    rear;
			Receipt data[STM2_MAXRECEIPTS]; // data for the circular queue
		} receipts;

		struct
		{
			DXS_CRITICAL_ENTRY critical; // only for critical info
		} extensionStatistics;
	};
};

#if (sizeof(STM2::Data) > STM2_BANKSIZE)
#error "STM2 overflow."
#endif

#endif // __STM2_H
