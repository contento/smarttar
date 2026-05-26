//
// [ RT_DO.CPP ]
//

#include "stdst.h"

#include <rt_eng.h>

extern CFG 	*g_cfg;
extern UINT g_pass;

//
// --- DO's ------------------------------------------------------------------
//
void ENGINE::DoLock(WORD cNum, WORD bNum)
{
    // ST must always lock the phone before ONHOOK.
    // the above avoid false hangs
    Clusters[cNum].DataPort.Lock |= (1 << bNum);
	if (GetStateCount(cNum, bNum) >= g_cfg->T_LOCK)
	{
		ResetIncomeCall(cNum, bNum);
		ResetElapsedCount(cNum, bNum);
		if (!Clusters[cNum].Locked[bNum])
		{ // is locked for the operator ?
			// unlock the booth
			Clusters[cNum].DataPort.Lock &= ~(1 << bNum);
			SetFSs(cNum, bNum, ONHOOK, ONHOOK);
		}
	}
}

void ENGINE::DoOnHook(WORD cNum, WORD bNum)
{
	if (BIT(Clusters[cNum].DataPort.OOD, bNum))
    {
		ResetStateCount(cNum, bNum);
		ResetOnHookCount(cNum, bNum);
		ResetBiasCount(cNum, bNum);
		if (g_cfg->DETECT_INCOME)
			SetFSs(cNum, bNum, RINGUP, RINGUP);
		else
			SetFSs(cNum, bNum, OFFHOOK, OFFHOOK);
	}
	else
	{
		if (g_cfg->DETECT_INCOME && IsIncomeCall(cNum, bNum))
			if (GetStateCount(cNum, bNum) >= g_cfg->T_INTER_RING)
				SetToneFS(cNum, bNum, LOCK);
	}
}

void ENGINE::DoRingUp(WORD cNum, WORD bNum)
{
    if (!BIT(Clusters[cNum].DataPort.OOD, bNum))
    {
        ResetStateCount(cNum, bNum);
        ResetOnHookCount(cNum, bNum);
        ResetBiasCount(cNum, bNum);
        SetIncomeCall(cNum, bNum);
        SetFSs(cNum, bNum, RINGDOWN, RINGDOWN);
    }
    else
    {
		if (GetStateCount(cNum, bNum) >= g_cfg->T_OFF_HOOK)
        {
            // not to reset state count !!!
			if (g_cfg->DETECT_INCOME && IsIncomeCall(cNum, bNum))
                SetFSs(cNum, bNum, INCOMETALK, INCOMETALK);
            else
                SetFSs(cNum, bNum, OFFHOOK, OFFHOOK);
        }
    }
}

void ENGINE::DoRingDown(WORD cNum, WORD bNum)
{
    if (BIT(Clusters[cNum].DataPort.OOD, bNum))
    {
        ResetStateCount(cNum, bNum);
        ResetOnHookCount(cNum, bNum);
        SetFSs(cNum, bNum, RINGUP, RINGUP);
    }
    else
    {
        IncOnHookCount(cNum, bNum);
		if (GetOnHookCount(cNum, bNum) >= g_cfg->T_ON_HOOK)
        {
            ResetStateCount(cNum, bNum);
            SetToneFS(cNum, bNum, LOCK);
        }
    }
}

void ENGINE::DoIncomeTalk(WORD cNum, WORD bNum)
{
    if (!BIT(Clusters[cNum].DataPort.OOD, bNum))
    {
        IncOnHookCount(cNum, bNum);
		if (GetOnHookCount(cNum, bNum) >= g_cfg->T_ON_HOOK)
        {
            ResetStateCount(cNum, bNum);
            SetToneFS(cNum, bNum, LOCK);
        }
    }
}

void ENGINE::ResetData(WORD cNum, WORD bNum)
{
    ResetCurrentDigit(cNum, bNum);
	ResetPhone(cNum, bNum);
	Clusters[cNum].Found[bNum]         = FALSE;
	Clusters[cNum].NumOfDigits[bNum]   = 0;
	Clusters[cNum].Tariffs[bNum]       = 0;
	Clusters[cNum].StartTimes[bNum]    = 0;
	Clusters[cNum].StartDates[bNum]    = 0;
	Clusters[cNum].FirstOnes[bNum]     = FALSE;
	Clusters[cNum].ManualAnswer[bNum]  = FALSE;
	Clusters[cNum].CallAttrs[bNum]     = 0; // not included
	if (!g_cfg->MANUAL || g_cfg->IsExtension(cNum, bNum))
		Clusters[cNum].NumOfCalls[bNum]  = 0;
}

