//
// [ DEMO_ENG.CPP ]
//
// DEMO_ENGINE: fake engine for demo / dev / training use.
//
// Phase 2: Poisson arrival generator.  Each booth runs an independent
// schedule driven by demo.ini.  OnTimerTick writes DataPort.OOD /
// DTMFFlags / DTMFDigits / Answer / ThreadC to push each booth through
//   ONHOOK -> OFFHOOK -> DIALING -> ANSWER -> TALK -> ONHOOK
// on its own timeline so the rest of the system can't tell it's synthetic.
//
// ISR constraints honored:
//   - No malloc/new inside OnTimerTick; all state pre-allocated.
//   - No C runtime calls except pure arithmetic.
//   - Private LCG replaces rand() (not ISR-safe in Borland C++ 3.1).
//   - No floating-point in OnTimerTick; inter-arrival uses integer LCG.
//

#include "stdst.h"

#include <demo_eng.h>
#include <stdio.h>   // fopen, fgets
#include <string.h>  // strcmp, strncpy, memset, strlen
#include <stdlib.h>  // atoi, atol
#include <ctype.h>   // isspace

extern CFG *g_cfg;

// -------------------------------------------------------------------------
// Constructor / destructor
// -------------------------------------------------------------------------

DEMO_ENGINE::DEMO_ENGINE(WORD numOfClusters)
	: ENGINE(numOfClusters),
	  _booths(NULL),
	  _numBooths(0),
	  _lcgState(12345678UL),
	  _totalWeight(0),
	  _meanArrivalTicks(1000)
{
	memset(_types, 0, sizeof(_types));
	InitHardware(numOfClusters);
	RecoverState();
	InstallISRs();
}

DEMO_ENGINE::~DEMO_ENGINE(void)
{
	delete[] _booths;
}

// -------------------------------------------------------------------------
// Template Method hooks
// -------------------------------------------------------------------------

void DEMO_ENGINE::InitHardware(WORD numOfClusters)
{
	if (g_cfg->ACTIVE_CLUSTERS == 0)
		g_cfg->ACTIVE_CLUSTERS = (WORD)numOfClusters;

	Clusters    = new BoothCluster[g_cfg->ACTIVE_CLUSTERS];
	_numBooths  = g_cfg->ACTIVE_CLUSTERS * CLUSTER_SIZE;
	_booths     = new DemoBooth[_numBooths];
	memset(_booths, 0, sizeof(DemoBooth) * _numBooths);

	ParseConfig();

	// Seed per-booth arrivals with staggered initial delays so booths
	// don't all fire on tick 0.
	for (WORD i = 0; i < _numBooths; i++)
	{
		_booths[i].phase     = DP_IDLE;
		_booths[i].countdown = (DWORD)(i * (_meanArrivalTicks / (_numBooths + 1) + 1));
	}

	// Make every booth available and start in ONHOOK so the FSM will
	// process DataPort signals from OnTimerTick.
	for (WORD cNum = 0; cNum < g_cfg->ACTIVE_CLUSTERS; cNum++)
	{
		Clusters[cNum].Available = TRUE;
		for (WORD bNum = 0; bNum < CLUSTER_SIZE; bNum++)
			SetFSs(cNum, bNum, ONHOOK, ONHOOK);
	}
}

void DEMO_ENGINE::RecoverState(void)
{
	// Demo runs are not persistent -- nothing to recover.
}

// -------------------------------------------------------------------------
// OnTimerTick: called from NewISR08h once per active cluster per tick.
// Advances the per-booth schedule and writes DataPort fields.
// ISR context: no heap, no C runtime, no float.
// -------------------------------------------------------------------------

