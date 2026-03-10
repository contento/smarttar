#ifndef __BCLUSTER_H
#define __BCLUSTER_H

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

class BoothCluster // 2.21.1 Build 4 -> convert to class
{
public:

	BoothCluster()
	{
		Init();
	}

	void Init()
	{
		memset(this, 0, sizeof(*this));
	}
public:

	struct _DataPort
	{
		BYTE OOD;
		BYTE Answer;
		BYTE DTMFFlags;
		union
		{
			DWORD DTMFDigits;
			WORD  U_DTMFDigits[2];   // a pair of UINTs, U_ stands for WORD
		};
		BYTE Lock;
		BYTE Spy;
		BYTE ThreadC;
	} DataPort;

	BOOL Available;                   // flat to indicate availability
	BOOL Locked       [CLUSTER_SIZE]; // flat to forced locks
	BOOL Found        [CLUSTER_SIZE]; // flag to avoid to search again
	BOOL FirstOnes    [CLUSTER_SIZE]; // flag to indicate if the first one is checked
	BOOL ManualAnswer [CLUSTER_SIZE]; // flag to indicate reset for Manual Answer
	BOOL Simula       [CLUSTER_SIZE]; // flag to indicate in simula
	BOOL NoReceipt    [CLUSTER_SIZE]; // flag to indicate not to generate receipt
	BOOL NoStatistics [CLUSTER_SIZE]; // flag to indicate not to generate statistics
	BOOL PrePaid      [CLUSTER_SIZE]; // flag to indicate prepaid call
	BOOL FirstPreValue[CLUSTER_SIZE]; // flag to indicate first prepaid call (manual) v.219

	PHONE  Phones       [CLUSTER_SIZE];
	PHONE  SimulaPhones [CLUSTER_SIZE]; // simula phones
	WORD   StartDates   [CLUSTER_SIZE]; // packed
	WORD   StartTimes   [CLUSTER_SIZE]; // packed
	DWORD  ElapsedCounts[CLUSTER_SIZE];
	WORD   Tariffs      [CLUSTER_SIZE];
	WORD   NumOfCalls   [CLUSTER_SIZE]; // Manual Mode
	WORD   ComErrors    [CLUSTER_SIZE]; // to keep track of errors
	WORD   DialErrors   [CLUSTER_SIZE];
	//
	DWORD  FinalElapsedCounts[CLUSTER_SIZE]; // 2.21.1 Build 4
	//
	double PreValue     [CLUSTER_SIZE];
	DWORD  PreTime      [CLUSTER_SIZE];

	WORD  StateCounts  [CLUSTER_SIZE]; // states counts
	WORD  DialCounts   [CLUSTER_SIZE]; // dial counts
	WORD  OnHookCounts [CLUSTER_SIZE]; // ON_HOOK counts
	WORD  BiasCounts   [CLUSTER_SIZE]; // bias counts
	BYTE  NumOfDigits  [CLUSTER_SIZE]; // digit counters per booth
	BYTE  CurrentDigits[CLUSTER_SIZE]; // current digits per booth
	WORD  PulseFSs     [CLUSTER_SIZE]; // pulse finite states
	WORD  ToneFSs      [CLUSTER_SIZE]; // tone finite states
	WORD  CallAttrs    [CLUSTER_SIZE]; // to indicate attr (e.g.NOT_INC)
	BOOL  IncomeCalls  [CLUSTER_SIZE]; // to indicate an income call

};

#endif // __BCLUSTER_H
