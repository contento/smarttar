//
// [ PH_UTIL.CPP ]
//

#include "stdst.h"

#include <ph_eng.h>

extern CFG *g_cfg;
extern UINT g_pass;

PHONE PH_ENGINE::LockedNumbers[MAX_LOCKED_NUMBERS];

BOOL PH_ENGINE::LoadHeader(fstream& file)
{
    FILE_HEADER header;
    file.read((char *)&header, sizeof(FILE_HEADER));
    // anotate .DAT version. v.220
    FileVersion = 0;
    BOOL ok = (file.gcount() == sizeof(FILE_HEADER)) && header.IsValid();
    if (ok)
    {
        FileVersion = header.GetVersion();
#if (__FHEADER>=3)
        header.getAppVersion(MajorAppVersion, MinorAppVersion, UpgradeAppVersion);
#endif
    }
    return ok;
}

BOOL PH_ENGINE::SaveHeader(fstream& file)
{
    FILE_HEADER header;
    file.write((char *)&header, sizeof(FILE_HEADER));
    return TRUE;
}

void PH_ENGINE::SetDefaultLockedNumbers(void)
{
    memset(&LockedNumbers, 0, sizeof(LockedNumbers));
}

BOOL PH_ENGINE::LoadLockedNumbers(fstream& file)
{
    file.read((char *)&LockedNumbers, sizeof(LockedNumbers));
    return file.gcount() == sizeof(LockedNumbers);
}

BOOL PH_ENGINE::SaveLockedNumbers(fstream& file)
{
    file.write((char *)LockedNumbers, sizeof(LockedNumbers));
    return TRUE;
}

BOOL PH_ENGINE::IsLockedNumber(const PHONE& phone)
{
    static BOOL found;
    found = FALSE;
    // first find in list of lockables
    static int i;
    static int phoneLen, leftLen, rightLen;
    phoneLen = strlen((char *)&phone);
    for (i = 0; i < MAX_LOCKED_NUMBERS/2 && !found; i++)
    {
        // only compare string with the same length !!!
        if (strlen(LockedNumbers[i]) == phoneLen)
        {
            if (!strcmp(LockedNumbers[i], (char *)&phone))
                found = TRUE;
        }
    }
    // next in ranges (by pairs)
    for (i = MAX_LOCKED_NUMBERS/2; i < MAX_LOCKED_NUMBERS && !found; i+= 2)
    {
        leftLen  = strlen(LockedNumbers[i]);
        rightLen = strlen(LockedNumbers[i+1]);
        // only compare string with the same length !!!
        if (leftLen == phoneLen && rightLen == phoneLen)
        {
            found = ( // compare strings instead of long v.218c
                        (strcmp(LockedNumbers[i], (char *)&phone  ) <= 0) &&
                        (strcmp((char *)&phone, LockedNumbers[i+1]) <= 0)
                    );
        }
    }
    return found;
}

BOOL PH_ENGINE::GetLockedNumber(PHONE& phone, int i)
{
    BOOL ok = FALSE;
    if (i >= 0 && i < MAX_LOCKED_NUMBERS)
    {
        strcpy((char *)&phone, LockedNumbers[i]);
        ok = TRUE;
    }
    return ok;
}

BOOL PH_ENGINE::SetLockedNumber(const PHONE& phone, int i)
{
    BOOL ok = FALSE;
    if (i >= 0 && i < MAX_LOCKED_NUMBERS)
    {
        strcpy(LockedNumbers[i], (char *)&phone);
        ok = TRUE;
    }
    return ok;
}

BOOL PH_ENGINE::IsLockable(const PHONE& phone, int numOfDigits)
{
	// numOfDigits because we don't wanna use expensive strlen()
	static BOOL locked;
	locked = FALSE;
	//
	static CALL_ATTR attr;
	if (GetCallAttr(phone, attr, numOfDigits))
	{
		// zero digits == locked
		switch (attr)
		{
#if !defined(__EDA__)
		case DDI_CALL:
#else
		case DDI_EDA2TEL_CALL:
#endif
			locked = (!g_cfg->INTER_DIGITS);
			break;
		case CELLULAR_CALL:
			locked = (!g_cfg->CELLULAR_DIGITS);
			break;
        case BORDER_CALL  :
			locked = (!g_cfg->BORDER_DIGITS);
            break;
#if !defined(__EDA__)
        case DDN_CALL     :
#else
        case DDN_EDA2EPM_CALL:
        case DDN_EDA2TEL_CALL:
#endif
			locked = (!g_cfg->NAL_DIGITS);
            break;
        case SPECIAL_CALL :
			locked = (!g_cfg->SPECIAL_DIGITS);
            break;
#if !defined(__EDA__)
        case LOCAL_CALL   :
#else
        case EDA2EDA_CALL:
        case LOCAL_EDA2EPM_CALL:
#endif
			locked = (!g_cfg->LOCAL_DIGITS);
            break;
        }
        if (!locked)
        {
            locked = IsLockedNumber(phone);
        }
    }
    else
    {
        // it was impossible to analyze but maybe it's in list of lockables v.218c
        locked = IsLockedNumber(phone);
    }
    return locked;
}