void ENGINE::DoOffHook(WORD cNum, WORD bNum, WORD phoneType)
{
	ResetData(cNum, bNum);

	// 2.21.1 reset final elapsed count:
	//   Useful for VIEW and DISPLAY. this allows to have the last count
	//   while the phone is on hook.
	ResetFinalElapsedCount(cNum, bNum);

    // --- we put these data here to avoid the refresh before next off-hook
    if (phoneType == TONEPHONE)
    {
        if (!BIT(Clusters[cNum].DataPort.OOD, bNum))
        {
            IncOnHookCount(cNum, bNum);
			if (GetOnHookCount(cNum, bNum) >= g_cfg->T_ON_HOOK)
            {
                ResetStateCount(cNum, bNum);
                SetToneFS(cNum, bNum, LOCK);
            }
        }
        else
        {
            ResetOnHookCount(cNum, bNum); // to avoid noise
            if (BIT(Clusters[cNum].DataPort.DTMFFlags, bNum))
            {
				if (GetStateCount(cNum, bNum) >= g_cfg->T_OFF_HOOK)
                {
                    ResetStateCount(cNum, bNum);
                    ResetDialCount(cNum, bNum);
                    SetToneFS(cNum, bNum, DTMFFLAG);
                }
            }
            else
            {
				if (GetStateCount(cNum, bNum) >= g_cfg->T_OFF_HOOK)
                    DecStateCount(cNum, bNum); // prevent count overflow
            }


        }
    }
    else
    {  // pulse
        if (!BIT(Clusters[cNum].DataPort.OOD, bNum))
        {
			if (GetStateCount(cNum, bNum) >= g_cfg->T_OFF_HOOK)
            {
                ResetStateCount(cNum, bNum);
                ResetDialCount(cNum, bNum);
                ResetOnHookCount(cNum, bNum); // to avoid noise
                SetPulseFS(cNum, bNum, BREAK);
            }
            else
            {
                IncOnHookCount(cNum, bNum);
				if (GetOnHookCount(cNum, bNum) >= g_cfg->T_ON_HOOK)
                {
                    ResetStateCount(cNum, bNum);
                    SetPulseFS(cNum, bNum, LOCK);
                }
            }
        }
		else if (GetStateCount(cNum, bNum) >= g_cfg->T_OFF_HOOK)
            DecStateCount(cNum, bNum); // prevent count overflow
    }


}

void ENGINE::DoDTMFFlag(WORD cNum, WORD bNum)
{
    // check if the phone is hanged
    if (!BIT(Clusters[cNum].DataPort.OOD, bNum))
    {
        IncOnHookCount(cNum, bNum);
		if (GetOnHookCount(cNum, bNum) >= g_cfg->T_ON_HOOK)
        {
            // use tone because tone has an upper priority
            SetFSs(cNum, bNum, LOCK, DIALERR);
            return ;
        }
    }
    else
    {
        if (!BIT(Clusters[cNum].DataPort.DTMFFlags, bNum))
        {
			if (GetStateCount(cNum, bNum) >= g_cfg->T_DTMF_FLAG)
            {
                SetCurrentDigit(cNum, bNum, (Clusters[cNum].DataPort.DTMFDigits >> (bNum*4)) & 0xF);
                // check invalid digits
                if (GetCurrentDigit(cNum, bNum) > 10)
                    SetToneFS(cNum, bNum, INTERDIG); // ignore '*' and '#'. JEAM/gcc
                else
                {
                    if (MaxNumOfDigits(cNum, bNum))
                    {
						if (g_cfg->IGNORE_EXTRA_DIGITS)
                            SetToneFS(cNum, bNum, INTERDIG); // ignore digits
                        else
                            SetFSs(cNum, bNum, LOCK, DIALERR);
                    }
                    else
                    {
                        if (IsFalseOne(cNum, bNum))
                            SetToneFS(cNum, bNum, INTERDIG); // ignore digits
                        else
                        {
                            if (GetCurrentDigit(cNum, bNum) == 10)
                                ResetCurrentDigit(cNum, bNum);  // ten changes to indicate 0
                            StoreCurrentDigit(cNum, bNum);
                            if (IsLockable(cNum, bNum))
                                SetFSs(cNum, bNum, LOCK, DIALERR);
                            else
                                SetToneFS(cNum, bNum, INTERDIG);
                        }
                    }
                }
                ResetStateCount(cNum, bNum);
            }
        }
    }
}

