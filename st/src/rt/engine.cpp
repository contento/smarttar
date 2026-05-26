//
// [ ENGINE.CPP ]
//
// ENGINE base class: shared lifecycle, ISR install/uninstall, PIT
// programming, and the FSM dispatchers (EvalToneState / EvalPulseState)
// that both RT_ENGINE and DEMO_ENGINE drive.
//

#include "stdst.h"

#include <engine.h>

extern CFG *g_cfg;

ENGINE *ENGINE::pThis = NULL;

ENGINE::ENGINE(WORD /*numOfClusters*/)
	:
	Receipts(NULL),
	Clusters(NULL),
	GeneralPort(0),
	CurrentDate(0),
	CurrentTime(0),
	SpyBooth(-1),
	LastSpyBooth(-1),
	NotIncBooth(-1),
	BadInterBooth(-1),
	ComErrBooth(-1),
	DialErrBooth(-1)
{
	pThis = this; // 2.30 build 10
#if !defined(__TEST__)
	Receipts = new CIRCULAR_QUEUE<DynamicReceipt>(RT_MAXRECEIPTS);
#endif
}

ENGINE::~ENGINE(void)
{
	UninstallISRs();
	SetPITRate(0); // by default 0 implies 65536 or 18.2 Hz
#if !defined(__TEST__)
	delete Receipts;
#endif
	delete Clusters;
}

//
// Install IRQ0 + Pause + Ctrl-C + Ctrl-Break + critical-error vectors.
// Called by derived ctor AFTER InitHardware/RecoverState so the ISR
// can safely deref pThis->GetDataPort etc.
//
void ENGINE::InstallISRs(void)
{
	SetPITRate(BOOTH_PIT_DIVISOR);
#ifdef DOSX286
	DosSetPassToProtVec(0x08, (PIHANDLER)NewISR08h, &OldProtIV08h, &OldRealIV08h);
	if (g_cfg->CHECK_PAUSE_KEY)
		DosSetPassToProtVec(0x09, (PIHANDLER)NewISR09h, &OldProtIV09h, &OldRealIV09h);
	DosSetPassToProtVec(0x1B, (PIHANDLER)NewISR1Bh, &OldProtIV1Bh, &OldRealIV1Bh);
	DosSetPassToProtVec(0x23, (PIHANDLER)NewISR23h, &OldProtIV23h, &OldRealIV23h);
	DosSetPassToProtVec(0x24, (PIHANDLER)NewISR24h, &OldProtIV24h, &OldRealIV24h);
#else
	OldIV08h = getvect(0x08);
	setvect(0x08, NewISR08h);
	if (g_cfg->CHECK_PAUSE_KEY)
		OldIV09h = getvect(0x09);
	setvect(0x09, NewISR09h);
	OldIV1Bh = getvect(0x1B);
	setvect(0x1B, NewISR1Bh);
	OldIV23h = getvect(0x23);
	setvect(0x23, NewISR23h);
	OldIV24h = getvect(0x24);
	setvect(0x24, NewISR24h);
#endif
}

void ENGINE::UninstallISRs(void)
{
#ifdef DOSX286
	DosSetRealProtVec(0x08, OldProtIV08h, OldRealIV08h, NULL, NULL);
	if (g_cfg->CHECK_PAUSE_KEY)
		DosSetRealProtVec(0x09, OldProtIV09h, OldRealIV09h, NULL, NULL);
	DosSetRealProtVec(0x1B, OldProtIV1Bh, OldRealIV1Bh, NULL, NULL);
	DosSetRealProtVec(0x23, OldProtIV23h, OldRealIV23h, NULL, NULL);
	DosSetRealProtVec(0x24, OldProtIV24h, OldRealIV24h, NULL, NULL);
#else
	setvect(0x08, OldIV08h);
	if (g_cfg->CHECK_PAUSE_KEY)
		setvect(0x09, OldIV09h);
	setvect(0x1B, OldIV1Bh);
	setvect(0x23, OldIV23h);
	setvect(0x24, OldIV24h);
#endif
}

