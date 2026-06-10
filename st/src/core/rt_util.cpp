//
// [ RT_UTIL.CPP ]
//

#include "stdst.h"

#include <ph_eng.h>
#include <rt_eng.h>
#include <dstorage.h>

extern CFG 	*g_cfg;
extern UINT m_pass;

void ENGINE::CheckAnswerSignal(WORD cNum, WORD bNum, WORD phoneType)
{
	const WORD T_BIAS_MARGIN = 30; // margen para inversion de polaridad -> T_BIAS
	switch (g_cfg->ASIGNAL)
    {
    case CFG::S_BIAS:
        {
			if (BIT(Clusters[cNum].DataPort.Answer, bNum))
            {
				// check to see if the number of digits is correct ???
                if (!IsAnswerable(cNum, bNum))
				{
					if (!g_cfg->IGNORE_PRE_ANSWER)
                        SetFSs(cNum, bNum, LOCK, COMERR); // tone has an upper priority
                    return ;
                }
                IncBiasCount(cNum, bNum);
				if (GetBiasCount(cNum, bNum) >= (g_cfg->T_BIAS-T_BIAS_MARGIN))
                {
                    ResetStateCount(cNum, bNum);
                    if (phoneType == TONEPHONE)
                        SetToneFS(cNum, bNum, ANSWER);
                    else
                        SetPulseFS(cNum, bNum, ANSWER);
                }
            }
			else
            {
				// the signal is missed then lock the phone
                ResetBiasCount(cNum, bNum); // avoid false pulse
				if (GetStateCount(cNum, bNum) >= g_cfg->T_ANSWER)
				{
					SetFSs(cNum, bNum, LOCK, COMERR); // tone has an upper priority
					return ;
				}
			}
			break;
		}
	case CFG::S_TONE:
	case CFG::S_THREAD:
		{
			// Warning: We use T_BIAS for thread C
			if (!BIT(Clusters[cNum].DataPort.ThreadC, bNum))
			{
				// check to see if the number of digits is correct ???
				if (!IsAnswerable(cNum, bNum))
				{
					SetFSs(cNum, bNum, LOCK, COMERR); // tone has an upper priority
					return ;
				}
				IncBiasCount(cNum, bNum);
				if (GetBiasCount(cNum, bNum) >= (g_cfg->T_BIAS-T_BIAS_MARGIN))
				{
					ResetStateCount(cNum, bNum);
					if (phoneType == TONEPHONE)
						SetToneFS(cNum, bNum, ANSWER);
					else
						SetPulseFS(cNum, bNum, ANSWER);
				}
			}
			else
			{
				// the signal is missed then lock the phone
				if (GetStateCount(cNum, bNum) >= g_cfg->T_ANSWER)
				{
					SetFSs(cNum, bNum, LOCK, COMERR); // tone has an upper priority
					return ;
				}
			}
			break;
		}
	case CFG::S_TIME:
		{
			if (GetStateCount(cNum, bNum) >= g_cfg->T_COM)
			{
				ResetStateCount(cNum, bNum);
				if (!IsAnswerable(cNum, bNum))
				{
					SetFSs(cNum, bNum, LOCK, COMERR); // tone has an upper priority
					return ;
                }
                if (phoneType == TONEPHONE)
                    SetToneFS(cNum, bNum, ANSWER);
                else
                    SetPulseFS(cNum, bNum, ANSWER);
			}
            break;
        }
    }
}

BOOL ENGINE::MaxNumOfDigits(WORD cNum, WORD bNum)
{
    return PH_ENGINE::IsMaxDigits(Clusters[cNum].Phones[bNum], Clusters[cNum].NumOfDigits[bNum]);
}

