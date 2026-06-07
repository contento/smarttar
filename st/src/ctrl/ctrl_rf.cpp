//
// [ CTRL_RF.CPP ]
//

#include "stdst.h"

#include <control.h>
#include <ph_eng.h>
#include <ssaver.h>

extern CFG       *g_cfg;
extern PH_ENGINE *g_phEngine;
extern SSAVER    *g_ssaver;

static const long MIN_PREPAID_TIME  = 1; // 1 milliseconds force to LOCK call
static const long PREPAID_THRESHOLD = PIT_TENMSECDIV;

//
// please analize the strategy:
// if (data coming from RTEngine different from data in view) {
//      update data in view with data from RTEngine
//      Refresh data in out device
// }
// 		(+) to avoid innecesary refreshes.
// 		(-) too much space to save data.
//
// GCC/gcc
//

void CONTROLLER::CookViewData(void)
{
	for (int cNum=0; cNum<g_cfg->ACTIVE_CLUSTERS; cNum++)
	{
		for (int bNum=0; bNum<CLUSTER_SIZE; bNum++)
		{
			cookViewPhoneInfo(cNum, bNum);
			cookViewElapsedTime(cNum, bNum);
			checkWatchDog(cNum, bNum);
		}
	}
}

void CONTROLLER::cookViewPhoneInfo(int cNum, int bNum)
{
	if (!RTEngine->GetFound(cNum, bNum))
	{
		// optimize found
		PHONE phone;
		RTEngine->GetPhone(cNum, bNum, phone);
		PH_ENGINE::CALL_PARAMETERS parameters;
		PH_ENGINE::PLACE_ENTRY placeEntry;
		BOOL found = g_phEngine->Search(phone, placeEntry, parameters);

		RTEngine->SetFound(cNum, bNum, found);

		RTEngine->SetCallAttr(cNum, bNum, parameters.Attr);

		// city
		strcpy(View->m_callInfo[cNum][bNum].city, (char *)placeEntry.Place);

		// tariff
		View->m_callInfo[cNum][bNum].nTariff 	= placeEntry.TariffNum;
		RTEngine->SetTariff(cNum, bNum, placeEntry.TariffNum);

		// area or access header
		strcpy(View->m_callInfo[cNum][bNum].area, ""); // clear 2.30 build 13
		if (g_cfg->VIEW_PHONE)
		{
			strcpy(View->m_callInfo[cNum][bNum].area, parameters.AccessHeader);
		}
	}

	// phone avoid header

	strcpy(View->m_callInfo[cNum][bNum].phone, ""); // clear 2.30 build 10
	if (g_cfg->VIEW_PHONE)
	{

		int from = strlen(View->m_callInfo[cNum][bNum].area);
		PHONE phone;
		RTEngine->GetPhone(cNum, bNum, phone);
		if (from < strlen(phone))
		{
			strcpy(View->m_callInfo[cNum][bNum].phone, &phone[from]);
		}
	}
}

void CONTROLLER::cookViewElapsedTime(int cNum, int bNum)
{
	View->m_callInfo[cNum][bNum].ceilMin	= 0.0; // 2.30 build 9
	View->m_callInfo[cNum][bNum].rawMin		= 0.0;
	View->m_callInfo[cNum][bNum].value 		= 0.0;

	long elapsedTime = RTEngine->GetElapsedCount(cNum, bNum);

	if (!elapsedTime && RTEngine->GetFinalElapsedCount(cNum, bNum))
	{
		// Call is hang but we need to refresh the equivalent to a Receipt
		elapsedTime = RTEngine->GetFinalElapsedCount(cNum, bNum);
	}

	if (!elapsedTime)
		return ;

	WORD boothCount	= cNum*CLUSTER_SIZE+bNum;

	View->m_callInfo[cNum][bNum].date 			= RTEngine->GetStartDate(cNum, bNum);
	View->m_callInfo[cNum][bNum].time 			= RTEngine->GetStartTime(cNum, bNum);
	View->m_callInfo[cNum][bNum].callAttr 		= RTEngine->GetCallAttr(cNum, bNum);
	View->m_callInfo[cNum][bNum].isExtension	= g_cfg->E_FIRST_EXT && boothCount >= (g_cfg->E_FIRST_EXT-1);
	View->m_callInfo[cNum][bNum].elapsedTime 	= elapsedTime;

	g_phEngine->CalcCallValues(View->m_callInfo[cNum][bNum]);

	// take advantage of view to store booth display 2.21.1 build 5
	if (boothDisplay)
	{
		m_pDisplayInfos[boothCount].elapsedTime = View->m_callInfo[cNum][bNum].ceilMin;
		m_pDisplayInfos[boothCount].cost 		= View->m_callInfo[cNum][bNum].value;
	}
}

