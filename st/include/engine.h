#ifndef __ENGINE_H
#define __ENGINE_H

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

#if !defined(__BCLUSTER_H)
#include <bcluster.h>
#endif

#if !defined(__DRECEIPT_H)
#include <dreceipt.h>
#endif

#ifdef DOSX286
#include <phapi.h>
#endif

#if !defined(__CQUEUE_H)
#include <cqueue.h>
#endif

/*
	Information about the app ports:
	--- first cluster of booths (cluster made up by 8 booths)
	0x280: Dialing (8 booths)
	0x281: Answering (8 booths)
	0x282: DTMF Flags (8 booths)
	0x283: 2 DTMF booths (4 bits each one)
	0x284: 2 DTMF booths (4 bits each one)
	0x285: 2 DTMF booths (4 bits each one)
	0x286: 2 DTMF booths (4 bits each one)
	0x287: Lock (8 booths) 1 active relay, relay normally close
	0x288: Spy (8 booths) 1 active relay, relay normally close
	0x289: General Port
			 75643210
			 00000001 Linea entrante
			 00000010 Pito de internacional
			 00000100 Cajero
	0x28A: C thread
*/

#define APP_PORT_BASE     0x0280
// PO stands for Port Offset
#define PO_OOD            0x00
#define PO_ANSWER         0x01
#define PO_DTMF_FLAGS     0x02
#define PO_DTMF_DIGITS    0x03 // 4 ports
#define PO_LOCK	          0x07
#define PO_SPY	          0x08
#define PO_GENERAL        0x09
#define PO_C_THREAD 	  	0x0A
// masks for general port
#define GP_INLINE         0x01
#define GP_BEEP           0x02
#define GP_CASH           0x04

// number of ports used by each cluster of booths
#define PORTS_BY_CLUSTER  0x10

//
// ENGINE: abstract base for the booth-cluster engine.
//
// Template Method pattern. Shared state (Clusters[], Receipts queue,
// timers, accessors, FSM dispatch) lives here. Three virtual hooks
// carry the per-subclass variation:
//
//   InitHardware() -- ctor-time setup (probe ports, allocate Clusters,
//                     initialize booth state). RT probes real hardware;
//                     DEMO trusts config and stubs.
//   RecoverState() -- ctor-time persistent-state restore. RT loads from
//                     STM2; DEMO no-ops.
//   OnTimerTick()  -- per-cluster work on every IRQ0 tick. RT reads
//                     hardware ports into the DataPort fields; DEMO
//                     synthesizes them. This is the seam that lets the
//                     rest of the system stay oblivious to which engine
//                     is behind the pointer.
//
// The static ISR handlers (NewISR08h etc.) and the pThis dispatch live
// on the base; vectors can't be virtual but the per-tick body can be.
//
class ENGINE
{

public:

	ENGINE(WORD numOfClusters=1);
	virtual ~ENGINE(void);

	// --- Variation hooks (Template Method) ----------------------------
	// Called by the derived ctor AFTER ENGINE() finishes; pure virtual
	// dispatch from inside a base ctor would resolve to base, so the
	// derived ctor invokes these explicitly.
	virtual void InitHardware(WORD numOfClusters) = 0;
	virtual void RecoverState(void) = 0;
	// Called from NewISR08h once per active cluster. RT reads the
	// hardware ports into dp; DEMO synthesizes the same fields. This
	// is the seam that lets downstream code stay oblivious to the
	// engine concrete behind the pointer.
	virtual void OnTimerTick(WORD cNum, BoothCluster::_DataPort & dataPort) = 0;
	// Called from NewISR08h once per tick AFTER all clusters have
	// been polled. RT writes the general-port output (relay etc.);
	// DEMO no-ops.
	virtual void OnTimerEnd(void) = 0;

	void GetDumpData(void * & ptr, int &size);
	void SetDumpData(void *   ptr, int  size);


	enum
	{
		PULSEPHONE, TONEPHONE
	};
	enum
	{
		LOCK,
		ONHOOK,
		RINGUP,
		RINGDOWN,
		INCOMETALK,
		OFFHOOK,
		BREAK, MAKE,
		INTERDIG,
		DTMFFLAG,
		ANSWER,
		TALK,
		STORE,
		SPY,
		NAL, INTER,
		DIALERR, COMERR,
		NOPHONE,
		END,
		// special cases after END !!!
		SIMULA
	};

#if defined (__TEST__)
	void Show(void);       // to test !!! in demos
#endif

protected:

#if !defined (__TEST__)
	CIRCULAR_QUEUE<DynamicReceipt> *Receipts;
#endif

