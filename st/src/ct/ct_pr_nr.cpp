//
// [ CT_PR_NR.CPP ]
//

#include "stdst.h"

#include <control.h>
#include <db_eng.h>
#include <ph_eng.h>
#include <spooler.h>
#include <info.h>

extern CFG        	*g_cfg;
extern PH_ENGINE 	*g_phEngine;
extern SPOOLER    	*g_spooler;
extern DB_ENGINE  	*g_dbEngine;
extern APP_INFO   	g_appInfo;

void CONTROLLER::PrintNData(DynamicReceipt& dynReceipt)
{
	BYTE channel = (!g_cfg->DOUBLE_PRN)?0:dynReceipt.m_r.BoothNumber%DS_MAXDOUBLEPRNENTRIES;
    char *paidAttr = "";
	switch (dynReceipt.m_r.Stat.Paid)
    {
    case NOT_PAID_CALL :
        paidAttr = "(NC)";
		break;
	case TOLL_FREE_CALL:
		paidAttr = "(PR)";
		break;
	}
	//
	int hour, minutes, seconds;
	g_Milisec2Time(dynReceipt.m_r.ElapsedTime, hour, minutes, seconds);
	UI_TIME time(hour, minutes, seconds);

	char elapsedStr[0x10];
	time.Export(elapsedStr, TMF_TWENTY_FOUR_HOUR|TMF_SECONDS);
	//
	BOOL mirrored;
	mirrored =
	(
		g_cfg->FORM == CFG::DR_EME    ||
		g_cfg->FORM == CFG::DR_HALF   ||
		g_cfg->FORM == CFG::DR_PRE    ||
		g_cfg->FORM == CFG::DR_80     ||
		g_cfg->FORM == CFG::DR_18
	);
	//
	char *fmt;
	prnFormatter->nrGetFmt(&fmt);
	if (g_cfg->FORM == CFG::LINEAL_80)
	{
		PrintNLINEAL_80(dynReceipt, fmt);
		return ;
	}

	STR16 szRecno;
	g_Number2Str(szRecno, dynReceipt.m_r.Number, g_cfg->RECNO_DIGITS, g_cfg->RECNO_LEADING_ZEROS);

	if (g_cfg->FORM == CFG::SR_40 || g_cfg->FORM == CFG::DR_40)
	{
		g_spooler->Print(channel, "\x1B\x21\x00\xFF", TRUE);
	}
	// 2.21.8 -> See below to change g_cfg->TAX_PERCENT
	double taxPercent;
	taxPercent = (dynReceipt.m_r.Tax)?(dynReceipt.m_r.Tax/(dynReceipt.m_r.Value-dynReceipt.m_r.Tax))*100:0;

	// print
	if (!mirrored)
	{
		g_spooler->Printf
		(
			channel, fmt,
			g_cfg->RECNO_LABEL, szRecno, g_cfg->BoothInfo[dynReceipt.m_r.BoothNumber].Name,
			dynReceipt.m_r.Phone, dynReceipt.m_r.City, elapsedStr,
			dynReceipt.m_r.CeilMin,
			g_cfg->CURRENCY, dynReceipt.m_r.ValuePerMin, dynReceipt.m_r.Percent,
			//
			g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax,
			g_cfg->TAX_NAME, taxPercent, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
			g_cfg->CURRENCY, dynReceipt.m_r.Value, paidAttr
		);

		return ;
	}

	if (g_cfg->FORM == CFG::DR_18)
	{
		g_spooler->Printf
		(
			channel, fmt,
			g_cfg->RECNO_LABEL, szRecno, g_cfg->RECNO_LABEL, szRecno,
			g_cfg->BoothInfo[dynReceipt.m_r.BoothNumber].Name, g_cfg->BoothInfo[dynReceipt.m_r.BoothNumber].Name,
			dynReceipt.m_r.Phone, dynReceipt.m_r.Phone,
			dynReceipt.m_r.City, dynReceipt.m_r.City,
			elapsedStr, elapsedStr,
			dynReceipt.m_r.CeilMin, dynReceipt.m_r.CeilMin,
			g_cfg->CURRENCY, dynReceipt.m_r.ValuePerMin, dynReceipt.m_r.Percent, g_cfg->CURRENCY, dynReceipt.m_r.ValuePerMin, dynReceipt.m_r.Percent,
			//
			g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax, g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax,
			g_cfg->TAX_NAME, taxPercent, g_cfg->CURRENCY, dynReceipt.m_r.Tax, g_cfg->TAX_NAME, taxPercent, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
			g_cfg->CURRENCY, dynReceipt.m_r.Value, paidAttr, g_cfg->CURRENCY, dynReceipt.m_r.Value, paidAttr
		);
	}
	else if (g_cfg->FORM == CFG::DR_EME)
	{
		g_spooler->Printf
		(
			channel, fmt,
			g_cfg->BoothInfo[dynReceipt.m_r.BoothNumber].Name, g_cfg->BoothInfo[dynReceipt.m_r.BoothNumber].Name,
			dynReceipt.m_r.Phone, dynReceipt.m_r.Phone,
			dynReceipt.m_r.City, dynReceipt.m_r.City,
			g_cfg->RECNO_LABEL, szRecno, g_cfg->RECNO_LABEL, szRecno,
			elapsedStr, elapsedStr,
			dynReceipt.m_r.CeilMin, dynReceipt.m_r.CeilMin,
			g_cfg->CURRENCY, dynReceipt.m_r.ValuePerMin, dynReceipt.m_r.Percent, g_cfg->CURRENCY, dynReceipt.m_r.ValuePerMin, dynReceipt.m_r.Percent,
			g_cfg->CURRENCY, dynReceipt.m_r.Value, paidAttr, g_cfg->CURRENCY, dynReceipt.m_r.Value, paidAttr
		);
	}
	else if (g_cfg->FORM == CFG::DR_HALF)
	{
		g_spooler->Printf
		(
			channel, fmt,
			g_cfg->RECNO_LABEL, szRecno, g_cfg->BoothInfo[dynReceipt.m_r.BoothNumber].Name, g_cfg->RECNO_LABEL, szRecno, g_cfg->BoothInfo[dynReceipt.m_r.BoothNumber].Name,
			dynReceipt.m_r.Phone, dynReceipt.m_r.City, elapsedStr, dynReceipt.m_r.Phone, dynReceipt.m_r.City, elapsedStr,
			dynReceipt.m_r.CeilMin, dynReceipt.m_r.CeilMin,
			g_cfg->CURRENCY, dynReceipt.m_r.ValuePerMin, dynReceipt.m_r.Percent, g_cfg->CURRENCY, dynReceipt.m_r.ValuePerMin, dynReceipt.m_r.Percent,
			g_cfg->CURRENCY, dynReceipt.m_r.Value, paidAttr, g_cfg->CURRENCY, dynReceipt.m_r.Value, paidAttr
		);
	}
	else if (g_cfg->FORM == CFG::DR_80)
	{
		g_spooler->Printf
		(
			channel, fmt,
			g_cfg->RECNO_LABEL, szRecno, g_cfg->BoothInfo[dynReceipt.m_r.BoothNumber].Name, g_cfg->RECNO_LABEL, szRecno, g_cfg->BoothInfo[dynReceipt.m_r.BoothNumber].Name,
			dynReceipt.m_r.Phone, dynReceipt.m_r.City, elapsedStr, dynReceipt.m_r.Phone, dynReceipt.m_r.City, elapsedStr,
			dynReceipt.m_r.CeilMin, dynReceipt.m_r.CeilMin,
			g_cfg->CURRENCY, dynReceipt.m_r.ValuePerMin, dynReceipt.m_r.Percent, g_cfg->CURRENCY, dynReceipt.m_r.ValuePerMin, dynReceipt.m_r.Percent,
			//
			g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax, g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax,
			g_cfg->TAX_NAME, taxPercent, g_cfg->CURRENCY, dynReceipt.m_r.Tax, g_cfg->TAX_NAME, taxPercent, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
			g_cfg->CURRENCY, dynReceipt.m_r.Value, paidAttr, g_cfg->CURRENCY, dynReceipt.m_r.Value, paidAttr
		);
	}
	else if (g_cfg->FORM == CFG::DR_PRE)
	{
		g_spooler->Printf
		(
			channel, fmt,
			g_cfg->RECNO_LABEL, szRecno, g_cfg->BoothInfo[dynReceipt.m_r.BoothNumber].Name, g_cfg->RECNO_LABEL, szRecno, g_cfg->BoothInfo[dynReceipt.m_r.BoothNumber].Name,
			dynReceipt.m_r.Phone, dynReceipt.m_r.City, elapsedStr, dynReceipt.m_r.Phone, dynReceipt.m_r.City, elapsedStr,
			dynReceipt.m_r.CeilMin, g_cfg->CURRENCY, dynReceipt.m_r.ValuePerMin, dynReceipt.m_r.Percent,
			dynReceipt.m_r.CeilMin, g_cfg->CURRENCY, dynReceipt.m_r.ValuePerMin, dynReceipt.m_r.Percent,
			//
			g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax, g_cfg->TAX_NAME, taxPercent, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
			g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax, g_cfg->TAX_NAME, taxPercent, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
			g_cfg->CURRENCY, dynReceipt.m_r.Value, paidAttr, g_cfg->CURRENCY, dynReceipt.m_r.Value, paidAttr
		);
	}
}