void ENGINE::DoBreak(WORD cNum, WORD bNum)
{
    if (BIT(Clusters[cNum].DataPort.OOD, bNum))
    {
		//
        // Since into the STB-x exists an OR implemented with a diode ($#@%*)
        // which response is pretty bad, some cards introduce a false break
        // when the answer signal goes low.
        // GCC/gcc. 21/12/94.
        // ignore the above and force to go interdigital state !!!
        //
		if (g_cfg->ASIGNAL == CFG::S_BIAS)
        {
            if (BIT(Clusters[cNum].DataPort.Answer, bNum))
            {
                //
                // we choose to prevent problems at inter digital rather than
                // during dialing since it is the most common event.
                // JEAM/gcc.  14/12/95.
                if (IsAnswerable(cNum, bNum))
                {
                    SetPulseFS(cNum, bNum, INTERDIG); // problems from inter digital
                    ResetStateCount(cNum, bNum);
                    return; // ignore !!!
				}


            }
        }
        // ignore false makes (noise) in Walter's phones. GCC/gcc. 28/10/94.
		if (GetStateCount(cNum, bNum) >= g_cfg->T_BREAK)
        {
            IncCurrentDigit(cNum, bNum);  // each change from BREAK to MAKE ...
            ResetStateCount(cNum, bNum);
            SetPulseFS(cNum, bNum, MAKE);
        }
        else
        {
            // bad break, go to make but doesn't take digit in account
			SetStateCount(cNum, bNum, g_cfg->T_MAKE);
            SetPulseFS(cNum, bNum, MAKE);
        }
        ResetOnHookCount(cNum, bNum); // avoid noise
	}
	else
	{
		IncOnHookCount(cNum, bNum);
		if (GetOnHookCount(cNum, bNum) >= g_cfg->T_ON_HOOK)
			SetFSs(cNum, bNum, LOCK, DIALERR); // tone has an upper priority
	}
}

void ENGINE::DoMake(WORD cNum, WORD bNum)
{
    if (!BIT(Clusters[cNum].DataPort.OOD, bNum))
    {
        // avoid false breaks (noise) in Walter's phones. GCC/gcc. 4/11/94.
		if (GetStateCount(cNum, bNum) >= g_cfg->T_MAKE)
        {
            ResetStateCount(cNum, bNum);
            SetPulseFS(cNum, bNum, BREAK);
        }
    }
    else
	{
		if (GetStateCount(cNum, bNum) >= g_cfg->T_MAKE+g_cfg->T_MAKE_MARGIN)
        {
            // check invalid digits
            if (GetCurrentDigit(cNum, bNum) > 10)
                SetFSs(cNum, bNum, LOCK, DIALERR); // noise !!!. JEAM/gcc
            else
            {
                if (MaxNumOfDigits(cNum, bNum))
                {
					if (g_cfg->IGNORE_EXTRA_DIGITS)
                        SetPulseFS(cNum, bNum, INTERDIG); // ignore
                    else
                        SetFSs(cNum, bNum, LOCK, DIALERR);
                }
                else
                {
                    if (IsFalseOne(cNum, bNum))
                        SetPulseFS(cNum, bNum, INTERDIG); // ignore
                    else
					{
                        if (GetCurrentDigit(cNum, bNum))
                        { // not a zero digit !
                            if (GetCurrentDigit(cNum, bNum) == 10)
                                ResetCurrentDigit(cNum, bNum);  // ten changes to indicate 0
                            StoreCurrentDigit(cNum, bNum);
                        }
                        if (IsLockable(cNum, bNum))
                            SetFSs(cNum, bNum, LOCK, DIALERR);
                        else
                            SetPulseFS(cNum, bNum, INTERDIG);
                    }
                }
			}
            ResetStateCount(cNum, bNum);
        }
	}
}