void CONTROLLER::ProcessPrepaid()
{
	for (int cNum=0; cNum<g_cfg->ACTIVE_CLUSTERS; ++cNum)
	{
		for (int bNum=0; bNum<CLUSTER_SIZE; ++bNum)
		{

			if (!RTEngine->GetPrePaid(cNum, bNum))
				continue ;

			// look we used 1L milliseconds to force to lock call.

			// check if the money was already spent
			if (
				g_cfg->MANUAL &&
				g_cfg->MULTIPLE_PREPAID_CALLS &&
				RTEngineGetNumOfCalls(cNum, bNum) > 0 &&
				RTEngineGetPreValue(cNum, bNum) == 0.0F
			)
			{
				RTEngine->SetPreTime(cNum, bNum, MIN_PREPAID_TIME);
				continue ;
			}

			long elapsedTime = RTEngine->GetElapsedCount(cNum, bNum);

			if (!elapsedTime && RTEngine->GetFinalElapsedCount(cNum, bNum))
			{
				// Call is hang but we need to refresh the equivalent to a Receipt
				elapsedTime = RTEngine->GetFinalElapsedCount(cNum, bNum);
			}

			if (!elapsedTime)
				continue ;

			double valuePerMin = View->m_callInfo[cNum][bNum].valuePerMin;

			if (!valuePerMin)
				continue ;

			// adjust tax (pre-paid and view) v.219b
			valuePerMin = valuePerMin*(1.0F+View->m_callInfo[cNum][bNum].taxPercent/100.0F);

			// calc the time for the prepaid
			double prepaidMin = (RTEngine->GetPreValue(cNum, bNum)/valuePerMin);

			// floor minutes JEF/gcc v.212.
			//   for all modes GC/gc v.219
			// check min time v.219.  AR/gc.
			WORD callAttrs   = RTEngine->GetCallAttr(cNum, bNum);
#if !defined(__EDA__)
			if (callAttrs == DDI_CALL)
			{ // include USA !!!
#else
			if (callAttrs == DDI_EDA2TEL_CALL)
			{ // include USA !!!
#endif
				if (g_cfg->CEIL_INTER != 0.0)
					prepaidMin = floor(prepaidMin);

				if (prepaidMin < g_cfg->MIN_INTER)
				{
					RTEngine->SetPreTime(cNum, bNum, MIN_PREPAID_TIME);
					continue ;
				}

			}
			else if (callAttrs == BORDER_CALL)
			{
				if (g_cfg->CEIL_BORDER != 0.0)
					prepaidMin = floor(prepaidMin);

				if (prepaidMin < g_cfg->MIN_BORDER)
				{
					RTEngine->SetPreTime(cNum, bNum, MIN_PREPAID_TIME);
					continue ;
				}
			}
			else if (callAttrs == CELLULAR_CALL)
			{
				if (g_cfg->CEIL_CELLULAR != 0.0)
					prepaidMin = floor(prepaidMin);

				if (prepaidMin < g_cfg->MIN_CELLULAR)
				{
					RTEngine->SetPreTime(cNum, bNum, MIN_PREPAID_TIME);
					continue ;
				}
			}
			else
			{ // DDN_CALL, LOCAL and EDA
				if (g_cfg->CEIL_NAL != 0.0)
					prepaidMin = floor(prepaidMin);

				if (prepaidMin < g_cfg->MIN_NAL)
				{
					RTEngine->SetPreTime(cNum, bNum, MIN_PREPAID_TIME);
					continue ;
				}
			}
			//
			// Translate minutes to msecs for the RTEngine (only the first time !)
			// remember RTEngine is based on an interrupt 0x08 !!!

			// optimize and check for possible glitches on call attr
			// 2.30 build 17 AR/gc
			long rtPreTime = RTEngine->GetPreTime(cNum, bNum);
			long preTime   = (long)(prepaidMin*(60.0F*1000.0F));
			if (rtPreTime == 0 || rtPreTime != preTime) // a glitch ?
			{
				RTEngine->SetPreTime(cNum, bNum, preTime);
			}

			// beep between 48 sec and 60 sec
			double remTime = prepaidMin*60 - elapsedTime/1000.0F;

			if (remTime > 48 && remTime <= 60 )
			{
				// don't activate if there is a SPY condition. v.219b
				if (RTEngine->GetSpyBooth() != -1 && g_cfg->NO_SOUND_WHILE_SPY)
				{
					BYTE spy = RTEngine->GetDataPortSpy(cNum);
					spy &= ~(1 << bNum); // close channel
					RTEngine->SetDataPortSpy(cNum, spy);

					BYTE generalPort = RTEngine->GetGeneralPort();
					generalPort &= (~GP_BEEP); // off
					RTEngine->SetGeneralPort(generalPort);
				}
				else
				{
					Alarms[cNum][bNum] = !Alarms[cNum][bNum];
					if (Alarms[cNum][bNum])
					{
						BYTE spy = RTEngine->GetDataPortSpy(cNum);
						spy |= (1 << bNum); // open booth channel
						RTEngine->SetDataPortSpy(cNum, spy);

						BYTE generalPort = RTEngine->GetGeneralPort();
						generalPort |= GP_BEEP; // on
						RTEngine->SetGeneralPort(generalPort);

						UI_ERROR_SYSTEM::Beep();
					}
					else
					{
						BYTE spy = RTEngine->GetDataPortSpy(cNum);
						spy &= ~(1 << bNum); // close channel
						RTEngine->SetDataPortSpy(cNum, spy);

						BYTE generalPort = RTEngine->GetGeneralPort();
						generalPort &= (~GP_BEEP); // off
						RTEngine->SetGeneralPort(generalPort);
					}
				}
			}
			else
			{
				// close current spy channel if another booth is using spy. JEF/gcc !!!
				WORD boothCount = cNum*CLUSTER_SIZE+bNum;

				if (boothCount != RTEngine->GetSpyBooth())
				{
					BYTE spy = RTEngine->GetDataPortSpy(cNum);
					spy &= ~(1 << bNum);
					RTEngine->SetDataPortSpy(cNum, spy);
				}

				// force the beep off (maybe the last time the beep was on)
				BYTE generalPort = RTEngine->GetGeneralPort();
				generalPort &= (~GP_BEEP); // off
				RTEngine->SetGeneralPort(generalPort);
			}
		}
	}
}

void CONTROLLER::checkWatchDog(int cNum, int bNum)
{
	WORD boothCount = cNum*CLUSTER_SIZE+bNum;
	long elapsedTime = RTEngine->GetElapsedCount(cNum, bNum);
	BOOL prePaid = RTEngine->GetPrePaid(cNum, bNum);
	long preTime = RTEngine->GetPreTime(cNum, bNum);

	if (prePaid && elapsedTime)
	{
		if (
			preTime > MIN_PREPAID_TIME &&
			elapsedTime > (preTime + PREPAID_THRESHOLD)
		)
		{
			watchDog = TRUE;
			// 2.33 build 3
			if (!TraceInfo::s_bTest)
			{
				sprintf(watchDogMessage,
					"El tiempo excede el prepago en cabina %d",
					boothCount+1
				);
			}
			else
			{
				sprintf(watchDogMessage,
					"El tiempo %-.3f s excede el prepago de %-3f s en cabina %d",
					elapsedTime/1000.0,
					preTime/1000.0,
					boothCount+1
				);
			}

		}
	}
}

void CONTROLLER::RefreshView(void)
{
    static UI_TIME lastTime;
	UI_TIME currentTime;
    if (lastTime == currentTime) // depend on BIOS resolution
		return ;

    lastTime.Import();
    static WORD boothCount = 0;
    WORD cNum = boothCount/CLUSTER_SIZE;
    WORD bNum = boothCount%CLUSTER_SIZE;
    //
    if (!g_ssaver->IsActive())
    {
        refreshStat(cNum, bNum);
        refreshArea(cNum, bNum);
        // Is an extension ?
		if (g_cfg->E_FIRST_EXT && boothCount >= (g_cfg->E_FIRST_EXT-1))
        {
			if (g_cfg->E_SHOW_PHONE)
                refreshPhone(cNum, bNum);
        }
        else
            refreshPhone(cNum, bNum);
        //
        refreshCity (cNum, bNum);
        refreshTar  (cNum, bNum);
        refreshETime(cNum, bNum);
        refreshValue(cNum, bNum);
        refreshCalls(cNum, bNum);
    }
    // one by step
	static WORD maxBooth = g_cfg->ACTIVE_CLUSTERS*CLUSTER_SIZE;
    boothCount = (boothCount+1)%maxBooth; // next booth
}

void CONTROLLER::refreshStat(WORD cNum, WORD bNum)
{
	WORD rtToneState  = RTEngine->GetToneFS(cNum, bNum);
	WORD rtPulseState = RTEngine->GetPulseFS(cNum, bNum);

	WORD viewToneState  = View->ToneFSs[cNum][bNum];
	WORD viewPulseState = View->PulseFSs[cNum][bNum];

	WORD state   = RT_ENGINE::LOCK;
	BOOL refresh = FALSE;
	//
	WORD boothCount = cNum*CLUSTER_SIZE+bNum;
	if (boothCount == RTEngine->GetLastSpyBooth())
	{
		refresh = TRUE; // restore after spy state
		RTEngine->SetLastSpyBooth(-1);
		state = rtPulseState;
		if (rtToneState > RT_ENGINE::OFFHOOK)
		{
			state = rtToneState;
		}
	}
	else
	{
		if (boothCount == RTEngine->GetSpyBooth())
		{
			// this is a special case is only for RT_ENGINE::SPY because we need to Refresh
			// only the icon, but, the RTEngine continues to work without knowing it
			refresh = TRUE;
			state = RT_ENGINE::SPY;
		}
		else if (viewToneState != rtToneState || viewPulseState != rtPulseState)
		{
			viewToneState  = rtToneState;
			viewPulseState = rtPulseState;
			refresh = TRUE;

			if (rtToneState > RT_ENGINE::OFFHOOK)
			{ // tone
				state = rtToneState;
				switch (rtToneState)
				{
				case RT_ENGINE::DTMFFLAG:
				case RT_ENGINE::INTERDIG:
				case RT_ENGINE::ANSWER:
					{
						state = RT_ENGINE::NAL;
						if (RTEngine->GetCallAttr(cNum, bNum) & INTERNATIONAL_CALL_MASK )
						{
							state = RT_ENGINE::INTER;
							viewToneState = RT_ENGINE::INTER;
						}
						break;
					}
				}
			}
			else
			{ // pulse
				state = rtPulseState;
				switch (rtPulseState)
				{
				case RT_ENGINE::BREAK:
				case RT_ENGINE::MAKE:
				case RT_ENGINE::INTERDIG:
				case RT_ENGINE::ANSWER:
					{
						state = RT_ENGINE::NAL;
						if (RTEngine->GetCallAttr(cNum, bNum) & INTERNATIONAL_CALL_MASK)
						{
							state = RT_ENGINE::INTER;
							viewToneState = RT_ENGINE::INTER;
						}
						break;
					}
				}
			}

			View->ToneFSs[cNum][bNum]  = viewToneState;
			View->PulseFSs[cNum][bNum] = viewPulseState;
		}
	}
	//
	if (state < RT_ENGINE::END && refresh)
	{
		View->WStates[cNum][bNum]->Information(SET_BITMAP_ARRAY, View->States[state].Bitmap);
		View->WStates[cNum][bNum]->DataSet(View->States[state].Text);
	}
}

void CONTROLLER::refreshArea(WORD cNum, WORD bNum)
{
	if (strcmp(View->WAreas[cNum][bNum]->DataGet(), View->m_callInfo[cNum][bNum].area))
		View->WAreas[cNum][bNum]->DataSet(View->m_callInfo[cNum][bNum].area);
}

void CONTROLLER::refreshPhone(WORD cNum, WORD bNum)
{
	if (strcmp(View->WPhones[cNum][bNum]->DataGet(), View->m_callInfo[cNum][bNum].phone))
		View->WPhones[cNum][bNum]->DataSet(View->m_callInfo[cNum][bNum].phone);
}

void CONTROLLER::refreshCity(WORD cNum, WORD bNum)
{
	if (strcmp(View->WCities[cNum][bNum]->DataGet(), View->m_callInfo[cNum][bNum].city))
		View->WCities[cNum][bNum]->DataSet(View->m_callInfo[cNum][bNum].city);
}

void CONTROLLER::refreshTar(WORD cNum, WORD bNum)
{
	if (View->WTariffs[cNum][bNum]->DataGet() != View->m_callInfo[cNum][bNum].nTariff)
	{
		View->WTariffs[cNum][bNum]->DataSet(&View->m_callInfo[cNum][bNum].nTariff);
	}
}

void CONTROLLER::refreshETime(WORD cNum, WORD bNum)
{
	double time =
		g_cfg->CALL_ACTUAL_COST?View->m_callInfo[cNum][bNum].ceilMin:View->m_callInfo[cNum][bNum].rawMin;

	if
	(
		View->WElapsedTimes[cNum][bNum]->DataGet() != View->m_callInfo[cNum][bNum].time &&
		RTEngine->GetToneFS(cNum, bNum) >= RT_ENGINE::ONHOOK
	)
	{
		View->WElapsedTimes[cNum][bNum]->DataSet(&time);
	}
}

void CONTROLLER::refreshCalls(WORD cNum, WORD bNum)
{
	int numOfCalls = RTEngine->GetNumOfCalls(cNum, bNum);
	if (View->WNumOfCalls[cNum][bNum]->DataGet() != numOfCalls)
	{
		View->WNumOfCalls[cNum][bNum]->DataSet(&numOfCalls);
	}
}

void CONTROLLER::refreshValue(WORD cNum, WORD bNum)
{
	if
	(
		*View->WValues[cNum][bNum]->DataGet() != View->m_callInfo[cNum][bNum].value &&
		RTEngine->GetToneFS(cNum, bNum) >= RT_ENGINE::ONHOOK
	)
	{
		View->WValues[cNum][bNum]->DataSet(&UI_BIGNUM(View->m_callInfo[cNum][bNum].value));
	}
}

void CONTROLLER::RefreshBoothDisplay(void)
{
	if (!boothDisplay)
		return ;

	static UI_TIME lastTime;
	UI_TIME currentTime;
	if (lastTime == currentTime) // depend on BIOS resolution
		return ;

	lastTime.Import();

	static WORD boothCount = 0;
	static WORD cNum, bNum;
	static WORD state;

	cNum = boothCount/CLUSTER_SIZE;
	bNum = boothCount%CLUSTER_SIZE;

	// we use the view values to change the display
	state = RTEngine->GetUnifiedState(cNum, bNum);

	switch (state)
	{
	case RT_ENGINE::LOCK:
		if (g_cfg->MANUAL && manualInfo[boothCount].numOfCalls)
		{
			m_pDisplayInfos[boothCount].elapsedTime = 0;
			m_pDisplayInfos[boothCount].cost        = 0;
			m_pDisplayInfos[boothCount].numOfCalls  = manualInfo[boothCount].numOfCalls;
			m_pDisplayInfos[boothCount].totalCost   = manualInfo[boothCount].totalCost;

			// 2.21.7: use new command
			boothDisplay->showLastRefresh(m_pDisplayInfos[boothCount]);
		}
		else
		{
			// 2.21.7: use new command
			m_pDisplayInfos[boothCount].numOfCalls = manualInfo[boothCount].numOfCalls;
			m_pDisplayInfos[boothCount].totalCost  = manualInfo[boothCount].totalCost + m_pDisplayInfos[boothCount].cost;

			boothDisplay->showLastRefresh(m_pDisplayInfos[boothCount]);
		}
		break;
	case RT_ENGINE::OFFHOOK:
		boothDisplay->showOffHook(m_pDisplayInfos[boothCount]);
		break;
	case RT_ENGINE::SPY:
		boothDisplay->showSpy(m_pDisplayInfos[boothCount]);
		break;
	case RT_ENGINE::DIALERR:
		boothDisplay->showDialErr(m_pDisplayInfos[boothCount]);
		break;
	case RT_ENGINE::COMERR:
		boothDisplay->showCommErr(m_pDisplayInfos[boothCount]);
		break;
	case RT_ENGINE::NAL:
	case RT_ENGINE::INTER:
		PHONE phone;
		strncpy(phone, View->WAreas[cNum][bNum]->DataGet(), sizeof(phone)-1);
		phone[sizeof(phone)-1] = '\0';
		strncat(phone, View->WPhones[cNum][bNum]->DataGet(), sizeof(phone)-1-strlen(phone));
		//
		strcpy(m_pDisplayInfos[boothCount].phone, phone);
		// trunc -> 16. v.2.20.2
		strncpy(m_pDisplayInfos[boothCount].cityName, View->m_callInfo[cNum][bNum].city, 16);
		m_pDisplayInfos[boothCount].cityName[16] = '\0';
		_ISO2ASCII2(m_pDisplayInfos[boothCount].cityName);

		boothDisplay->showDialing(m_pDisplayInfos[boothCount]);
		break;
	case RT_ENGINE::TALK:
	case RT_ENGINE::STORE:
		m_pDisplayInfos[boothCount].numOfCalls  = manualInfo[boothCount].numOfCalls;
		m_pDisplayInfos[boothCount].totalCost   = manualInfo[boothCount].totalCost + m_pDisplayInfos[boothCount].cost;
		//
		boothDisplay->showComm(m_pDisplayInfos[boothCount]);
		break;
	default:
		// "Lib" or another non catched state
		if (g_cfg->MANUAL && manualInfo[boothCount].numOfCalls)
		{
			m_pDisplayInfos[boothCount].elapsedTime = 0.0;
			m_pDisplayInfos[boothCount].cost        = 0.0;
			m_pDisplayInfos[boothCount].numOfCalls  = manualInfo[boothCount].numOfCalls;
			m_pDisplayInfos[boothCount].totalCost   = manualInfo[boothCount].totalCost;

			boothDisplay->showComm(m_pDisplayInfos[boothCount]);
		}
		else
		{
			boothDisplay->showOnHook(m_pDisplayInfos[boothCount]);
		}
	}

	// one step at a time
	static WORD maxBooth = g_cfg->ACTIVE_CLUSTERS*CLUSTER_SIZE;
	boothCount = (boothCount+1)%maxBooth;  // next booth
}
