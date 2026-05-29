//
// [ CT_UTIL.CPP ]
//

#include "stdst.h"

#include <control.h>
#include <db_eng.h>
#include <ph_eng.h>
#include <spooler.h>
#include <info.h>

extern CFG        	*g_cfg;
extern PH_ENGINE  	*g_phEngine;
extern SPOOLER    	*g_spooler;
extern DB_ENGINE  	*g_dbEngine;
extern APP_INFO    	g_appInfo;

void CONTROLLER::activateCash(UI_TIME& lastTime, WORD lastHundreth, WORD currentHundreth)
{
    BYTE channel = 0;
    static WORD cashCount = 0;
	if (!strcmp(g_cfg->CASH, "prn"))
    {
		switch (g_cfg->FORM)
        {
		case g_cfg->CFG::DR_40:
        case CFG::SR_28:
		case g_cfg->CFG::SR_40:
			g_spooler->Print(channel, "\x1B\x70\x00\x0A\0x0A\xFF", TRUE);
            break; // TM
        case CFG::DR_18:
            g_spooler->Print(channel,  "\x1C");
            ;
            break; // SP
        }


        CashReq = FALSE;
    }
	else if (!strcmp(g_cfg->CASH, "com1"))
    {
        if (cashCount < 0x400)
        { // 0x400 times to activate serial port cash
            outportb(0x3F8, 0x00); // send via UART Rx/Tx register
            cashCount++;
        }
        else
        {
            cashCount = 0;
            CashReq = FALSE;
        }
    }
	else if (!strcmp(g_cfg->CASH, "com2"))
    {
        if (cashCount < 0x400)
        { // 0x400 times to activate serial port cash
            outportb(0x2F8, 0x00); // send via UART Rx/Tx register
            cashCount++;
        }
        else
        {
            cashCount = 0;
            CashReq = FALSE;
		}
	}
	else
	{
		// via STM-2
		if (lastHundreth != currentHundreth)
		{
			lastTime.Import();

			if (cashCount < 10)
			{
				BYTE generalPort = RTEngine->GetGeneralPort();
				generalPort |= GP_CASH;
				RTEngine->SetGeneralPort(generalPort);
				cashCount++;
			}
			else
			{
				cashCount = 0;
				CashReq = FALSE;
				BYTE generalPort = RTEngine->GetGeneralPort();
				generalPort &= ~GP_CASH;
				RTEngine->SetGeneralPort(generalPort);
			}
		}
	}
}