void ENGINE::DoInterdig(WORD cNum, WORD bNum, WORD phoneType)
{
	// v.2.20.8
	// lock not included based on number of digits
	if (Clusters[cNum].CallAttrs[bNum] & NOT_INCLUDED_CALL_MASK)
	{
		static BOOL bLock;
		bLock = FALSE;

		if (Clusters[cNum].CallAttrs[bNum] & DDI_DIAL_MASK)
			bLock = Clusters[cNum].NumOfDigits[bNum] >= g_cfg->INTER_DIGITS_NOT_INCLUDED;
		else if (Clusters[cNum].CallAttrs[bNum] & LOCAL_DIAL_MASK)
			bLock = Clusters[cNum].NumOfDigits[bNum] >= g_cfg->LOCAL_DIGITS_NOT_INCLUDED;
		else // DDN/Cellular
			bLock = Clusters[cNum].NumOfDigits[bNum] >= g_cfg->NAL_DIGITS_NOT_INCLUDED;

		if (bLock)
		{
			NotIncBooth = cNum*CLUSTER_SIZE+bNum;
			ResetStateCount(cNum, bNum);
			SetFSs(cNum, bNum, LOCK, LOCK);
			if (Clusters[cNum].Simula[bNum])
			{
				Clusters[cNum].Simula[bNum] = FALSE;
				if (!Clusters[cNum].Available)
				{
					SetToneFS(cNum, bNum, NOPHONE);
				}
			}

			return ; // bye
		}
	}
	// included or allowed !
	if (g_cfg->MANUAL_ANSWER)
	{
		// Insert a pulse to reset the JEAM's answer circuit.
		if (!Clusters[cNum].ManualAnswer[bNum])
		{
			// Put the line up. JEAM/gcc.
			Clusters[cNum].DataPort.Lock |= (1 << bNum);
			Clusters[cNum].ManualAnswer[bNum] = TRUE;
		}
		else
		{
			// Put the line down to finish the insert pulse. JEAM/gcc.
			if (BIT(Clusters[cNum].DataPort.Lock, bNum))
				Clusters[cNum].DataPort.Lock &= ~(1 << bNum);
		}
	}
	//
	// check to see if the user is sleeping on top of the phone !!!
	if (GetDialCount(cNum, bNum) >= g_cfg->T_DIAL)
	{
		// not to hang. JEAM/gcc.
		if (!IsAnswerable(cNum, bNum))
		{
			SetFSs(cNum, bNum, LOCK, DIALERR); // tone has an upper priority
			return ;
		}
	}
	// tone
	if (phoneType == TONEPHONE)
	{
		if (!BIT(Clusters[cNum].DataPort.OOD, bNum))
		{
			// the user is not so sure !
			IncOnHookCount(cNum, bNum);
			if (GetOnHookCount(cNum, bNum) >= g_cfg->T_ON_HOOK)
			{
				ResetStateCount(cNum, bNum);
				if (Clusters[cNum].NumOfDigits[bNum] > 0)
					SetFSs(cNum, bNum, LOCK, DIALERR); // tone has an upper priority
				else
					SetToneFS(cNum, bNum, LOCK);
				return ;
			}
		}
		if (!BIT(Clusters[cNum].DataPort.DTMFFlags, bNum))
		{
			CheckAnswerSignal(cNum, bNum, phoneType);
		}
		else
		{
			if (GetStateCount(cNum, bNum) >= g_cfg->T_DTMF_INTERDIG)
			{
				ResetStateCount(cNum, bNum);
				ResetCurrentDigit(cNum, bNum);
				SetToneFS(cNum, bNum, DTMFFLAG);
			}
		}
	}
	else
	{ // Pulse
		CheckAnswerSignal(cNum, bNum, phoneType);
		if (!BIT(Clusters[cNum].DataPort.OOD, bNum))
		{
			if (!GetBiasCount(cNum, bNum))
			{
				if (GetStateCount(cNum, bNum) >= g_cfg->T_INTERDIG)
				{
					ResetStateCount(cNum, bNum);
					ResetCurrentDigit(cNum, bNum);
					SetPulseFS(cNum, bNum, BREAK);
				}
			}
		}
	}
}