	BoothCluster *Clusters;
	BYTE GeneralPort;

public:

	BOOL IsAvailable(WORD cNum) const
	{
		return Clusters[cNum].Available;
	}

	void GetPhone(WORD cNum, WORD bNum, PHONE & phone) const;

	BYTE GetGeneralPort() const;
	void SetGeneralPort(BYTE generalPort);

	BYTE GetDataPortSpy(WORD cNum) const;
	void SetDataPortSpy(WORD cNum, BYTE value);

protected:
	BoothCluster::_DataPort & GetDataPort(WORD cNum);

	void ResetPhone(WORD cNum, WORD bNum);

	void StoreCurrentDigit(WORD cNum, WORD bNum);
	BYTE GetCurrentDigit(WORD cNum, WORD bNum);
	void SetCurrentDigit(WORD cNum, WORD bNum, BYTE digit);
	void ResetCurrentDigit(WORD cNum, WORD bNum);
	void IncCurrentDigit(WORD cNum, WORD bNum);

public:
	BOOL GetFound(WORD cNum, WORD bNum) const;
	void SetFound(WORD cNum, WORD bNum, BOOL value) const;

	WORD GetCallAttr(WORD cNum, WORD bNum) const;
	void SetCallAttr(WORD cNum, WORD bNum, WORD attr);

	WORD GetTariff(WORD cNum, WORD bNum) const;
	void SetTariff(WORD cNum, WORD bNum, WORD tariff);

	int  GetStartTime(WORD cNum, WORD bNum);
	void SetStartTime(WORD cNum, WORD bNum, int time);

	int  GetStartDate(WORD cNum, WORD bNum);
	void SetStartDate(WORD cNum, WORD bNum, int date);

	BOOL GetSimula(WORD cNum, WORD bNum) const;
	void SetSimula(WORD cNum, WORD bNum, BOOL value) const;
	void GetSimulaPhone(WORD cNum, WORD bNum, PHONE & phone) const;
	void SetSimulaPhone(WORD cNum, WORD bNum, PHONE const & phone);
	void ResetSimulaPhone(WORD cNum, WORD bNum);
	BOOL GetNoReceipt(WORD cNum, WORD bNum) const;
	void SetNoReceipt(WORD cNum, WORD bNum, BOOL value) const;
	BOOL GetNoStatistics(WORD cNum, WORD bNum) const;
	void SetNoStatistics(WORD cNum, WORD bNum, BOOL value) const;

	BOOL GetLocked(WORD cNum, WORD bNum) const;
	void SetLocked(WORD cNum, WORD bNum, BOOL locked);

	SHORT GetLastSpyBooth() const;
	void  SetLastSpyBooth(WORD booth);
	void  SaveLastSpyBooth();
	SHORT GetSpyBooth() const;
	void  SetSpyBooth(WORD booth);

	int  GetCurrentDate();
	void SetCurrentDate(int date);
	int  GetCurrentTime();
	void SetCurrentTime(int time);

	SHORT GetNotIncBooth() const;
	void  SetNotIncBooth(SHORT booth);
	SHORT GetBadInterBooth() const;
	void  SetBadInterBooth(SHORT booth);
	SHORT GetComErrBooth() const;
	void  SetComErrBooth(SHORT booth);
	SHORT GetDialErrBooth() const;
	void  SetDialErrBooth(SHORT booth);

	void GetClusters(BoothCluster clusters[]);

	BOOL GetReceipt(DynamicReceipt & dynReceipt);
	void ForceStoreActiveCalls(void); // 2.50 -- settle in-progress TALK calls (drain/exit)

	WORD GetNumOfCalls(WORD cNum, WORD bNum) const;
	void SetNumOfCalls(WORD cNum, WORD bNum, WORD numOfCalls);

	BOOL IsBusy(void);
	BOOL IsBoothBusy(WORD cNum, WORD bNum);

	// TRUE if this is the synthetic DEMO_ENGINE; overridden there.
	// Lets the Exit() dialog distinguish "real booths busy -- block quit"
	// from "demo generator running -- prompt to stop and quit".
	virtual BOOL IsDemo(void) { return FALSE; }

	// Pause/resume the synthetic generator (DEMO_ENGINE only).  No-op
	// on RT_ENGINE -- real hardware can't be "paused" via menu.  Wired
	// to UE_DEMO_TOGGLE via CONTROLLER::RTEngineToggleDemo().
	virtual void TogglePaused(void) {}
	virtual BOOL IsPaused(void) { return FALSE; }

protected:

	WORD CurrentDate; // packed date
	WORD CurrentTime; // packed time

	SHORT SpyBooth;    // keep track
	SHORT LastSpyBooth;

	// error signals
	SHORT NotIncBooth;		// not included phone (booth)
	SHORT BadInterBooth;    // bad international call (booth)
	SHORT ComErrBooth;	  	// comunication error (booth)
	SHORT DialErrBooth;     // dial error (booth)

	void EvalPulseState(WORD cNum, WORD bNum);
	void EvalToneState (WORD cNum, WORD bNum);

	void ResetData(WORD cNum, WORD bNum);

	void DoLock      (WORD cNum, WORD bNum);
	void DoRingUp    (WORD cNum, WORD bNum);
	void DoRingDown  (WORD cNum, WORD bNum);
	void DoIncomeTalk(WORD cNum, WORD bNum);
	void DoOnHook    (WORD cNum, WORD bNum);
	void DoOffHook   (WORD cNum, WORD bNum, WORD phoneType);
	void DoDTMFFlag  (WORD cNum, WORD bNum);
	void DoBreak     (WORD cNum, WORD bNum);
	void DoMake      (WORD cNum, WORD bNum);
	void DoInterdig  (WORD cNum, WORD bNum, WORD phoneType);
	void DoAnswer    (WORD cNum, WORD bNum, WORD phoneType);
	void DoTalk      (WORD cNum, WORD bNum, WORD phoneType);
	void DoStore     (WORD cNum, WORD bNum);
	void DoDialErr   (WORD cNum, WORD bNum);
	void DoComErr    (WORD cNum, WORD bNum);

	BOOL IsLockable(WORD cNum, WORD bNum);
	BOOL MaxNumOfDigits(WORD cNum, WORD bNum);

	BOOL IsAnswerable(WORD cNum, WORD bNum);
	void StoreReceipt(WORD cNum, WORD bNum);
	void CheckAnswerSignal(WORD cNum, WORD bNum, WORD phoneType);

	BOOL IsFalseOne(WORD cNum, WORD bNum);

public: // 2.30 build 10

	WORD GetStateCount(WORD cNum, WORD bNum);
	void SetStateCount(WORD cNum, WORD bNum, WORD value);
	void ResetStateCount(WORD cNum, WORD bNum);
	void IncStateCount(WORD cNum, WORD bNum);
	void DecStateCount(WORD cNum, WORD bNum);

protected:

	WORD GetOnHookCount(WORD cNum, WORD bNum);
	void ResetOnHookCount(WORD cNum, WORD bNum);
	void IncOnHookCount(WORD cNum, WORD bNum);
	void DecOnHookCount(WORD cNum, WORD bNum);

	BOOL IsIncomeCall(WORD cNum, WORD bNum);
	void SetIncomeCall(WORD cNum, WORD bNum);
	void ResetIncomeCall(WORD cNum, WORD bNum);

	WORD GetDialCount(WORD cNum, WORD bNum);
	void ResetDialCount(WORD cNum, WORD bNum);
	void IncDialCount(WORD cNum, WORD bNum);
	void DecDialCount(WORD cNum, WORD bNum);

	WORD GetBiasCount(WORD cNum, WORD bNum);
	void ResetBiasCount(WORD cNum, WORD bNum);
	void IncBiasCount(WORD cNum, WORD bNum);
	void DecBiasCount(WORD cNum, WORD bNum);

public: // 2.30 build 10

	void  SetElapsedCount(WORD cNum, WORD bNum, DWORD value);
	DWORD GetElapsedCount(WORD cNum, WORD bNum);
	void  ResetElapsedCount(WORD cNum, WORD bNum);
	void  IncElapsedCount(WORD cNum, WORD bNum);
	void  DecElapsedCount(WORD cNum, WORD bNum);

	DWORD GetFinalElapsedCount(WORD cNum, WORD bNum);
	void  SetFinalElapsedCount(WORD cNum, WORD bNum, DWORD value);
	void  ResetFinalElapsedCount(WORD cNum, WORD bNum);

	BOOL   GetPrePaid(WORD cNum, WORD bNum);
	void   SetPrePaid(WORD cNum, WORD bNum, BOOL value);

	BOOL   GetFirstPreValue(WORD cNum, WORD bNum);
	void   SetFirstPreValue(WORD cNum, WORD bNum, BOOL value);