void DEMO_ENGINE::OnTimerTick(WORD cNum, BoothCluster::_DataPort & dp)
{
	for (WORD bNum = 0; bNum < CLUSTER_SIZE; bNum++)
	{
		WORD bAbs = cNum * CLUSTER_SIZE + bNum;
		if (bAbs >= _numBooths)
			break;
		DemoBooth & b = _booths[bAbs];

		switch (b.phase)
		{
		case DP_IDLE:
			dp.OOD    &= ~(BYTE)(1 << bNum);
			dp.Answer &= ~(BYTE)(1 << bNum);
			dp.ThreadC |=  (BYTE)(1 << bNum); // ThreadC idle = bit HIGH
			if (b.countdown == 0)
			{
				GenCall(b);
				b.phase     = DP_OFFHOOK;
				b.countdown = (DWORD)g_cfg->T_OFF_HOOK + 5;
				dp.OOD |= (BYTE)(1 << bNum);
			}
			else
			{
				b.countdown--;
			}
			break;

		case DP_OFFHOOK:
			// OOD stays high; wait for the FSM's T_OFF_HOOK debounce
			// before feeding DTMF (prevents DTMF starting before OFFHOOK).
			dp.OOD     |= (BYTE)(1 << bNum);
			dp.DTMFFlags &= ~(BYTE)(1 << bNum);
			if (b.countdown == 0)
			{
				b.digitIdx   = 0;
				b.dtmfTimer  = (WORD)g_cfg->T_DTMF_FLAG + 2;
				// put first digit into DTMFDigits for this booth slot
				DWORD mask = ~(0x0FUL << (bNum * 4));
				dp.DTMFDigits = (dp.DTMFDigits & mask)
				              | ((DWORD)b.digits[0] << (bNum * 4));
				dp.DTMFFlags |= (BYTE)(1 << bNum);
				b.phase      = DP_DIALING_HI;
			}
			else
			{
				b.countdown--;
			}
			break;

		case DP_DIALING_HI:
			// DTMFFlags bit HIGH: tone is active; keep digit stable.
			dp.OOD      |= (BYTE)(1 << bNum);
			dp.DTMFFlags |= (BYTE)(1 << bNum);
			{
				DWORD mask    = ~(0x0FUL << (bNum * 4));
				dp.DTMFDigits = (dp.DTMFDigits & mask)
				              | ((DWORD)b.digits[b.digitIdx] << (bNum * 4));
			}
			if (b.dtmfTimer == 0)
			{
				// tone done; drop flag so FSM reads the digit
				dp.DTMFFlags &= ~(BYTE)(1 << bNum);
				b.dtmfTimer   = (WORD)g_cfg->T_DTMF_INTERDIG + 2;
				b.phase       = DP_DIALING_LO;
			}
			else
			{
				b.dtmfTimer--;
			}
			break;

		case DP_DIALING_LO:
			// DTMFFlags bit LOW: inter-digit gap.
			dp.OOD      |= (BYTE)(1 << bNum);
			dp.DTMFFlags &= ~(BYTE)(1 << bNum);
			if (b.dtmfTimer == 0)
			{
				b.digitIdx++;
				if (b.digitIdx >= b.numDigits)
				{
					// all digits sent; drive answer signal
					b.dtmfTimer = (WORD)g_cfg->T_BIAS + 10;
					b.phase     = DP_ANSWER_WAIT;
				}
				else
				{
					// next digit: go high
					DWORD mask    = ~(0x0FUL << (bNum * 4));
					dp.DTMFDigits = (dp.DTMFDigits & mask)
					              | ((DWORD)b.digits[b.digitIdx] << (bNum * 4));
					dp.DTMFFlags |= (BYTE)(1 << bNum);
					b.dtmfTimer   = (WORD)g_cfg->T_DTMF_FLAG + 2;
					b.phase       = DP_DIALING_HI;
				}
			}
			else
			{
				b.dtmfTimer--;
			}
			break;

		case DP_ANSWER_WAIT:
			// Hold the configured answer signal long enough for the FSM's
			// bias/thread counter to reach its threshold.
			dp.OOD      |= (BYTE)(1 << bNum);
			dp.DTMFFlags &= ~(BYTE)(1 << bNum);
			if (g_cfg->ASIGNAL == CFG::S_BIAS)
				dp.Answer  |= (BYTE)(1 << bNum);
			else
				dp.ThreadC &= ~(BYTE)(1 << bNum); // S_THREAD/S_TONE: bit LOW = answer
			if (b.dtmfTimer == 0)
			{
				// clear signal; FSM should be in ANSWER or TALK by now
				dp.Answer  &= ~(BYTE)(1 << bNum);
				dp.ThreadC |=  (BYTE)(1 << bNum);
				b.phase     = DP_TALKING;
				// countdown = call duration (already set by GenCall)
			}
			else
			{
				b.dtmfTimer--;
			}
			break;

		case DP_TALKING:
			// Call in progress.  OOD stays high.  Count down on b.duration.
			dp.OOD     |= (BYTE)(1 << bNum);
			dp.DTMFFlags &= ~(BYTE)(1 << bNum);
			dp.Answer  &= ~(BYTE)(1 << bNum);
			dp.ThreadC |=  (BYTE)(1 << bNum);
			if (b.duration == 0)
			{
				// hang up: clear OOD so FSM transitions TALK -> STORE
				dp.OOD &= ~(BYTE)(1 << bNum);
				b.phase = DP_IDLE;
				// draw next inter-arrival countdown
				b.countdown = (DWORD)(LcgNext() % (_meanArrivalTicks * 2)) + 1;
			}
			else
			{
				b.duration--;
			}
			break;
		}
	}
}