void ENGINE::DoAnswer(WORD cNum, WORD bNum, WORD phoneType)
{
	if (BIT(Clusters[cNum].DataPort.OOD, bNum))
	{
		// ready to go to talk
		ResetStateCount(cNum, bNum);
		if (phoneType == TONEPHONE)
            SetToneFS(cNum, bNum, TALK);
        else
            SetPulseFS(cNum, bNum, TALK);
        // record date and time
        Clusters[cNum].StartTimes[bNum] = CurrentTime;
        Clusters[cNum].StartDates[bNum] = CurrentDate;
        // indicate if the call is not include into phone info
        if (Clusters[cNum].CallAttrs[bNum] & NOT_INCLUDED_CALL_MASK)
        {
            NotIncBooth = cNum*CLUSTER_SIZE+bNum;
        }
        // each good call force to forget errors
        Clusters[cNum].DialErrors[bNum] = 0; // reset !!!
        Clusters[cNum].ComErrors[bNum]  = 0; // reset !!!
        // force to calc PreTime AR/gc v.218c
        SetPreTime(cNum, bNum, 0U);
    }
    else
    {
        IncOnHookCount(cNum, bNum);
		if (GetOnHookCount(cNum, bNum) >= g_cfg->T_ON_HOOK)
        {
            ResetStateCount(cNum, bNum);
            SetFSs(cNum, bNum, LOCK, LOCK);
            if (Clusters[cNum].Simula[bNum])
            {
                Clusters[cNum].Simula[bNum] = FALSE;
                if (!Clusters[cNum].Available)
				{
                    SetToneFS(cNum, bNum, NOPHONE);
                }
            }
        }
	}
}

void ENGINE::DoTalk(WORD cNum, WORD bNum, WORD phoneType)
{
	// Attention:
	// Use 2.20.5 to patch 2.32 build 1.
	// Version 2.20 build 5. changed to use cascade non structured
	// sentences. Nov 10, 2001.

	// don't allow "Not Included DDI". v.219a. CR/gc.
	if (
		(Clusters[cNum].CallAttrs[bNum] & DDI_DIAL_MASK) &&
		(Clusters[cNum].CallAttrs[bNum] & NOT_INCLUDED_CALL_MASK)
	)
	{
		NotIncBooth = cNum*CLUSTER_SIZE+bNum;
		ResetStateCount(cNum, bNum);
		SetFSs(cNum, bNum, LOCK, LOCK); // LOCK !
		// check simula !
		if (Clusters[cNum].Simula[bNum])
		{
			Clusters[cNum].Simula[bNum] = FALSE;
			if (!Clusters[cNum].Available)
			{
				SetToneFS(cNum, bNum, NOPHONE);
			}
		}
		return ; // NSP
	}

	/*
	////////////////////////////////////////////////////////////////////
	// Don't check SPY: Not necessary. 2.20 build 5 .Nov 10 2001. CR/gc
	////////////////////////////////////////////////////////////////////
	static WORD boothCount; // static !!!
	boothCount = cNum*CLUSTER_SIZE+bNum;
	if (boothCount == SpyBooth)
	{
		// check for spy JEF/gcc v.211
		// SPY !!!
		ResetOnHookCount(cNum, bNum) ; // clear. HTR/gcc
		IncElapsedCount(cNum, bNum);
		if (Clusters[cNum].PrePaid[bNum] && Clusters[cNum].PreTime[bNum])
		{
			// is time to hang up the phone?
			if (GetElapsedCount(cNum, bNum) >= Clusters[cNum].PreTime[bNum])
			{
				if (phoneType == TONEPHONE)
					SetToneFS(cNum, bNum, STORE);
				else
					SetPulseFS(cNum, bNum, STORE);
			}
		}
		return ; // NSP
	}
	////////////////////////////////////////////////////////////////////
	// Don't check SPY: END
	////////////////////////////////////////////////////////////////////
	*/

	// no spy
	if (BIT(Clusters[cNum].DataPort.OOD, bNum))
	{
		// call on progress
		ResetOnHookCount(cNum, bNum) ; // clear. HTR/gcc
		IncElapsedCount(cNum, bNum);
		if (Clusters[cNum].PrePaid[bNum] && Clusters[cNum].PreTime[bNum])
		{
			// is time to hang the phone?
			if (GetElapsedCount(cNum, bNum) >= Clusters[cNum].PreTime[bNum] )
			{
				if (phoneType == TONEPHONE)
					SetToneFS(cNum, bNum, STORE);
				else
					SetPulseFS(cNum, bNum, STORE);

				ResetStateCount(cNum, bNum); // 2.21.1 Build 5 gc|hj

				return ;
			}
		}
	}
	else
	{
		// phone was hung
		IncOnHookCount(cNum, bNum);
		if (GetOnHookCount(cNum, bNum) >= g_cfg->T_ON_HOOK)
		{
			if (phoneType == TONEPHONE)
				SetToneFS(cNum, bNum, STORE);
			else
				SetPulseFS(cNum, bNum, STORE);

			ResetStateCount(cNum, bNum); // 2.21.1 Build 5 gc|hj
		}
	}
}

