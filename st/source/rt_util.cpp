//
// [ RT_UTIL.CPP ]
//

#include "stdst.h"

#include <ph_eng.h>
#include <rt_eng.h>
#include <dstorage.h>

extern CFG 	*g_cfg;
extern UINT m_pass;

void RT_ENGINE::CheckAnswerSignal(WORD cNum, WORD bNum, WORD phoneType)
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

BOOL RT_ENGINE::MaxNumOfDigits(WORD cNum, WORD bNum)
{
    return PH_ENGINE::IsMaxDigits(Clusters[cNum].Phones[bNum], Clusters[cNum].NumOfDigits[bNum]);
}

BOOL RT_ENGINE::IsLockable(WORD cNum, WORD bNum)
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

BOOL RT_ENGINE::IsAnswerable(WORD cNum, WORD bNum)
{
	return PH_ENGINE::IsAnswerable
	(
		Clusters[cNum].Phones[bNum],
		Clusters[cNum].NumOfDigits[bNum],
		(Clusters[cNum].PrePaid[bNum] || g_cfg->IsExtension(cNum, bNum))
	);
}

BOOL RT_ENGINE::IsBusy(void)
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

BOOL RT_ENGINE::IsBoothBusy(WORD cNum, WORD bNum)
{
	return
	(
		(IsPulseFS_G(cNum, bNum, ANSWER) && IsPulseFS_L(cNum, bNum, SPY)) ||
		(IsToneFS_G(cNum, bNum, ANSWER)  && IsToneFS_L(cNum, bNum, SPY))
	);
}

BYTE RT_ENGINE::GetCurrentDigit(WORD cNum, WORD bNum)
{
	return Clusters[cNum].CurrentDigits[bNum];
}

void RT_ENGINE::SetCurrentDigit(WORD cNum, WORD bNum, BYTE digit)
{
	Clusters[cNum].CurrentDigits[bNum] = digit;
}

void RT_ENGINE::ResetCurrentDigit(WORD cNum, WORD bNum)
{
	Clusters[cNum].CurrentDigits[bNum] = 0;
}

void RT_ENGINE::IncCurrentDigit(WORD cNum, WORD bNum)
{
	Clusters[cNum].CurrentDigits[bNum]++;
}

WORD RT_ENGINE::GetStateCount(WORD cNum, WORD bNum)
{
	return Clusters[cNum].StateCounts[bNum];
}

void RT_ENGINE::SetStateCount(WORD cNum, WORD bNum, WORD value)
{
	Clusters[cNum].StateCounts[bNum] = value;
}

void RT_ENGINE::ResetStateCount(WORD cNum, WORD bNum)
{
	Clusters[cNum].StateCounts[bNum] = 0;
}

void RT_ENGINE::IncStateCount(WORD cNum, WORD bNum)
{
	Clusters[cNum].StateCounts[bNum] += T_EVAL;
}

void RT_ENGINE::DecStateCount(WORD cNum, WORD bNum)
{
	Clusters[cNum].StateCounts[bNum] -= T_EVAL;
}

WORD RT_ENGINE::GetOnHookCount(WORD cNum, WORD bNum)
{
	return Clusters[cNum].OnHookCounts[bNum];
}

void RT_ENGINE::ResetOnHookCount(WORD cNum, WORD bNum)
{
	Clusters[cNum].OnHookCounts[bNum] = 0;
}

void RT_ENGINE::IncOnHookCount(WORD cNum, WORD bNum)
{
	Clusters[cNum].OnHookCounts[bNum] += T_EVAL;
}

BOOL RT_ENGINE::IsIncomeCall(WORD cNum, WORD bNum)
{
	return Clusters[cNum].IncomeCalls[bNum];
}

void RT_ENGINE::SetIncomeCall(WORD cNum, WORD bNum)
{
	Clusters[cNum].IncomeCalls[bNum] = TRUE;
}

void RT_ENGINE::ResetIncomeCall(WORD cNum, WORD bNum)
{
	Clusters[cNum].IncomeCalls[bNum] = FALSE;
}

WORD RT_ENGINE::GetDialCount(WORD cNum, WORD bNum)
{
	return Clusters[cNum].DialCounts[bNum];
}

void RT_ENGINE::ResetDialCount(WORD cNum, WORD bNum)
{
    Clusters[cNum].DialCounts[bNum] = 0;
}

void RT_ENGINE::IncDialCount(WORD cNum, WORD bNum)
{
	Clusters[cNum].DialCounts[bNum] += T_EVAL;
}

WORD RT_ENGINE::GetBiasCount(WORD cNum, WORD bNum)
{
    return Clusters[cNum].BiasCounts[bNum];
}

