#ifndef __DEMO_ENG_H
#define __DEMO_ENG_H

#if !defined(__ENGINE_H)
#include <engine.h>
#endif

//
// DEMO_ENGINE: fake engine for demo / dev / training use. No hardware
// dependency. Phase 2: Poisson arrival generator driven by demo.ini.
//
// Honors the same concurrency contract as RT_ENGINE: hooks IRQ0 via the
// base class InstallISRs(), writes BoothCluster::_DataPort fields from
// inside OnTimerTick() so downstream consumers can't tell it's synthetic.
//
class DEMO_ENGINE : public ENGINE
{
public:
	DEMO_ENGINE(WORD numOfClusters=1);
	virtual ~DEMO_ENGINE(void);

	virtual void InitHardware(WORD numOfClusters);
	virtual void RecoverState(void);
	virtual void OnTimerTick(WORD cNum, BoothCluster::_DataPort & dataPort);
	virtual void OnTimerEnd(void);

private:

	// --- per-booth demo schedule phases ------------------------------------
	// Independent of the FSM states (ONHOOK, OFFHOOK, ...) in ENGINE.
	// The demo engine writes DataPort fields to drive those FSM transitions.
	enum
	{
		DP_IDLE,          // waiting until next call arrival countdown
		DP_OFFHOOK,       // OOD high, waiting T_OFF_HOOK before DTMF
		DP_DIALING_HI,    // DTMFFlags high (tone active)
		DP_DIALING_LO,    // DTMFFlags low  (inter-digit gap, or post-dial)
		DP_ANSWER_WAIT,   // driving Answer/ThreadC to trigger ANSWER state
		DP_TALKING        // call in progress; countdown to hangup
	};

	// --- per-booth state (pre-allocated, ISR-safe) -------------------------
	struct DemoBooth
	{
		BYTE  phase;
		BYTE  type;       // 0=LOCAL, 1=NAL, 2=INTER
		BYTE  numDigits;
		BYTE  digitIdx;
		WORD  dtmfTimer;  // ticks remaining in current DTMF hi/lo phase
		DWORD countdown;  // ticks to next phase transition (inter-arrival or offhook wait)
		DWORD duration;   // call duration in ticks (set by GenCall, consumed in DP_TALKING)
		BYTE  digits[16]; // pre-generated digit sequence
	};

	// --- per-type parameters -----------------------------------------------
	struct DemoCallType
	{
		WORD  weight;
		WORD  numDigits;
		DWORD minDurTicks;
		DWORD maxDurTicks;
	};

	// --- members -----------------------------------------------------------
	DemoBooth    *_booths;      // [ACTIVE_CLUSTERS * CLUSTER_SIZE]
	WORD          _numBooths;
	DWORD         _lcgState;
	WORD          _totalWeight;
	WORD          _meanArrivalTicks; // per-booth mean inter-arrival
	DemoCallType  _types[3];         // LOCAL, NAL, INTER

	// --- helpers (called from InitHardware, not ISR) ----------------------
	void ParseConfig(void);
	void GenCall(DemoBooth & b);

	// --- ISR-safe RNG -----------------------------------------------------
	DWORD LcgNext(void)
	{
		_lcgState = _lcgState * 1664525UL + 1013904223UL;
		return _lcgState;
	}
};

#endif // __DEMO_ENG_H