void ENGINE::DoStore(WORD cNum, WORD bNum)
{
	// check if the elapsed time is enough to generate a receipt
	if (GetElapsedCount(cNum, bNum) >= g_cfg->T_TALK)
	{
		// 2.21.1 Build 5: add delay for store state gc|hj
		if (GetStateCount(cNum, bNum) >= g_cfg->T_STORE)
		{
			ResetStateCount(cNum, bNum);
			StoreReceipt(cNum, bNum);
		}
	}
	else
	{
		ResetStateCount(cNum, bNum);
		SetFSs(cNum, bNum, LOCK, LOCK);
		if (Clusters[cNum].Simula[bNum])
		{
			Clusters[cNum].Simula[bNum] = FALSE;
			if (!Clusters[cNum].Available)
				SetToneFS(cNum, bNum, NOPHONE);
		}
	}
}

void ENGINE::DoDialErr(WORD cNum, WORD bNum)
{
	// accumulate dialing errors
	if (!Clusters[cNum].NoStatistics[bNum])
	{
		g_cfg->N_DIAL_ERR++;
		if (g_cfg->MAX_DIAL_ERR)
        { // !0 to process error
            Clusters[cNum].DialErrors[bNum]++;
			if (Clusters[cNum].DialErrors[bNum] == g_cfg->MAX_DIAL_ERR)
            {
                Clusters[cNum].DialErrors[bNum] = 0; // reset !!!
                DialErrBooth = cNum*CLUSTER_SIZE+bNum;   // signalize an error
            }


        }
    }
    ResetStateCount(cNum, bNum);
    SetFSs(cNum, bNum, LOCK, LOCK);
    if (Clusters[cNum].Simula[bNum])
    {
        Clusters[cNum].Simula[bNum] = FALSE;
        Clusters[cNum].NoReceipt[bNum] = FALSE;
        Clusters[cNum].NoStatistics[bNum] = FALSE;
        if (!Clusters[cNum].Available)
            SetToneFS(cNum, bNum, NOPHONE);
    }
}

void ENGINE::DoComErr(WORD cNum, WORD bNum)
{
    // accumulate comunication errors
    if (!Clusters[cNum].NoStatistics[bNum])
    {
		g_cfg->N_COM_ERR++;
		if (g_cfg->MAX_COM_ERR)
        { // !0 to process error
            Clusters[cNum].ComErrors[bNum]++;
			if (Clusters[cNum].ComErrors[bNum] == g_cfg->MAX_COM_ERR)
            {
                Clusters[cNum].ComErrors[bNum] = 0; // reset !!!
                ComErrBooth = cNum*CLUSTER_SIZE+bNum;   // signalize an error
            }


        }
    }
    ResetStateCount(cNum, bNum);
    SetFSs(cNum, bNum, LOCK, LOCK);
    if (Clusters[cNum].Simula[bNum])
    {
        Clusters[cNum].Simula[bNum] = FALSE;
        Clusters[cNum].NoReceipt[bNum] = FALSE;
        Clusters[cNum].NoStatistics[bNum] = FALSE;
        if (!Clusters[cNum].Available)
            SetToneFS(cNum, bNum, NOPHONE);
    }
}
