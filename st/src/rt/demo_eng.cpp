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
	memset(_types,  0, sizeof(_types));
	memset(_phones, 0, sizeof(_phones));
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
	ParsePhones();

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
	// Map b.type -> CALL_ATTR (st_defs.h).  Used to pin CallAttrs once
	// the FSM has left OFFHOOK, so DoInterdig's NOT_INCLUDED threshold
	// check (rt_do.cpp:355) doesn't fire LOCK on a stale controller
	// refresh.  Controller's cookViewPhoneInfo runs Search() every
	// VIEW_REFRESH_TIME=500ms and flags partial dials as NOT_INCLUDED
	// before the prefix is in the place tree; demo dials at ~120ms/digit,
	// far faster than the refresh, so by the time NumOfDigits crosses
	// LOCAL_DIGITS_NOT_INCLUDED=4 / NAL_DIGITS_NOT_INCLUDED=9 /
	// INTER_DIGITS_NOT_INCLUDED=10, CallAttrs still has NOT_INCLUDED
	// from an earlier "1-digit prefix unknown" Search -- and the booth
	// gets LOCKed mid-dial.  This write loses races with the controller
	// only briefly (10ms ISR window); EvalState reads CallAttrs after
	// our write each tick, so DoInterdig never sees the stale flag.
	static const WORD demoCallAttr[3] = { LOCAL_CALL, DDN_CALL, DDI_CALL };

	for (WORD bNum = 0; bNum < CLUSTER_SIZE; bNum++)
	{
		WORD bAbs = cNum * CLUSTER_SIZE + bNum;
		if (bAbs >= _numBooths)
			break;
		DemoBooth & b = _booths[bAbs];

		// Pin CallAttrs to the known call type during the active dial.
		// Skip DP_IDLE (no call) and DP_OFFHOOK (DoOffHook -> ResetData
		// clobbers CallAttrs every tick until the FSM leaves OFFHOOK).
		if (b.phase != DP_IDLE && b.phase != DP_OFFHOOK)
			Clusters[cNum].CallAttrs[bNum] = demoCallAttr[b.type];

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
				// T_OFF_HOOK is in ms; the ISR fires once per T_EVAL ms,
				// so wait g_cfg->T_OFF_HOOK / T_EVAL ticks plus a small
				// margin for the FSM's Inc/Dec dance to settle.
				b.countdown = (DWORD)g_cfg->T_OFF_HOOK / T_EVAL + 5;
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
				// dtmfTimer is decremented per ISR tick.  Convert the
				// ms-valued g_cfg threshold into ticks before adding the
				// safety margin.
				b.dtmfTimer  = (WORD)(g_cfg->T_DTMF_FLAG / T_EVAL + 2);
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
				b.dtmfTimer   = (WORD)(g_cfg->T_DTMF_INTERDIG / T_EVAL + 2);
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
					// all digits sent; drive answer signal long enough
					// for the FSM's bias counter to clear T_BIAS-T_BIAS_MARGIN.
					b.dtmfTimer = (WORD)(g_cfg->T_BIAS / T_EVAL + 10);
					b.phase     = DP_ANSWER_WAIT;
				}
				else
				{
					// next digit: go high
					DWORD mask    = ~(0x0FUL << (bNum * 4));
					dp.DTMFDigits = (dp.DTMFDigits & mask)
					              | ((DWORD)b.digits[b.digitIdx] << (bNum * 4));
					dp.DTMFFlags |= (BYTE)(1 << bNum);
					b.dtmfTimer   = (WORD)(g_cfg->T_DTMF_FLAG / T_EVAL + 2);
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
// ParsePhones: read phones.csv from the working directory and fill the
// per-type pools.  RFC 4180 CSV with all string fields double-quoted;
// lines starting with ';' are comments, the first '"category",...'
// line is the header.  We only need columns 0 (category) and 7
// (dial_from_smarttar).  Missing/malformed file -> empty pools, and
// GenCall falls back to its small hardcoded prefix set.
// Called from InitHardware (not ISR).
// -------------------------------------------------------------------------

void DEMO_ENGINE::ParsePhones(void)
{
	FILE *fp = fopen("phones.csv", "r");
	if (!fp)
	{
		// Leave a diagnostic breadcrumb so the runtime tells us why
		// GenCall fell back to the hardcoded pools.
		FILE *dbg = fopen("phones.dbg", "w");
		if (dbg)
		{
			fprintf(dbg, "DEMO_ENGINE::ParsePhones: fopen(\"phones.csv\") FAILED\n");
			fprintf(dbg, "  -> GenCall will use the hardcoded fallback pools.\n");
			fclose(dbg);
		}
		return;
	}

	char line[512];
	while (fgets(line, sizeof(line), fp))
	{
		// Skip comments and the header row (which starts with '"c...').
		if (line[0] == ';' || line[0] == '\n' || line[0] == '\r')
			continue;
		if (line[0] != '"')
			continue;
		if (line[1] == 'c' && line[2] == 'a')   // "category" header
			continue;

		// Parse: walk the line as "<f0>","<f1>",...,"<f10>". No commas
		// inside fields (we control the dataset), no escaped quotes.
		char *fields[11];
		int   nFields = 0;
		char *p = line;
		int   parseOK = 1;
		while (*p && nFields < 11)
		{
			if (*p != '"') { parseOK = 0; break; }
			p++;
			fields[nFields++] = p;
			while (*p && *p != '"') p++;
			if (*p != '"') { parseOK = 0; break; }
			*p++ = '\0';            // terminate the field
			if (*p == ',') p++;     // skip comma between fields
		}
		if (!parseOK || nFields < 8)
			continue;

		// Field 0 = category
		int typeIdx = -1;
		if      (strcmp(fields[0], "LOCAL") == 0) typeIdx = 0;
		else if (strcmp(fields[0], "NAL")   == 0) typeIdx = 1;
		else if (strcmp(fields[0], "INTER") == 0) typeIdx = 2;
		if (typeIdx < 0)
			continue;

		PhonePool & pool = _phones[typeIdx];
		if (pool.count >= DEMO_MAX_PHONES_PER_TYPE)
			continue;

		// Field 7 = dial_from_smarttar -- convert ASCII digits to BYTE 0..9
		BYTE  n = 0;
		char *d = fields[7];
		while (*d && n < DEMO_MAX_DIGITS_PER_PHONE)
		{
			if (*d >= '0' && *d <= '9')
				pool.digits[pool.count][n++] = (BYTE)(*d - '0');
			d++;
		}
		if (n == 0)
			continue;
		pool.digitCount[pool.count] = n;
		pool.count++;
	}
	fclose(fp);

	// Write a one-shot diagnostic so we can confirm at runtime that
	// ParsePhones found phones.csv and loaded entries (the Zinc UI
	// captures stdout, so a printf is invisible).  File lands in the
	// same CWD as phones.csv (= bin/ when launched via run.bat).
	FILE *dbg = fopen("phones.dbg", "w");
	if (dbg)
	{
		fprintf(dbg, "DEMO_ENGINE::ParsePhones loaded: %u LOCAL, %u NAL, %u INTER\n",
			(unsigned)_phones[0].count,
			(unsigned)_phones[1].count,
			(unsigned)_phones[2].count);
		static const char *labels[3] = { "LOCAL", "NAL", "INTER" };
		for (int t = 0; t < 3; t++)
		{
			PhonePool & p = _phones[t];
			for (BYTE i = 0; i < p.count; i++)
			{
				fprintf(dbg, "  %-5s [%2u] dial=", labels[t], (unsigned)i);
				for (BYTE k = 0; k < p.digitCount[i]; k++)
					fputc('0' + p.digits[i][k], dbg);
				fprintf(dbg, " (%u digits)\n", (unsigned)p.digitCount[i]);
			}
		}
		fclose(dbg);
	}
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

	DemoCallType & ct = _types[b.type];
	b.numDigits = (BYTE)ct.numDigits;
	if (b.numDigits > 16)
		b.numDigits = 16;

	// Access prefix per call type -- so the FSM classifies the call as
	// LOCAL/DDN/DDI before the place lookup runs.
	BYTE d = 0;
	if (b.type == 1)        // NAL: access(0) + operator(9)
	{
		b.digits[d++] = (BYTE)g_cfg->ACCESS;
		b.digits[d++] = (BYTE)g_cfg->OPERATOR_ACCESS;
	}
	else if (b.type == 2)   // INTER: access(0) + inter(0) + operator(9)
	{
		b.digits[d++] = (BYTE)g_cfg->ACCESS;
		b.digits[d++] = (BYTE)g_cfg->INTER_ACCESS;
		b.digits[d++] = (BYTE)g_cfg->OPERATOR_ACCESS;
	}

	// Real-number pool from phones.csv: a random row's full dial sequence
	// is the rest of the digits.  Each row was vetted against
	// util/inf2dat/{local,ddn,ddi}.inf so the call resolves to a named
	// destination rather than "--- No Incluida ---".
	PhonePool & pool = _phones[b.type];
	if (pool.count > 0)
	{
		BYTE idx = (BYTE)(LcgNext() % pool.count);
		BYTE n   = pool.digitCount[idx];
		for (BYTE i = 0; i < n && d < b.numDigits; i++)
			b.digits[d++] = pool.digits[idx][i];
	}
	else
	{
		// Fallback when phones.csv is missing or empty -- minimal
		// hardcoded prefixes that still hit named ph_info.dat entries.
		// LOCAL: 2/3/4 prefix -> Medellin(Ant) 200-499.
		// NAL: Bogota Etb 2-digit singles (12,13,17) + ranges (14X,16X).
		// INTER: NANP area codes + 2-digit country codes from ddi.inf.
		static const char  localFirst[] = { 2, 3, 4 };
		static const char *nalPool[] = {
			"12", "13", "14", "16", "17"
		};
		static const char *interPool[] = {
			"1212", "1305", "1416",
			"33",   "34",   "39",   "44",   "49",   "54",   "55",   "56"
		};
		if (b.type == 0)
		{
			b.digits[d++] = (BYTE)localFirst[LcgNext() % 3];
		}
		else if (b.type == 1)
		{
			const char *p = nalPool[LcgNext() % (sizeof(nalPool)/sizeof(nalPool[0]))];
			while (*p && d < b.numDigits)
				b.digits[d++] = (BYTE)(*p++ - '0');
		}
		else
		{
			const char *p = interPool[LcgNext() % (sizeof(interPool)/sizeof(interPool[0]))];
			while (*p && d < b.numDigits)
				b.digits[d++] = (BYTE)(*p++ - '0');
		}
	}

	// Pad with random subscriber digits (1..9) if the pool row was
	// shorter than numDigits.  Most rows are exact length; this
	// only fires for the Italy/Germany short entries.
	while (d < b.numDigits)
		b.digits[d++] = (BYTE)((LcgNext() % 9) + 1);

	// Call duration: uniform in [min, max] -- stored in b.duration, not
	// b.countdown (countdown is reused for the offhook-wait phase).
	DWORD range = (ct.maxDurTicks > ct.minDurTicks)
	            ? (ct.maxDurTicks - ct.minDurTicks) : 1;
	b.duration = ct.minDurTicks + (LcgNext() % range);
}