void CONTROLLER::CookReceipt(DynamicReceipt& dynReceipt)
{
	//
	// since the RTEngine needs a high trouhput the DController cook the Receipt
	//
	// --- city
	PH_ENGINE::PLACE_ENTRY 		placeEntry;
	PH_ENGINE::CALL_PARAMETERS 	parameters;
	g_phEngine->Search(dynReceipt.m_r.Phone, placeEntry, parameters);

	dynReceipt.Area_ = parameters.AreaCode;
	strcpy(dynReceipt.m_r.City, (char *)placeEntry.Place);

	PH_ENGINE::CallInfo info;

	strcpy(info.city, dynReceipt.m_r.City);
	strcpy(info.phone, dynReceipt.m_r.Phone);
	info.date			= dynReceipt.m_r.Date;
	info.time 			= dynReceipt.m_r.Time;
	info.nTariff		= dynReceipt.m_r.Tariff;
	info.callAttr 		= dynReceipt.m_r.Stat.CallAttr;
	info.isExtension 	= dynReceipt.m_r.Stat.Extension;
	info.elapsedTime	= dynReceipt.m_r.ElapsedTime;

	g_phEngine->CalcCallValues(info);
	//
	dynReceipt.DecTime_ 		= info.ceilMin;
	dynReceipt.m_r.CeilMin 		= dynReceipt.DecTime_;
	dynReceipt.m_r.Percent 		= info.paidPercent;
	dynReceipt.m_r.ValuePerMin	= info.valuePerMin;
	dynReceipt.m_r.Value		= info.value;
	dynReceipt.m_r.Tax			= info.tax;
	//
	// money back
	// to avoid a manual dynReceipt, since it's processed in UIW_MANUAL
	if (!dynReceipt.m_r.Stat.Manual)
	{
		dynReceipt.MoneyBack_ = dynReceipt.PreValue_ - dynReceipt.m_r.Value;
	}
	else
	{
		if (g_cfg->MANUAL && g_cfg->MULTIPLE_PREPAID_CALLS)
		{ // v.219
			// simulate a continuos prepaid v.219
			int    cNum  = dynReceipt.m_r.BoothNumber/CLUSTER_SIZE;
			int    bNum  = dynReceipt.m_r.BoothNumber%CLUSTER_SIZE;
			double value = RTEngine->GetPreValue(cNum, bNum) - dynReceipt.m_r.Value;

			/*
			if (TraceInfo::s_bTest)
			{
				char msg[128];
				sprintf(msg, "Value=%.2f=%.2f-%.2f\r\n",
					value,
					RTEngine->GetPreValue(cNum, bNum),
					dynReceipt.m_r.Value);
				UI_WINDOW_OBJECT::errorSystem->ReportError(
					UI_WINDOW_OBJECT::windowManager, WOS_NO_STATUS, msg);
			}
			*/

			// only positive values
			if (value >= 0)
			{
				RTEngine->SetPreValue(cNum, bNum, value);
				RTEngine->SetPrePaid (cNum, bNum, TRUE);  // still prepaid
			}
		}
		// v.2.20.2
		manualInfo[dynReceipt.m_r.BoothNumber].numOfCalls++;
		manualInfo[dynReceipt.m_r.BoothNumber].totalCost += dynReceipt.m_r.Value;
	}

	dynReceipt.m_r.Stat.Cooked = TRUE;
}

BOOL CONTROLLER::RTEngineIsBusy(void)
{
	return RTEngine->IsBusy();
}

BOOL CONTROLLER::RTEngineIsDemo(void)
{
	return RTEngine->IsDemo();
}

void CONTROLLER::RTEngineToggleDemoPause(void)
{
	RTEngine->TogglePaused();
}

BOOL CONTROLLER::RTEngineIsDemoPaused(void)
{
	return RTEngine->IsPaused();
}

WORD CONTROLLER::RTEngineGetNumOfCalls(WORD cNum, WORD bNum)
{
	return RTEngine->GetNumOfCalls(cNum, bNum);
}

BOOL CONTROLLER::RTEngineIsBoothBusy(WORD cNum, WORD bNum)
{
	return RTEngine->IsBoothBusy(cNum, bNum);
}

double CONTROLLER::RTEngineGetPreValue(WORD cNum, WORD bNum)
{
	return RTEngine->GetPreValue(cNum, bNum);
}

void CONTROLLER::RTEngineSetPreValue(WORD cNum, WORD bNum, double value)
{
	RTEngine->SetPreValue(cNum, bNum, value);
}

double CONTROLLER::RTEngineGetFirstPreValue(WORD cNum, WORD bNum)
{
	return RTEngine->GetFirstPreValue(cNum, bNum);
}

void CONTROLLER::RTEngineSetFirstPreValue(WORD cNum, WORD bNum, double value)
{
	RTEngine->SetFirstPreValue(cNum, bNum, value);
}

BOOL CONTROLLER::RTEngineGetPrePaid(WORD cNum, WORD bNum)
{
	return RTEngine->GetPrePaid(cNum, bNum);
}

void CONTROLLER::RTEngineSetPrePaid(WORD cNum, WORD bNum, BOOL value)
{
	RTEngine->SetPrePaid(cNum, bNum, value);
}

DWORD CONTROLLER::RTEngineGetPreTime(WORD cNum, WORD bNum)
{
	return RTEngine->GetPreTime(cNum, bNum);
}

void CONTROLLER::RTEngineSetPreTime(WORD cNum, WORD bNum, DWORD value)
{
	RTEngine->SetPreTime(cNum, bNum, value);
}