void RT_ENGINE::ResetBiasCount(WORD cNum, WORD bNum)
{
    Clusters[cNum].BiasCounts[bNum] = 0;
}

void RT_ENGINE::IncBiasCount(WORD cNum, WORD bNum)
{
    Clusters[cNum].BiasCounts[bNum] += T_EVAL;
}

DWORD RT_ENGINE::GetElapsedCount(WORD cNum, WORD bNum)
{
	return Clusters[cNum].ElapsedCounts[bNum];
}

void RT_ENGINE::SetElapsedCount(WORD cNum, WORD bNum, DWORD value)
{
    Clusters[cNum].ElapsedCounts[bNum] = value;
}

void RT_ENGINE::ResetElapsedCount(WORD cNum, WORD bNum)
{
    Clusters[cNum].ElapsedCounts[bNum] = 0;
}

void RT_ENGINE::IncElapsedCount(WORD cNum, WORD bNum)
{
    Clusters[cNum].ElapsedCounts[bNum] += T_EVAL;
}

// 2.21.1
DWORD RT_ENGINE::GetFinalElapsedCount(WORD cNum, WORD bNum)
{
	return Clusters[cNum].FinalElapsedCounts[bNum];
}

void RT_ENGINE::SetFinalElapsedCount(WORD cNum, WORD bNum, DWORD value)
{
	Clusters[cNum].FinalElapsedCounts[bNum] = value;
}

void RT_ENGINE::ResetFinalElapsedCount(WORD cNum, WORD bNum)
{
	Clusters[cNum].FinalElapsedCounts[bNum] = 0;
}

void RT_ENGINE::SetFSs(WORD cNum, WORD bNum, WORD pulseState, WORD toneState)
{
	Clusters[cNum].PulseFSs[bNum] = pulseState;
	Clusters[cNum].ToneFSs[bNum]  = toneState;
}

WORD RT_ENGINE::GetPulseFS(WORD cNum, WORD bNum)
{
    return Clusters[cNum].PulseFSs[bNum];
}

void RT_ENGINE::SetPulseFS(WORD cNum, WORD bNum, WORD state)
{
    Clusters[cNum].PulseFSs[bNum] = state;
}

BOOL RT_ENGINE::IsPulseFS_E(WORD cNum, WORD bNum, WORD state)
{
    return (Clusters[cNum].PulseFSs[bNum] == state);
}

BOOL RT_ENGINE::IsPulseFS_L (WORD cNum, WORD bNum, WORD state)
{
    return (Clusters[cNum].PulseFSs[bNum] < state);
}

BOOL RT_ENGINE::IsPulseFS_LE(WORD cNum, WORD bNum, WORD state)
{
    return (Clusters[cNum].PulseFSs[bNum] <= state);
}

BOOL RT_ENGINE::IsPulseFS_G (WORD cNum, WORD bNum, WORD state)
{
    return (Clusters[cNum].PulseFSs[bNum] > state);
}

BOOL RT_ENGINE::IsPulseFS_GE(WORD cNum, WORD bNum, WORD state)
{
    return (Clusters[cNum].PulseFSs[bNum] >= state);
}

WORD RT_ENGINE::GetToneFS(WORD cNum, WORD bNum)
{
    return Clusters[cNum].ToneFSs[bNum];
}

void RT_ENGINE::SetToneFS(WORD cNum, WORD bNum, WORD state)
{
    Clusters[cNum].ToneFSs[bNum] = state;
}

BOOL RT_ENGINE::IsToneFS_E (WORD cNum, WORD bNum, WORD state)
{
    return (Clusters[cNum].ToneFSs[bNum] == state);
}

BOOL RT_ENGINE::IsToneFS_L (WORD cNum, WORD bNum, WORD state)
{
    return (Clusters[cNum].ToneFSs[bNum] < state);
}

BOOL RT_ENGINE::IsToneFS_LE(WORD cNum, WORD bNum, WORD state)
{
    return (Clusters[cNum].ToneFSs[bNum] <= state);
}

BOOL RT_ENGINE::IsToneFS_G (WORD cNum, WORD bNum, WORD state)
{
    return (Clusters[cNum].ToneFSs[bNum] > state);
}

BOOL RT_ENGINE::IsToneFS_GE(WORD cNum, WORD bNum, WORD state)
{
    return (Clusters[cNum].ToneFSs[bNum] >= state);
}

BOOL RT_ENGINE::IsFalseOne(WORD cNum, WORD bNum)
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

BOOL RT_ENGINE::GetPrePaid(WORD cNum, WORD bNum)
{
	return Clusters[cNum].PrePaid[bNum];
}