void CONTROLLER::PrintNLINEAL_80(DynamicReceipt& dynReceipt, const char *fmt)
{
	BYTE channel = (!g_cfg->DOUBLE_PRN)?0:dynReceipt.m_r.BoothNumber%DS_MAXDOUBLEPRNENTRIES;

	// date & time
	char dateStr[0x10], timeStr[0x10];
	UI_DATE date(dynReceipt.m_r.Date);
	date.Export(dateStr, DTF_ZERO_FILL|DTF_SLASH|DTF_EUROPEAN_FORMAT);
	UI_TIME time(dynReceipt.m_r.Time);
	time.Export(timeStr, TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL|TMF_SECONDS);

	STR16 szRecno;
	g_Number2Str(szRecno, dynReceipt.m_r.Number, g_cfg->RECNO_DIGITS, g_cfg->RECNO_LEADING_ZEROS);

	int hour, minutes, seconds;
	g_Milisec2Time(dynReceipt.m_r.ElapsedTime, hour, minutes, seconds);
	time.Import(hour, minutes, seconds);

	char elapsedStr[0x10];
    time.Export(elapsedStr, TMF_TWENTY_FOUR_HOUR|TMF_SECONDS);
    //
	g_spooler->Printf
	(
		channel, fmt,
		szRecno,
		dynReceipt.m_r.BoothNumber+1,
		dateStr,
		timeStr,
		dynReceipt.m_r.Phone,
		dynReceipt.m_r.City,
		elapsedStr,
		dynReceipt.m_r.CeilMin,
		dynReceipt.m_r.ValuePerMin, dynReceipt.m_r.Percent,
		g_cfg->CURRENCY,
		dynReceipt.m_r.Value
	);
}