BOOL ENGINE::IsLockable(WORD cNum, WORD bNum)
{
	static BOOL locked;
	locked = FALSE;

	// for prepaid or for extensions deactivate locking
	if (!Clusters[cNum].PrePaid[bNum] && !g_cfg->IsExtension(cNum, bNum))
	{
		locked = PH_ENGINE::IsLockable(Clusters[cNum].Phones[bNum], Clusters[cNum].NumOfDigits[bNum]);
		if (locked)
		{
			// special check to trigger an alarm with INTERNATIONAL calls
			static CALL_ATTR attr;
			if (PH_ENGINE::GetCallAttr(Clusters[cNum].Phones[bNum], attr, Clusters[cNum].NumOfDigits[bNum]))
			{
#if !defined(__EDA__)
				if (attr == DDI_CALL)
				{
#else
				if (attr == DDI_EDA2TEL_CALL)
				{
#endif
					BadInterBooth = cNum*CLUSTER_SIZE+bNum; // signalize an error
				}
			}
		}
	}
	return locked;
}

BOOL ENGINE::IsAnswerable(WORD cNum, WORD bNum)
{
	return PH_ENGINE::IsAnswerable
	(
		Clusters[cNum].Phones[bNum],
		Clusters[cNum].NumOfDigits[bNum],
		(Clusters[cNum].PrePaid[bNum] || g_cfg->IsExtension(cNum, bNum))
	);
}

BOOL ENGINE::IsBusy(void)
{
	WORD cNum, bNum;
	for (cNum=0; cNum<g_cfg->ACTIVE_CLUSTERS; cNum++) // 2.30
	{
		for (bNum=0; bNum<CLUSTER_SIZE; bNum++)
		{
			// 2.21.8 check get number of calls in manual mode
			if
			(
				pThis->IsBoothBusy(cNum, bNum) ||
				pThis->GetNumOfCalls(cNum, bNum))
			{
				return TRUE;
			}
        }
	}

    return FALSE; // still here !!!
}

BOOL ENGINE::IsBoothBusy(WORD cNum, WORD bNum)
{
	return
	(
		(IsPulseFS_G(cNum, bNum, ANSWER) && IsPulseFS_L(cNum, bNum, SPY)) ||
		(IsToneFS_G(cNum, bNum, ANSWER)  && IsToneFS_L(cNum, bNum, SPY))
	);
}

BYTE ENGINE::GetCurrentDigit(WORD cNum, WORD bNum)
{
	return Clusters[cNum].CurrentDigits[bNum];
}

void ENGINE::SetCurrentDigit(WORD cNum, WORD bNum, BYTE digit)
{
	Clusters[cNum].CurrentDigits[bNum] = digit;
}

void ENGINE::ResetCurrentDigit(WORD cNum, WORD bNum)
{
	Clusters[cNum].CurrentDigits[bNum] = 0;
}

void ENGINE::IncCurrentDigit(WORD cNum, WORD bNum)
{
	Clusters[cNum].CurrentDigits[bNum]++;
}

WORD ENGINE::GetStateCount(WORD cNum, WORD bNum)
{
	return Clusters[cNum].StateCounts[bNum];
}

void ENGINE::SetStateCount(WORD cNum, WORD bNum, WORD value)
{
	Clusters[cNum].StateCounts[bNum] = value;
}

void ENGINE::ResetStateCount(WORD cNum, WORD bNum)
{
	Clusters[cNum].StateCounts[bNum] = 0;
}

void ENGINE::IncStateCount(WORD cNum, WORD bNum)
{
	Clusters[cNum].StateCounts[bNum] += T_EVAL;
}

void ENGINE::DecStateCount(WORD cNum, WORD bNum)
{
	Clusters[cNum].StateCounts[bNum] -= T_EVAL;
}

WORD ENGINE::GetOnHookCount(WORD cNum, WORD bNum)
{
	return Clusters[cNum].OnHookCounts[bNum];
}

void ENGINE::ResetOnHookCount(WORD cNum, WORD bNum)
{
	Clusters[cNum].OnHookCounts[bNum] = 0;
}

void ENGINE::IncOnHookCount(WORD cNum, WORD bNum)
{
	Clusters[cNum].OnHookCounts[bNum] += T_EVAL;
}

BOOL ENGINE::IsIncomeCall(WORD cNum, WORD bNum)
{
	return Clusters[cNum].IncomeCalls[bNum];
}

void ENGINE::SetIncomeCall(WORD cNum, WORD bNum)
{
	Clusters[cNum].IncomeCalls[bNum] = TRUE;
}

void ENGINE::ResetIncomeCall(WORD cNum, WORD bNum)
{
	Clusters[cNum].IncomeCalls[bNum] = FALSE;
}

WORD ENGINE::GetDialCount(WORD cNum, WORD bNum)
{
	return Clusters[cNum].DialCounts[bNum];
}

void ENGINE::ResetDialCount(WORD cNum, WORD bNum)
{
    Clusters[cNum].DialCounts[bNum] = 0;
}

void ENGINE::IncDialCount(WORD cNum, WORD bNum)
{
	Clusters[cNum].DialCounts[bNum] += T_EVAL;
}

WORD ENGINE::GetBiasCount(WORD cNum, WORD bNum)
{
    return Clusters[cNum].BiasCounts[bNum];
}

void ENGINE::ResetBiasCount(WORD cNum, WORD bNum)
{
    Clusters[cNum].BiasCounts[bNum] = 0;
}

void ENGINE::IncBiasCount(WORD cNum, WORD bNum)
{
    Clusters[cNum].BiasCounts[bNum] += T_EVAL;
}

DWORD ENGINE::GetElapsedCount(WORD cNum, WORD bNum)
{
	return Clusters[cNum].ElapsedCounts[bNum];
}

void ENGINE::SetElapsedCount(WORD cNum, WORD bNum, DWORD value)
{
    Clusters[cNum].ElapsedCounts[bNum] = value;
}

void ENGINE::ResetElapsedCount(WORD cNum, WORD bNum)
{
    Clusters[cNum].ElapsedCounts[bNum] = 0;
}

void ENGINE::IncElapsedCount(WORD cNum, WORD bNum)
{
    Clusters[cNum].ElapsedCounts[bNum] += T_EVAL;
}

// 2.21.1
DWORD ENGINE::GetFinalElapsedCount(WORD cNum, WORD bNum)
{
	return Clusters[cNum].FinalElapsedCounts[bNum];
}

void ENGINE::SetFinalElapsedCount(WORD cNum, WORD bNum, DWORD value)
{
	Clusters[cNum].FinalElapsedCounts[bNum] = value;
}

void ENGINE::ResetFinalElapsedCount(WORD cNum, WORD bNum)
{
	Clusters[cNum].FinalElapsedCounts[bNum] = 0;
}

void ENGINE::SetFSs(WORD cNum, WORD bNum, WORD pulseState, WORD toneState)
{
	Clusters[cNum].PulseFSs[bNum] = pulseState;
	Clusters[cNum].ToneFSs[bNum]  = toneState;
}

WORD ENGINE::GetPulseFS(WORD cNum, WORD bNum)
{
    return Clusters[cNum].PulseFSs[bNum];
}

void ENGINE::SetPulseFS(WORD cNum, WORD bNum, WORD state)
{
    Clusters[cNum].PulseFSs[bNum] = state;
}

BOOL ENGINE::IsPulseFS_E(WORD cNum, WORD bNum, WORD state)
{
    return (Clusters[cNum].PulseFSs[bNum] == state);
}

BOOL ENGINE::IsPulseFS_L (WORD cNum, WORD bNum, WORD state)
{
    return (Clusters[cNum].PulseFSs[bNum] < state);
}

BOOL ENGINE::IsPulseFS_LE(WORD cNum, WORD bNum, WORD state)
{
    return (Clusters[cNum].PulseFSs[bNum] <= state);
}

BOOL ENGINE::IsPulseFS_G (WORD cNum, WORD bNum, WORD state)
{
    return (Clusters[cNum].PulseFSs[bNum] > state);
}

BOOL ENGINE::IsPulseFS_GE(WORD cNum, WORD bNum, WORD state)
{
    return (Clusters[cNum].PulseFSs[bNum] >= state);
}

WORD ENGINE::GetToneFS(WORD cNum, WORD bNum)
{
    return Clusters[cNum].ToneFSs[bNum];
}

void ENGINE::SetToneFS(WORD cNum, WORD bNum, WORD state)
{
    Clusters[cNum].ToneFSs[bNum] = state;
}

BOOL ENGINE::IsToneFS_E (WORD cNum, WORD bNum, WORD state)
{
    return (Clusters[cNum].ToneFSs[bNum] == state);
}

BOOL ENGINE::IsToneFS_L (WORD cNum, WORD bNum, WORD state)
{
    return (Clusters[cNum].ToneFSs[bNum] < state);
}

BOOL ENGINE::IsToneFS_LE(WORD cNum, WORD bNum, WORD state)
{
    return (Clusters[cNum].ToneFSs[bNum] <= state);
}

BOOL ENGINE::IsToneFS_G (WORD cNum, WORD bNum, WORD state)
{
    return (Clusters[cNum].ToneFSs[bNum] > state);
}

BOOL ENGINE::IsToneFS_GE(WORD cNum, WORD bNum, WORD state)
{
    return (Clusters[cNum].ToneFSs[bNum] >= state);
}

BOOL ENGINE::IsFalseOne(WORD cNum, WORD bNum)
{
	if
	(
		Clusters[cNum].NumOfDigits[bNum] == 0 && // first digit
		GetCurrentDigit(cNum, bNum) == 1      &&
		g_cfg->EXIST_FALSE_ONE                 &&
		!Clusters[cNum].FirstOnes[bNum]
	)
		return (Clusters[cNum].FirstOnes[bNum] = TRUE);
	else
		return FALSE;
}

BOOL ENGINE::GetPrePaid(WORD cNum, WORD bNum)
{
	return Clusters[cNum].PrePaid[bNum];
}

void ENGINE::SetPrePaid(WORD cNum, WORD bNum, BOOL value)
{
	Clusters[cNum].PrePaid[bNum] = value;
}

BOOL ENGINE::GetFirstPreValue(WORD cNum, WORD bNum)
{
	return Clusters[cNum].FirstPreValue[bNum];
}

void ENGINE::SetFirstPreValue(WORD cNum, WORD bNum, BOOL value)
{
	Clusters[cNum].FirstPreValue[bNum] = value;
}

double ENGINE::GetPreValue(WORD cNum, WORD bNum)
{
    return Clusters[cNum].PreValue[bNum];
}

void ENGINE::SetPreValue(WORD cNum, WORD bNum, double value)
{
    // fix:
    //   Clusters[cNum].PreValue[bNum] = value;
    //   Non float math -> non interrupt (emulation)
    memcpy(&Clusters[cNum].PreValue[bNum], &value, sizeof(double));
}

DWORD ENGINE::GetPreTime(WORD cNum, WORD bNum)
{
	return Clusters[cNum].PreTime[bNum];
}

void ENGINE::SetPreTime(WORD cNum, WORD bNum, DWORD value)
{
	Clusters[cNum].PreTime[bNum] = value;
}

WORD ENGINE::GetNumOfCalls(WORD cNum, WORD bNum) const
{
	return Clusters[cNum].NumOfCalls[bNum];
}

void ENGINE::SetNumOfCalls(WORD cNum, WORD bNum, WORD numOfCalls)
{
	Clusters[cNum].NumOfCalls[bNum] = numOfCalls;
}

WORD ENGINE::GetUnifiedState(WORD cNum, WORD bNum)
{
	// Unify pulse and tone states
	WORD state = GetToneFS(cNum, bNum); // tone has a high priority

	WORD boothCount = cNum*CLUSTER_SIZE+bNum;
	if (boothCount == SpyBooth)
	{
		// This is a special case is only for SPY because we need to Refresh
		// the icon, but the RTEngine continues to work without knowing it
		return SPY;
	}

	if (state > OFFHOOK)
	{
		// tone
		if
		(
			state == DTMFFLAG ||
			state == INTERDIG ||
			state == ANSWER)
		{
			state = NAL;
			if (Clusters[cNum].CallAttrs[bNum] & INTERNATIONAL_CALL_MASK)
			{
				state = INTER;
			}
		}
	}
	else
	{
		// pulse
		state = GetPulseFS(cNum, bNum);
		if (state == BREAK    ||
			state == MAKE     ||
			state == INTERDIG ||
			state == ANSWER)
		{
			state = NAL;
			if (Clusters[cNum].CallAttrs[bNum] & INTERNATIONAL_CALL_MASK)
			{
				state = INTER;
			}
		}
	}

	return state;
}

void ENGINE::GetDumpData(void * & ptr, int &size)
{
	 ptr  = Clusters;
	 size = sizeof(BoothCluster) * g_cfg->ACTIVE_CLUSTERS;
}

void ENGINE::SetDumpData(void * ptr, int size)
{
	memcpy(Clusters, ptr, size);
}

BOOL ENGINE::GetReceipt(DynamicReceipt & dynReceipt)
{
	RTReceiptQueueMutex mutex;
	return Receipts->Get(dynReceipt);
}

void ENGINE::GetClusters(BoothCluster clusters[])
{
	RTBoothClustersMutex mutex;
	memcpy(clusters, Clusters, sizeof(BoothCluster) * g_cfg->ACTIVE_CLUSTERS);
}

BYTE ENGINE::GetGeneralPort() const
{
	return GeneralPort;
}

void ENGINE::SetGeneralPort(BYTE generalPort)
{
	GeneralPort = generalPort;
}

BYTE ENGINE::GetDataPortSpy(WORD cNum) const
{
	return Clusters[cNum].DataPort.Spy;
}

void ENGINE::SetDataPortSpy(WORD cNum, BYTE value)
{
	Clusters[cNum].DataPort.Spy = value;
}

BoothCluster::_DataPort & ENGINE::GetDataPort(WORD cNum)
{
	return Clusters[cNum].DataPort;
}

void ENGINE::StoreCurrentDigit(WORD cNum, WORD bNum)
{
	// store digit, and adjust ASCII string (first adjust)
	Clusters[cNum].Phones[bNum][Clusters[cNum].NumOfDigits[bNum]+1] = '\0';
	Clusters[cNum].Phones[bNum][Clusters[cNum].NumOfDigits[bNum]]   = '0'+GetCurrentDigit(cNum, bNum);
	Clusters[cNum].NumOfDigits[bNum]++; // a new digit
}

void ENGINE::GetPhone(WORD cNum, WORD bNum, PHONE & phone) const
{
	strcpy((char *)&phone, (char *)&Clusters[cNum].Phones[bNum]);
}

void ENGINE::ResetPhone(WORD cNum, WORD bNum)
{
	memset(&Clusters[cNum].Phones[bNum], 0, sizeof(PHONE));
}

BOOL ENGINE::GetFound(WORD cNum, WORD bNum) const
{
	return Clusters[cNum].Found[bNum];
}

void ENGINE::SetFound(WORD cNum, WORD bNum, BOOL value) const
{
	Clusters[cNum].Found[bNum] = value;
}

WORD ENGINE::GetCallAttr(WORD cNum, WORD bNum) const
{
	return Clusters[cNum].CallAttrs[bNum];
}

void ENGINE::SetCallAttr(WORD cNum, WORD bNum, WORD attr)
{
	Clusters[cNum].CallAttrs[bNum] = attr;
}

WORD ENGINE::GetTariff(WORD cNum, WORD bNum) const
{
	return Clusters[cNum].Tariffs[bNum];
}

void ENGINE::SetTariff(WORD cNum, WORD bNum, WORD tariff)
{
	Clusters[cNum].Tariffs[bNum] = tariff;
}

int ENGINE::GetStartTime(WORD cNum, WORD bNum)
{
	return Clusters[cNum].StartTimes[bNum];
}

void ENGINE::SetStartTime(WORD cNum, WORD bNum, int time)
{
	Clusters[cNum].StartTimes[bNum] = time;
}

int ENGINE::GetStartDate(WORD cNum, WORD bNum)
{
	return Clusters[cNum].StartDates[bNum];
}

void ENGINE::SetStartDate(WORD cNum, WORD bNum, int date)
{
	Clusters[cNum].StartDates[bNum] = date;
}

BOOL ENGINE::GetLocked(WORD cNum, WORD bNum) const
{
	return Clusters[cNum].Locked[bNum];
}

void ENGINE::SetLocked(WORD cNum, WORD bNum, BOOL locked)
{
	Clusters[cNum].Locked[bNum] = locked;
}

SHORT ENGINE::GetLastSpyBooth() const
{
	return LastSpyBooth;
}

void ENGINE::SetLastSpyBooth(WORD booth)
{
	LastSpyBooth = booth;
}

void ENGINE::SaveLastSpyBooth()
{
	LastSpyBooth = SpyBooth;
}

SHORT ENGINE::GetSpyBooth() const
{
	return SpyBooth;
}

void ENGINE::SetSpyBooth(WORD booth)
{
	SpyBooth = booth;
}

int ENGINE::GetCurrentDate()
{
	return CurrentDate;
}

void ENGINE::SetCurrentDate(int date)
{
	CurrentDate = date;
}

int ENGINE::GetCurrentTime()
{
	return CurrentTime;
}

void ENGINE::SetCurrentTime(int time)
{
	CurrentTime = time;
}

SHORT ENGINE::GetNotIncBooth() const
{
	return NotIncBooth;
}

void ENGINE::SetNotIncBooth(SHORT booth)
{
	NotIncBooth = booth;
}

SHORT ENGINE::GetBadInterBooth() const
{
	return BadInterBooth;
}

void ENGINE::SetBadInterBooth(SHORT booth)
{
	BadInterBooth = booth;
}

SHORT ENGINE::GetComErrBooth() const
{
	return ComErrBooth;
}

void ENGINE::SetComErrBooth(SHORT booth)
{
	ComErrBooth = booth;
}

SHORT ENGINE::GetDialErrBooth() const
{
	return DialErrBooth;
}

void ENGINE::SetDialErrBooth(SHORT booth)
{
	DialErrBooth = booth;
}

BOOL ENGINE::GetSimula(WORD cNum, WORD bNum) const
{
	return Clusters[cNum].Simula[bNum];
}

void ENGINE::SetSimula(WORD cNum, WORD bNum, BOOL value) const
{
	Clusters[cNum].Simula[bNum] = value;
}

void ENGINE::GetSimulaPhone(WORD cNum, WORD bNum, PHONE & phone) const
{
	strcpy((char *)&phone, (char *)&Clusters[cNum].SimulaPhones[bNum]);
}

void ENGINE::SetSimulaPhone(WORD cNum, WORD bNum, PHONE const & phone)
{
	strcpy((char *)&Clusters[cNum].SimulaPhones[bNum], (char *)&phone);
}

void ENGINE::ResetSimulaPhone(WORD cNum, WORD bNum)
{
	strcpy((char *)&Clusters[cNum].SimulaPhones[bNum], "");
}

BOOL ENGINE::GetNoReceipt(WORD cNum, WORD bNum) const
{
	return Clusters[cNum].NoReceipt[bNum];
}

void ENGINE::SetNoReceipt(WORD cNum, WORD bNum, BOOL value) const
{
	Clusters[cNum].NoReceipt[bNum] = value;
}

BOOL ENGINE::GetNoStatistics(WORD cNum, WORD bNum) const
{
	return Clusters[cNum].NoStatistics[bNum];
}

void ENGINE::SetNoStatistics(WORD cNum, WORD bNum, BOOL value) const
{
	Clusters[cNum].NoStatistics[bNum] = value;
}