void RT_ENGINE::SetPrePaid(WORD cNum, WORD bNum, BOOL value)
{
	Clusters[cNum].PrePaid[bNum] = value;
}

BOOL RT_ENGINE::GetFirstPreValue(WORD cNum, WORD bNum)
{
	return Clusters[cNum].FirstPreValue[bNum];
}

void RT_ENGINE::SetFirstPreValue(WORD cNum, WORD bNum, BOOL value)
{
	Clusters[cNum].FirstPreValue[bNum] = value;
}

double RT_ENGINE::GetPreValue(WORD cNum, WORD bNum)
{
    return Clusters[cNum].PreValue[bNum];
}

void RT_ENGINE::SetPreValue(WORD cNum, WORD bNum, double value)
{
    // fix:
    //   Clusters[cNum].PreValue[bNum] = value;
    //   Non float math -> non interrupt (emulation)
    memcpy(&Clusters[cNum].PreValue[bNum], &value, sizeof(double));
}

DWORD RT_ENGINE::GetPreTime(WORD cNum, WORD bNum)
{
	return Clusters[cNum].PreTime[bNum];
}

void RT_ENGINE::SetPreTime(WORD cNum, WORD bNum, DWORD value)
{
	Clusters[cNum].PreTime[bNum] = value;
}

WORD RT_ENGINE::GetNumOfCalls(WORD cNum, WORD bNum) const
{
	return Clusters[cNum].NumOfCalls[bNum];
}

void RT_ENGINE::SetNumOfCalls(WORD cNum, WORD bNum, WORD numOfCalls)
{
	Clusters[cNum].NumOfCalls[bNum] = numOfCalls;
}

WORD RT_ENGINE::GetUnifiedState(WORD cNum, WORD bNum)
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

void RT_ENGINE::GetDumpData(void * & ptr, int &size)
{
	 ptr  = Clusters;
	 size = sizeof(BoothCluster) * g_cfg->ACTIVE_CLUSTERS;
}

void RT_ENGINE::SetDumpData(void * ptr, int size)
{
	memcpy(Clusters, ptr, size);
}

BOOL RT_ENGINE::GetReceipt(DynamicReceipt & dynReceipt)
{
	RTReceiptQueueMutex mutex;
	return Receipts->Get(dynReceipt);
}

void RT_ENGINE::GetClusters(BoothCluster clusters[])
{
	RTBoothClustersMutex mutex;
	memcpy(clusters, Clusters, sizeof(BoothCluster) * g_cfg->ACTIVE_CLUSTERS);
}

BYTE RT_ENGINE::GetGeneralPort() const
{
	return GeneralPort;
}

void RT_ENGINE::SetGeneralPort(BYTE generalPort)
{
	GeneralPort = generalPort;
}

BYTE RT_ENGINE::GetDataPortSpy(WORD cNum) const
{
	return Clusters[cNum].DataPort.Spy;
}

void RT_ENGINE::SetDataPortSpy(WORD cNum, BYTE value)
{
	Clusters[cNum].DataPort.Spy = value;
}

BoothCluster::_DataPort & RT_ENGINE::GetDataPort(WORD cNum)
{
	return Clusters[cNum].DataPort;
}

void RT_ENGINE::StoreCurrentDigit(WORD cNum, WORD bNum)
{
	// store digit, and adjust ASCII string (first adjust)
	Clusters[cNum].Phones[bNum][Clusters[cNum].NumOfDigits[bNum]+1] = '\0';
	Clusters[cNum].Phones[bNum][Clusters[cNum].NumOfDigits[bNum]]   = '0'+GetCurrentDigit(cNum, bNum);
	Clusters[cNum].NumOfDigits[bNum]++; // a new digit
}

void RT_ENGINE::GetPhone(WORD cNum, WORD bNum, PHONE & phone) const
{
	strcpy((char *)&phone, (char *)&Clusters[cNum].Phones[bNum]);
}

void RT_ENGINE::ResetPhone(WORD cNum, WORD bNum)
{
	memset(&Clusters[cNum].Phones[bNum], 0, sizeof(PHONE));
}

BOOL RT_ENGINE::GetFound(WORD cNum, WORD bNum) const
{
	return Clusters[cNum].Found[bNum];
}

void RT_ENGINE::SetFound(WORD cNum, WORD bNum, BOOL value) const
{
	Clusters[cNum].Found[bNum] = value;
}

WORD RT_ENGINE::GetCallAttr(WORD cNum, WORD bNum) const
{
	return Clusters[cNum].CallAttrs[bNum];
}

void RT_ENGINE::SetCallAttr(WORD cNum, WORD bNum, WORD attr)
{
	Clusters[cNum].CallAttrs[bNum] = attr;
}