//
// this finite state machine evaluate the different states of each DTMF phone
//
void ENGINE::EvalToneState(WORD cNum, WORD bNum)
{
	// a pulse phone is active ?
	if (IsPulseFS_G(cNum, bNum, OFFHOOK))
		return ;

	switch (GetToneFS(cNum, bNum))
	{
	case LOCK      :
		DoLock    (cNum, bNum);
		break;
	case ONHOOK    :
		DoOnHook  (cNum, bNum);
		break;
	case RINGUP    :
		DoRingUp  (cNum, bNum);
		break;
	case RINGDOWN  :
		DoRingDown(cNum, bNum);
		break;
	case INCOMETALK:
		DoIncomeTalk(cNum, bNum);
		break;
	case OFFHOOK   :
		DoOffHook (cNum, bNum, TONEPHONE);
		break;
	case DTMFFLAG  :
		DoDTMFFlag(cNum, bNum);
		break;
	case INTERDIG  :
		DoInterdig(cNum, bNum, TONEPHONE);
		break;
	case ANSWER    :
		DoAnswer  (cNum, bNum, TONEPHONE);
		break;
	case TALK      :
		DoTalk    (cNum, bNum, TONEPHONE);
		break;
	case STORE     :
		DoStore   (cNum, bNum);
		break;
	case DIALERR   :
		DoDialErr (cNum, bNum);
		break;
	case COMERR    :
		DoComErr  (cNum, bNum);
		break;
		// these special case, just for TONEPHONE !!!
	case SPY	     :
		break;
	case NOPHONE   :
		break;
	case SIMULA    :
		{
			if (IsPulseFS_E(cNum, bNum, ONHOOK))
			{
				ResetData(cNum, bNum);
				ResetElapsedCount(cNum, bNum);
				SetPulseFS(cNum, bNum, LOCK); // now yes !!!
			}

			if (Clusters[cNum].NumOfDigits[bNum] == strlen(Clusters[cNum].SimulaPhones[bNum]))
			{
				if (IsToneFS_E(cNum, bNum, SIMULA)) // still here !!!
					SetToneFS(cNum, bNum, ANSWER);
			}
			else
			{
				SetCurrentDigit(cNum, bNum, Clusters[cNum].SimulaPhones[bNum][Clusters[cNum].NumOfDigits[bNum]]-'0');
				if (GetCurrentDigit(cNum, bNum) < 10)
				{ // check invalid digits
					if (MaxNumOfDigits(cNum, bNum))
					{
						if (!g_cfg->IGNORE_EXTRA_DIGITS)
							SetFSs(cNum, bNum, LOCK, DIALERR);
					}
					else
					{
						if (!IsFalseOne(cNum, bNum))
						{
							StoreCurrentDigit(cNum, bNum);
							if (IsLockable(cNum, bNum))
								SetFSs(cNum, bNum, LOCK, DIALERR);
						}
					}
				}
				ResetStateCount(cNum, bNum);
			}
			break;
		}
	}
}
//
// this finite state machine evaluate the different states of each pulse phone
//
// the break time (T_BREAK) is not useful here because the phones have different
// break/make ratios, for example, typical pulse phones have a 60/40 ratio
// but some old phones change this ratio to 50/50 for instance. GCC/gcc
//
void ENGINE::EvalPulseState(WORD cNum, WORD bNum)
{
	// a tone phone is active ?
	if (IsToneFS_G(cNum, bNum, OFFHOOK) || IsToneFS_E(cNum, bNum, LOCK))
		return ;
	switch (GetPulseFS(cNum, bNum))
	{
	case LOCK      :
		DoLock    (cNum, bNum);
		break;
	case ONHOOK    :
		DoOnHook  (cNum, bNum);
		break;
	case RINGUP    :
		DoRingUp  (cNum, bNum);
		break;
	case RINGDOWN  :
		DoRingDown(cNum, bNum);
		break;
	case INCOMETALK:
		DoIncomeTalk(cNum, bNum);
		break;
	case OFFHOOK   :
		DoOffHook (cNum, bNum, PULSEPHONE);
		break;
	case BREAK     :
		DoBreak   (cNum, bNum);
		break;
	case MAKE      :
		DoMake    (cNum, bNum);
		break;
	case INTERDIG  :
		DoInterdig(cNum, bNum, PULSEPHONE);
		break;
	case ANSWER    :
		DoAnswer  (cNum, bNum, PULSEPHONE);
		break;
	case TALK      :
		DoTalk    (cNum, bNum, PULSEPHONE);
		break;
	case STORE     :
		DoStore   (cNum, bNum);
		break;
	case DIALERR   :
		DoDialErr (cNum, bNum);
		break;
	case COMERR    :
		DoComErr  (cNum, bNum);
		break;
	case SPY	     :
		break;
	}
}