BOOL PH_ENGINE::IsAnswerable(const PHONE& phone, int numOfDigits, BOOL isException)
{
    static BOOL answerable;
    answerable = FALSE;
    //
    // verify number of digits, but this time the size minus a margin.
    // v2.16: If the number of digits is zero and exist prepaid is allowed.
    //
    static CALL_ATTR attr;
	if (PH_ENGINE::GetCallAttr(phone, attr, numOfDigits))
	{
		// zero digits under prepaid is allowed
		switch (attr)
        {
#if !defined(__EDA__)
        case DDI_CALL:
            {
#else
		case DDI_EDA2TEL_CALL:
			{
#endif
				if (numOfDigits >= g_cfg->INTER_DIGITS-g_cfg->INTER_DIGITS_MARGIN)
				{
					answerable = TRUE;
				}
				else
				{
					answerable = (!g_cfg->INTER_DIGITS && isException); // allow it
				}


				break;
			}
		case BORDER_CALL  :
			{
				if (numOfDigits >= g_cfg->BORDER_DIGITS-g_cfg->INTER_DIGITS_MARGIN)
				{
					answerable = TRUE;
				}
				else
				{
					answerable = (!g_cfg->BORDER_DIGITS && isException); // allow it
				}


				break;
			}
		case CELLULAR_CALL:
#if !defined(__EDA__)
		case DDN_CALL:
			{
#else
		case DDN_EDA2EPM_CALL:
		case DDN_EDA2TEL_CALL:
			{
#endif
				if (numOfDigits >= g_cfg->NAL_DIGITS-g_cfg->NAL_DIGITS_MARGIN)
				{
					answerable = TRUE;
				}
				else
				{
					answerable = (!g_cfg->NAL_DIGITS && isException); // allow it
				}
				break;
			}
		case SPECIAL_CALL :
			{
				if (numOfDigits >= g_cfg->SPECIAL_DIGITS)
				{
					answerable = TRUE;
				}
				else
				{
					answerable = (!g_cfg->SPECIAL_DIGITS && isException); // allow it
				}
				break;
			}
#if !defined(__EDA__)
		case LOCAL_CALL   :
			{
#else
		case EDA2EDA_CALL:
		case LOCAL_EDA2EPM_CALL:
			{
#endif
				if (numOfDigits >= g_cfg->LOCAL_DIGITS-g_cfg->LOCAL_DIGITS_MARGIN)
				{
					answerable = TRUE;
                }
                else
                {
					answerable = (!g_cfg->LOCAL_DIGITS && isException); // allow it
                }


                break;
            }
        }
    }
    return answerable;
}

BOOL PH_ENGINE::IsMaxDigits(const PHONE& phone, int numOfDigits)
{
    // numOfDigits because we can't use expensive strlen()
    static BOOL ok;
	ok = FALSE;
    static CALL_ATTR attr;
	if (GetCallAttr(phone, attr, numOfDigits))
    {
        switch (attr)
        {
#if !defined(__EDA__)
        case DDI_CALL:
#else
		case DDI_EDA2TEL_CALL:
#endif
			ok = (numOfDigits == g_cfg->INTER_DIGITS);
            break;
        case CELLULAR_CALL:
			ok = (numOfDigits == g_cfg->CELLULAR_DIGITS);
            break;
        case BORDER_CALL  :
			ok = (numOfDigits == g_cfg->BORDER_DIGITS);
            break;
#if !defined(__EDA__)
        case DDN_CALL     :
#else
        case DDN_EDA2EPM_CALL:
        case DDN_EDA2TEL_CALL:
#endif
			ok = (numOfDigits == g_cfg->NAL_DIGITS);
            break;
        case SPECIAL_CALL :
			ok = (numOfDigits == g_cfg->SPECIAL_DIGITS);
            break;
#if !defined(__EDA__)
        case LOCAL_CALL   :
#else
        case EDA2EDA_CALL:
        case LOCAL_EDA2EPM_CALL:
#endif
			ok = (numOfDigits == g_cfg->LOCAL_DIGITS);
            break;
        }
    }
    return ok;
}

BOOL PH_ENGINE::AnalizeFirstDigit(const PHONE& phone, CALL_ATTR &attr)
{
	if (!isdigit(phone[0]))
		return FALSE;

	// TRUE: if it was possible to analize
    static BOOL ok;
    ok = FALSE;
    static int firstDigit;
    firstDigit  = phone[0]-'0';
    //
	if (firstDigit != g_cfg->ACCESS)
    {
        // local, maybe EDA
#if !defined(__EDA__)
        attr = LOCAL_CALL;
        ok = TRUE;
#else
        if (firstDigit > 1 && firstDigit < 8)
        {
            attr = LOCAL_EDA2EPM_CALL; // 2..7
            ok = TRUE;
        }
		if (firstDigit == g_cfg->SPECIAL_ACCESS)
        {
            attr = SPECIAL_CALL;
            ok = TRUE;
        }
#endif
    }
    return ok;
}

BOOL PH_ENGINE::AnalizeSecondDigit(const PHONE& phone, CALL_ATTR &attr)
{
	if (!isdigit(phone[0]))
		return FALSE;

	// TRUE: if it was possible to analize
	static BOOL ok;
	ok = FALSE;
	// warning we have to analize the first too to avoid surprises
	static int firstDigit, secondDigit;
	firstDigit  = phone[0]-'0';
	secondDigit = phone[1]-'0';
	//
	if (firstDigit == g_cfg->ACCESS)
	{
		if (g_cfg->ACCESS_LEVELS == 2)
		{
			if (secondDigit == g_cfg->INTER_ACCESS)
			{
#if !defined(__EDA__)
				attr = DDI_CALL;
#else
				attr = DDI_EDA2TEL_CALL;
#endif
                ok = TRUE;
			}
			else if (secondDigit == g_cfg->BORDER_ACCESS)
            {
                attr = BORDER_CALL;
                ok = TRUE;
            }
			else if (secondDigit == g_cfg->CELLULAR_ACCESS)
            {
                attr = CELLULAR_CALL;
                ok = TRUE;
            }
            else
            {
#if !defined(__EDA__)
                attr = DDN_CALL;
#else
                attr = DDN_EDA2TEL_CALL; // the others
#endif
                ok = TRUE;
            }
		}
    }
#if defined(__EDA__)
	else if (firstDigit == g_cfg->EDA_ACCESS && (secondDigit > 1 && secondDigit < 9))
    {
        attr = EDA2EDA_CALL; // 82..88 -> 2.21.8
        ok = TRUE;
    }
#endif
    //
    return ok;
}

BOOL PH_ENGINE::AnalizeThirdDigit(const PHONE& phone, CALL_ATTR &attr)
{
	if (!isdigit(phone[0]))
		return FALSE;

	// TRUE: if it was possible to analize
	static BOOL ok;
	ok = FALSE;
	// warning we have to analize the first two to avoid surprises
	static int firstDigit, secondDigit, thirdDigit;
	firstDigit  = phone[0]-'0';
	secondDigit = phone[1]-'0';
	thirdDigit  = phone[2]-'0';
	//
	if (firstDigit == g_cfg->ACCESS)
	{
		if (g_cfg->ACCESS_LEVELS == 2)
		{
#if defined(__EDA__)
			if (secondDigit == 4 && (thirdDigit > 1 && thirdDigit < 7))
			{
				attr = DDN_EDA2EPM_CALL; // 942..946
				ok = TRUE;
			}
#endif
		}
		else
		{ // three levels
			if (secondDigit == g_cfg->INTER_ACCESS)
			{
#if !defined(__EDA__)
				attr = DDI_CALL;
#else
				attr = DDI_EDA2TEL_CALL;
#endif
				ok = TRUE;

				if (thirdDigit != g_cfg->OPERATOR_ACCESS)
				{
					// 2.22 build 25
					attr |= NOT_INCLUDED_CALL_MASK;
				}
			}
			else if (secondDigit == g_cfg->CELLULAR_OPERATOR_ACCESS)
			{
				attr = CELLULAR_CALL;
				ok = TRUE;
				if (thirdDigit != g_cfg->CELLULAR_ACCESS)
				{
					// 2.22 build 25
					// It has to be a NI Cell call !!! AR/gc.
					attr |= NOT_INCLUDED_CALL_MASK;
				}
			}
			else if (secondDigit == g_cfg->OPERATOR_ACCESS)
			{
				if (thirdDigit == g_cfg->BORDER_ACCESS)
				{
					attr = BORDER_CALL;
					ok = TRUE;
				}
				else
				{
#if !defined(__EDA__)
					attr = DDN_CALL;
#else
					attr = DDN_EDA2TEL_CALL; // the others
#endif
					ok = TRUE;
				}
			}
			else
			{
				// 2.22 build 25
				// It has to be a NI DDN call !!! AR/gc.
#if !defined(__EDA__)
					attr = DDN_CALL;
#else
					attr = DDN_EDA2TEL_CALL; // the others
#endif
				attr |= NOT_INCLUDED_CALL_MASK;
				ok = TRUE;
			}
		}
	}
	//
	return ok;
}

#if defined(__EDA__)
BOOL PH_ENGINE::AnalizeFourthDigit(const PHONE& phone, CALL_ATTR &attr)
{
	if (!isdigit(phone[0]))
		return FALSE;

	// TRUE: if it was possible to analize
    static BOOL ok;
    ok = FALSE;
    // warning we have to analize the first two to avoid surprises
    static int firstDigit, secondDigit, thirdDigit, fourthDigit;
    firstDigit  = phone[0]-'0';
    secondDigit = phone[1]-'0';
    thirdDigit  = phone[2]-'0';
    fourthDigit = phone[3]-'0';
    //
    if (
		firstDigit == g_cfg->ACCESS           &&
        secondDigit == g_cfg->OPERATOR_ACCESS &&
        thirdDigit == 4                      &&
        (fourthDigit > 1 && fourthDigit < 7)
    )
    {
        attr = DDN_EDA2EPM_CALL;
        ok = TRUE; // 0x42-0x46
    }


	return ok;
}
#endif

void PH_ENGINE::CalcCallValues(CallInfo & info)
{
	info.rawMin	= g_Milisec2Time(info.elapsedTime, g_cfg->CORRECTION_TIME);

	WORD schedule = 0;
	TARIFF_ENTRY tariffEntry;
	if (info.callAttr & INTERNATIONAL_CALL_MASK)
	{
		tariffEntry = GetDDITariff(info.nTariff);
		schedule 	= DDI_REDUCED_BORDER;
		if (info.callAttr != BORDER_CALL)
		{
			schedule = (!strcmp(info.city, g_cfg->USA))?DDI_REDUCED_USA:DDI_REDUCED_OTHER;
		}
	}
	else
	{
		tariffEntry = GetDDNTariff(info.nTariff);
		// adjust cellular. 2.33
		if (info.callAttr == CELLULAR_CALL)
		{
			tariffEntry.TaxPercent = g_cfg->CELLULAR_TAX;
		}
	}

	info.ceilMin 	 = CalcCeilMinutes(info.rawMin, schedule, info.callAttr, info.isExtension);
	info.paidPercent = CalcCallPercent(info.date, info.time, schedule, info.callAttr, info.isExtension);
	info.taxPercent  = tariffEntry.TaxPercent;

	// --- value per min (change for ST 2.09, no tax)
	info.valuePerMin = tariffEntry.Value*(((double)info.paidPercent)/100.0F);

	// --- call value (+tax)
	info.value = info.ceilMin*info.valuePerMin*(1.0F+tariffEntry.TaxPercent/100.0F);

	// apply round Ja, Ja ...
	if (info.isExtension)
	{
		if (g_cfg->E_APPLY_ROUND)
		{
			info.value = g_Round(info.value, g_cfg->M_ROUND);
		}
	}
	else
	{
		info.value = g_Round(info.value, g_cfg->M_ROUND);
	}

	// lookout Tax is for the value after adjusting (v.220 CE/gc) !!!
	info.tax = info.value*(tariffEntry.TaxPercent/(100.0F+tariffEntry.TaxPercent));
}

double PH_ENGINE::CalcCeilMinutes
(
	double 	rawMin,
	WORD 	schedule,
	WORD 	callAttr,
	BOOL 	isExtension
)
{
	double ceilMin = rawMin;
	if (callAttr & INTERNATIONAL_CALL_MASK)
    {
        switch (schedule)
        {
		case DDI_REDUCED_BORDER:
			// adjust to the minimum allowed time
			if (isExtension)
			{
				if (ceilMin < g_cfg->E_MIN_BORDER)
					ceilMin = g_cfg->E_MIN_BORDER;

				ceilMin = g_Ceil(ceilMin, g_cfg->E_CEIL_BORDER);
			}
			else
			{
				if (ceilMin < g_cfg->MIN_BORDER)
					ceilMin = g_cfg->MIN_BORDER;

				ceilMin = g_Ceil(ceilMin, g_cfg->CEIL_BORDER);
			}
			break;
		case DDI_REDUCED_USA:
			if (isExtension)
			{
				if (ceilMin < g_cfg->E_MIN_USA)
					ceilMin = g_cfg->E_MIN_USA;

				ceilMin = g_Ceil(ceilMin, g_cfg->E_CEIL_USA);
			}
			else
			{
				if (ceilMin < g_cfg->MIN_USA)
					ceilMin = g_cfg->MIN_USA;

				ceilMin = g_Ceil(ceilMin, g_cfg->CEIL_USA);
			}
            break;
        default:
            if (isExtension)
            {
				if (ceilMin < g_cfg->E_MIN_INTER)
					ceilMin = g_cfg->E_MIN_INTER;

				ceilMin = g_Ceil(ceilMin, g_cfg->E_CEIL_INTER);
			}
			else
			{
				if (ceilMin < g_cfg->MIN_INTER)
					ceilMin = g_cfg->MIN_INTER;

				ceilMin = g_Ceil(ceilMin, g_cfg->CEIL_INTER);
			}
		}
    }
    else
	{
        if (callAttr == CELLULAR_CALL)
        {
            if (isExtension)
			{
				if (ceilMin < g_cfg->E_MIN_CELLULAR)
					ceilMin = g_cfg->E_MIN_CELLULAR;

				ceilMin = g_Ceil(ceilMin, g_cfg->E_CEIL_CELLULAR);
			}
            else
            {
				if (ceilMin < g_cfg->MIN_CELLULAR)
					ceilMin = g_cfg->MIN_CELLULAR;

				ceilMin = g_Ceil(ceilMin, g_cfg->CEIL_CELLULAR);
			}
        }
		else
        {
			if (isExtension)
            {
				if (ceilMin < g_cfg->E_MIN_NAL)
					ceilMin= g_cfg->E_MIN_NAL;

				ceilMin = g_Ceil(ceilMin, g_cfg->E_CEIL_NAL);
			}
			else
			{
				if (ceilMin < g_cfg->MIN_NAL)
					ceilMin= g_cfg->MIN_NAL;

				ceilMin = g_Ceil(ceilMin, g_cfg->CEIL_NAL);
			}
        }
	}

	return ceilMin;
}

int PH_ENGINE::CalcCallPercent
(
	int 	date,
	int 	time,
	WORD 	schedule,
	WORD 	callAttr,
	BOOL 	isExtension
)
{
	int percent = 100; // 100%
	if (callAttr & INTERNATIONAL_CALL_MASK)
	{
		WORD dayType = PH_ENGINE::DDI_MONDAY_FRIDAY;
		if (_IsSunday(date) || g_cfg->IsHollyday(date))
			dayType = PH_ENGINE::DDI_SUNDAY_HOLLYDAY;
		else if (_IsSaturday(date))
            dayType = PH_ENGINE::DDI_SATURDAY;
        if (isExtension)
		{
			if (g_cfg->E_APPLY_DDI_SCHEDULE)
				percent = GetDDIPercent(schedule, dayType, time);
		}
		else
		{
			if (g_cfg->APPLY_DDI_SCHEDULE)
				percent = GetDDIPercent(schedule, dayType, time);
		}
    }
    else
	{
        WORD dayType = PH_ENGINE::DDN_MONDAY_FRIDAY;
		if (g_cfg->IsHollyday(date))
            dayType = PH_ENGINE::DDN_HOLLYDAY;
		else if (_IsWeekend(date))
            dayType = PH_ENGINE::DDN_WEEKEND;
        if (isExtension)
        {
			if (g_cfg->E_APPLY_DDN_SCHEDULE)
				percent = GetDDNPercent(dayType, time);
		}
		else
		{
			if (g_cfg->APPLY_DDN_SCHEDULE)
				percent = GetDDNPercent(dayType, time);
		}
	}

	return percent;
}