	double GetPreValue(WORD cNum, WORD bNum);
	void   SetPreValue(WORD cNum, WORD bNum, double value);

	DWORD  GetPreTime(WORD cNum, WORD bNum);
	void   SetPreTime(WORD cNum, WORD bNum, DWORD value);

	// operators for common pulse and tone operations
public: // 2.30 build 10

	void SetFSs(WORD cNum, WORD bNum, WORD pulseState, WORD toneState);

	WORD GetPulseFS(WORD cNum, WORD bNum);
	void SetPulseFS(WORD cNum, WORD bNum, WORD state);

	WORD GetUnifiedState(WORD cNum, WORD bNum);

protected:

	BOOL IsPulseFS_E (WORD cNum, WORD bNum, WORD state);
	BOOL IsPulseFS_L (WORD cNum, WORD bNum, WORD state);
	BOOL IsPulseFS_LE(WORD cNum, WORD bNum, WORD state);
	BOOL IsPulseFS_G (WORD cNum, WORD bNum, WORD state);
	BOOL IsPulseFS_GE(WORD cNum, WORD bNum, WORD state);

public: // 2.30 build 10

	WORD GetToneFS(WORD cNum, WORD bNum);
	void SetToneFS(WORD cNum, WORD bNum, WORD state);

protected:

	BOOL IsToneFS_E (WORD cNum, WORD bNum, WORD state);
	BOOL IsToneFS_L (WORD cNum, WORD bNum, WORD state);
	BOOL IsToneFS_LE(WORD cNum, WORD bNum, WORD state);
	BOOL IsToneFS_G (WORD cNum, WORD bNum, WORD state);
	BOOL IsToneFS_GE(WORD cNum, WORD bNum, WORD state);

	// --- We changed the trigger approach !!!
	// because we needed to obtain better performance GCC/gcc see st9420.zip
	// --- items to replace TPIT and TTrigger ...
	void SetPITRate(WORD divisor = 0);
	//
	// Shared ISR install/uninstall used by both concretes' ctors/dtors.
	void InstallISRs(void);
	void UninstallISRs(void);
	//
#ifdef DOSX286
	// the new ISR to cope with the problem of the low resolution timer
	static REALPTR            OldRealIV08h;
	static PIHANDLER          OldProtIV08h;
	static void interrupt far NewISR08h(REGS_BINT);
	// the new ISR to cope with the pause key
	static REALPTR            OldRealIV09h;
	static PIHANDLER          OldProtIV09h;
	static void interrupt far NewISR09h(REGS_BINT);
	// the new ISR to cope with Ctrl-C
	static REALPTR            OldRealIV1Bh;
	static PIHANDLER          OldProtIV1Bh;
	static void interrupt far NewISR1Bh(REGS_BINT);
	// the new ISR to cope with Ctrl-break
	static REALPTR            OldRealIV23h;
	static PIHANDLER          OldProtIV23h;
	static void interrupt far NewISR23h(REGS_BINT);
	// the new ISR to cope with Critical Errors
	static REALPTR            OldRealIV24h;
	static PIHANDLER          OldProtIV24h;
	static void interrupt far NewISR24h(REGS_BINT regs);
#else
	static void interrupt far   NewISR08h(...);
	static void interrupt (far *OldIV08h)(...);
	//
	static void interrupt far   NewISR09h(...);
	static void interrupt (far *OldIV09h)(...);
	//
	static void interrupt far   NewISR1Bh(...);
	static void interrupt (far *OldIV1Bh)(...);
	//
	static void interrupt far   NewISR23h(...);
	static void interrupt (far *OldIV23h)(...);
	//
	static void interrupt far   NewISR24h(...);
	static void interrupt (far *OldIV24h)(...);
#endif

protected:
	static ENGINE *pThis;

};

// this macro useful to check bit "i" of Value
#define BIT(value, i) ((value) & (1<<i))

const WORD PIT_MSECDIV       = 1190U;  // divisor for one mili-second
const WORD PIT_TENMSECDIV    = 11900U; // divisor for ten mili-seconds
const WORD BOOTH_PIT_DIVISOR = PIT_TENMSECDIV;
const WORD T_EVAL            = BOOTH_PIT_DIVISOR/PIT_MSECDIV; // synchronize
const UINT PIC_PORT          = 0x20;
const BYTE EOI               = 0x20; // non-specific EOI to PIC, OCW2 (EOI=1, SL=0, R=0)

#endif // __ENGINE_H