void DEMO_ENGINE::OnTimerEnd(void)
{
	// No general-port hardware to drive.
}

// -------------------------------------------------------------------------
// ParseConfig: read demo.ini from the working directory.
// Called from InitHardware (not ISR).  Simple key=value INI parser.
// -------------------------------------------------------------------------

static void TrimRight(char *s)
{
	int n = (int)strlen(s);
	while (n > 0 && (s[n-1] == '\r' || s[n-1] == '\n' || s[n-1] == ' '))
		s[--n] = '\0';
}

void DEMO_ENGINE::ParseConfig(void)
{
	// Sensible defaults (overridden by demo.ini if present).
	_types[0].weight       = 70; // LOCAL
	_types[0].numDigits    = (WORD)g_cfg->LOCAL_DIGITS;
	_types[0].minDurTicks  = 30UL  * 100; // 30 s
	_types[0].maxDurTicks  = 300UL * 100; // 5 min

	_types[1].weight       = 25; // NAL
	_types[1].numDigits    = (WORD)g_cfg->NAL_DIGITS;
	_types[1].minDurTicks  = 60UL  * 100;
	_types[1].maxDurTicks  = 600UL * 100;

	_types[2].weight       = 5;  // INTER
	_types[2].numDigits    = (WORD)g_cfg->INTER_DIGITS;
	_types[2].minDurTicks  = 120UL * 100;
	_types[2].maxDurTicks  = 900UL * 100;

	WORD callsPerMinute = 6;
	WORD seed           = 0;

	FILE *fp = fopen("demo.ini", "r");
	if (fp)
	{
		char   line[128];
		int    section = -1; // -1=none, 0=GLOBAL, 1=LOCAL, 2=NAL, 3=INTER
		while (fgets(line, sizeof(line), fp))
		{
			TrimRight(line);
			if (line[0] == ';' || line[0] == '\0')
				continue;
			if (line[0] == '[')
			{
				if      (strcmp(line, "[GLOBAL]") == 0) section = 0;
				else if (strcmp(line, "[LOCAL]")  == 0) section = 1;
				else if (strcmp(line, "[NAL]")    == 0) section = 2;
				else if (strcmp(line, "[INTER]")  == 0) section = 3;
				else                                     section = -1;
				continue;
			}
			char *eq = strchr(line, '=');
			if (!eq) continue;
			*eq = '\0';
			char *key = line;
			char *val = eq + 1;

			if (section == 0) // GLOBAL
			{
				if      (strcmp(key, "calls_per_minute") == 0)
					callsPerMinute = (WORD)atoi(val);
				else if (strcmp(key, "seed") == 0)
					seed = (WORD)atoi(val);
			}
			else if (section >= 1 && section <= 3)
			{
				int t = section - 1;
				if      (strcmp(key, "weight")            == 0)
					_types[t].weight       = (WORD)atoi(val);
				else if (strcmp(key, "min_duration_secs") == 0)
					_types[t].minDurTicks  = (DWORD)atol(val) * 100;
				else if (strcmp(key, "max_duration_secs") == 0)
					_types[t].maxDurTicks  = (DWORD)atol(val) * 100;
			}
		}
		fclose(fp);
	}

	// Apply seed (0 = use the constructor default).
	if (seed != 0)
		_lcgState = (DWORD)seed;

	// Compute total weight and mean inter-arrival per booth (ticks).
	_totalWeight = 0;
	for (int i = 0; i < 3; i++)
		_totalWeight = (WORD)(_totalWeight + _types[i].weight);
	if (_totalWeight == 0)
		_totalWeight = 100;

	// Mean inter-arrival per booth (ticks at 100 Hz).
	// callsPerMinute is aggregate across all booths.
	WORD effectiveBooths = (_numBooths > 0) ? _numBooths : 1;
	WORD effectiveCPM    = (callsPerMinute > 0) ? callsPerMinute : 1;
	DWORD perBoothPerSec = (DWORD)effectiveCPM * 100 / (60UL * effectiveBooths);
	if (perBoothPerSec == 0) perBoothPerSec = 1;
	// mean = 1 / (perBoothPerSec ticks^-1) but we store in ticks:
	// 100 ticks/s / (perBoothPerSec calls/s) per booth
	// Actually: per-booth rate = callsPerMinute / (60 * numBooths) calls/s
	// mean ticks = 6000 * numBooths / callsPerMinute
	DWORD meanTicks = (DWORD)6000 * effectiveBooths / effectiveCPM;
	_meanArrivalTicks = (meanTicks > 0xFFFFU) ? 0xFFFFU : (WORD)meanTicks;
}