WORD RT_ENGINE::GetTariff(WORD cNum, WORD bNum) const
{
	return Clusters[cNum].Tariffs[bNum];
}

void RT_ENGINE::SetTariff(WORD cNum, WORD bNum, WORD tariff)
{
	Clusters[cNum].Tariffs[bNum] = tariff;
}

int RT_ENGINE::GetStartTime(WORD cNum, WORD bNum)
{
	return Clusters[cNum].StartTimes[bNum];
}

void RT_ENGINE::SetStartTime(WORD cNum, WORD bNum, int time)
{
	Clusters[cNum].StartTimes[bNum] = time;
}

int RT_ENGINE::GetStartDate(WORD cNum, WORD bNum)
{
	return Clusters[cNum].StartDates[bNum];
}

void RT_ENGINE::SetStartDate(WORD cNum, WORD bNum, int date)
{
	Clusters[cNum].StartDates[bNum] = date;
}

BOOL RT_ENGINE::GetLocked(WORD cNum, WORD bNum) const
{
	return Clusters[cNum].Locked[bNum];
}

void RT_ENGINE::SetLocked(WORD cNum, WORD bNum, BOOL locked)
{
	Clusters[cNum].Locked[bNum] = locked;
}

SHORT RT_ENGINE::GetLastSpyBooth() const
{
	return LastSpyBooth;
}

void RT_ENGINE::SetLastSpyBooth(WORD booth)
{
	LastSpyBooth = booth;
}

void RT_ENGINE::SaveLastSpyBooth()
{
	LastSpyBooth = SpyBooth;
}

SHORT RT_ENGINE::GetSpyBooth() const
{
	return SpyBooth;
}

void RT_ENGINE::SetSpyBooth(WORD booth)
{
	SpyBooth = booth;
}

int RT_ENGINE::GetCurrentDate()
{
	return CurrentDate;
}

void RT_ENGINE::SetCurrentDate(int date)
{
	CurrentDate = date;
}

int RT_ENGINE::GetCurrentTime()
{
	return CurrentTime;
}

void RT_ENGINE::SetCurrentTime(int time)
{
	CurrentTime = time;
}

SHORT RT_ENGINE::GetNotIncBooth() const
{
	return NotIncBooth;
}

void RT_ENGINE::SetNotIncBooth(SHORT booth)
{
	NotIncBooth = booth;
}

SHORT RT_ENGINE::GetBadInterBooth() const
{
	return BadInterBooth;
}

void RT_ENGINE::SetBadInterBooth(SHORT booth)
{
	BadInterBooth = booth;
}

SHORT RT_ENGINE::GetComErrBooth() const
{
	return ComErrBooth;
}

void RT_ENGINE::SetComErrBooth(SHORT booth)
{
	ComErrBooth = booth;
}

SHORT RT_ENGINE::GetDialErrBooth() const
{
	return DialErrBooth;
}

void RT_ENGINE::SetDialErrBooth(SHORT booth)
{
	DialErrBooth = booth;
}

BOOL RT_ENGINE::GetSimula(WORD cNum, WORD bNum) const
{
	return Clusters[cNum].Simula[bNum];
}

void RT_ENGINE::SetSimula(WORD cNum, WORD bNum, BOOL value) const
{
	Clusters[cNum].Simula[bNum] = value;
}

void RT_ENGINE::GetSimulaPhone(WORD cNum, WORD bNum, PHONE & phone) const
{
	strcpy((char *)&phone, (char *)&Clusters[cNum].SimulaPhones[bNum]);
}

void RT_ENGINE::SetSimulaPhone(WORD cNum, WORD bNum, PHONE const & phone)
{
	strcpy((char *)&Clusters[cNum].SimulaPhones[bNum], (char *)&phone);
}

void RT_ENGINE::ResetSimulaPhone(WORD cNum, WORD bNum)
{
	strcpy((char *)&Clusters[cNum].SimulaPhones[bNum], "");
}

BOOL RT_ENGINE::GetNoReceipt(WORD cNum, WORD bNum) const
{
	return Clusters[cNum].NoReceipt[bNum];
}

void RT_ENGINE::SetNoReceipt(WORD cNum, WORD bNum, BOOL value) const
{
	Clusters[cNum].NoReceipt[bNum] = value;
}

BOOL RT_ENGINE::GetNoStatistics(WORD cNum, WORD bNum) const
{
	return Clusters[cNum].NoStatistics[bNum];
}

void RT_ENGINE::SetNoStatistics(WORD cNum, WORD bNum, BOOL value) const
{
	Clusters[cNum].NoStatistics[bNum] = value;
}