// -------------------------------------------------------------------------
// GenCall: pre-generate one call's digit sequence and duration.
// Called from OnTimerTick when a booth fires (not in tight ISR inner
// loop, but still in ISR context -- no malloc, no runtime).
// -------------------------------------------------------------------------

void DEMO_ENGINE::GenCall(DemoBooth & b)
{
	// Select call type by weight
	DWORD roll = LcgNext() % _totalWeight;
	WORD  sum  = 0;
	b.type     = 0;
	for (int i = 0; i < 3; i++)
	{
		sum = (WORD)(sum + _types[i].weight);
		if (roll < (DWORD)sum)
		{
			b.type = (BYTE)i;
			break;
		}
	}

	// Build a dial sequence the tariff engine will actually recognize:
	//   access prefix from g_cfg (so the call gets classified as
	//   LOCAL/DDN/DDI), then a real place prefix from ph_info.dat
	//   territory (so PLACE_INFO::Search hits an entry instead of
	//   "--- No Incluida ---"), then random subscriber digits to fill
	//   out to numDigits.  Pools below are representative entries
	//   from util/inf2dat/{local,ddn,ddi}.inf -- keep them small;
	//   exhaustive coverage isn't the point.
	DemoCallType & ct = _types[b.type];
	b.numDigits = (BYTE)ct.numDigits;
	if (b.numDigits > 16)
		b.numDigits = 16;

	// LOCAL: any 7-digit number starting with 2/3/4 hits Medellin in
	// local.inf (prefixes 200-499).
	static const char  localFirst[] = { 2, 3, 4 };
	// DDN: after stripping the carrier "09", these prefixes are
	// guaranteed to find a place in ddn.inf within 1-2 subscriber
	// digits.  All from Bogota Etb (12,13,141-149,150,155,161-169,17):
	//   - 12, 13, 17 are 2-digit singles      -> match at dialed digit 4
	//   - 14, 16 expand via range 14X / 16X   -> match at dialed digit 5
	//     for any subscriber 1-9
	// Don't add 2-digit "1X" combos that aren't singles in ddn.inf
	// (e.g. "15" only matches a few specific 3rd digits) -- the FSM
	// locks the booth at NumOfDigits >= NAL_DIGITS_NOT_INCLUDED (9)
	// if Search never succeeds, killing the call mid-dial.
	static const char *nalPool[] = {
		"12", "13", "14", "16", "17"
	};
	// DDI: after stripping "009", these exist in ddi.inf.
	// Mix of NANP area codes (4-digit singles/ranges) and 2-digit
	// country codes -- both match exactly at their natural length.
	static const char *interPool[] = {
		"1212", "1305", "1416",
		"33",   "34",   "39",   "44",   "49",   "54",   "55",   "56"
	};

	BYTE d = 0;
	if (b.type == 0)
	{
		// LOCAL: just the subscriber number.
		b.digits[d++] = (BYTE)localFirst[LcgNext() % 3];
	}
	else if (b.type == 1)
	{
		// DDN: access(0) + operator(9), then a real DDN prefix.
		b.digits[d++] = (BYTE)g_cfg->ACCESS;
		b.digits[d++] = (BYTE)g_cfg->OPERATOR_ACCESS;
		const char *p = nalPool[LcgNext() % (sizeof(nalPool)/sizeof(nalPool[0]))];
		while (*p && d < b.numDigits)
			b.digits[d++] = (BYTE)(*p++ - '0');
	}
	else
	{
		// DDI: access(0) + inter(0) + operator(9), then a real DDI prefix.
		b.digits[d++] = (BYTE)g_cfg->ACCESS;
		b.digits[d++] = (BYTE)g_cfg->INTER_ACCESS;
		b.digits[d++] = (BYTE)g_cfg->OPERATOR_ACCESS;
		const char *p = interPool[LcgNext() % (sizeof(interPool)/sizeof(interPool[0]))];
		while (*p && d < b.numDigits)
			b.digits[d++] = (BYTE)(*p++ - '0');
	}
	// Pad subscriber digits (1..9).
	while (d < b.numDigits)
		b.digits[d++] = (BYTE)((LcgNext() % 9) + 1);

	// Call duration: uniform in [min, max] — stored in b.duration, not
	// b.countdown (countdown is reused for the offhook-wait phase).
	DWORD range = (ct.maxDurTicks > ct.minDurTicks)
	            ? (ct.maxDurTicks - ct.minDurTicks) : 1;
	b.duration = ct.minDurTicks + (LcgNext() % range);
}
