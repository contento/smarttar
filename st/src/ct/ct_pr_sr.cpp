//
// [ CT_PR_SR.CPP ]
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
extern APP_INFO   	g_appInfo;

void CONTROLLER::PrintSTelData(DynamicReceipt& dynReceipt)
{
	BYTE channel = 0;

	int hour, minutes, seconds;
	g_Milisec2Time(dynReceipt.m_r.ElapsedTime, hour, minutes, seconds);
	UI_TIME time(hour, minutes, seconds);

	char elapsedStr[0x10];
	time.Export(elapsedStr, TMF_TWENTY_FOUR_HOUR|TMF_SECONDS);
	char *fmt;
	prnFormatter->srGetTelFmt(&fmt);
	if (g_cfg->FORM == CFG::LINEAL_80)
	{
		PrintSTelLINEAL_80(dynReceipt, fmt);
	}
	else
	{
		BOOL mirrored;
		mirrored =
		(
			g_cfg->FORM == CFG::DR_EME    ||
			g_cfg->FORM == CFG::DR_HALF   ||
			g_cfg->FORM == CFG::DR_PRE    ||
			g_cfg->FORM == CFG::DR_80     ||
			g_cfg->FORM == CFG::DR_18
		);
		if (g_cfg->FORM == CFG::SR_40 || g_cfg->FORM == CFG::DR_40 || g_cfg->FORM == CFG::SR_28)
		{
			g_spooler->Print(channel, "\x1B\x21\x00\xFF", TRUE);
		}
		// print
		// 2.21.8 -> See below to change g_cfg->TAX_PERCENT
		double taxPercent;
		taxPercent = (dynReceipt.m_r.Tax)?(dynReceipt.m_r.Tax/(dynReceipt.m_r.Value-dynReceipt.m_r.Tax))*100:0;

		STR16 szRecno;
		g_Number2Str(szRecno, dynReceipt.m_r.Number, g_cfg->RECNO_DIGITS, g_cfg->RECNO_LEADING_ZEROS);

		if (mirrored)
		{
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
					g_cfg->CURRENCY, dynReceipt.m_r.Value, g_cfg->CURRENCY, dynReceipt.m_r.Value
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
					g_cfg->CURRENCY, dynReceipt.m_r.Value, g_cfg->CURRENCY, dynReceipt.m_r.Value
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
					g_cfg->CURRENCY, dynReceipt.m_r.Value, g_cfg->CURRENCY, dynReceipt.m_r.Value
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
					g_cfg->CURRENCY, dynReceipt.m_r.Value, g_cfg->CURRENCY, dynReceipt.m_r.Value
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
					g_cfg->CURRENCY, dynReceipt.m_r.Value, g_cfg->CURRENCY, dynReceipt.m_r.Value
				);
            }
        }
        else
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
				g_cfg->CURRENCY, dynReceipt.m_r.Value
			);
		}
    }
}

void CONTROLLER::PrintFaxData(DynamicReceipt& dynReceipt)
{
	char *fmt;
	prnFormatter->srGetFaxFmt(&fmt);
	if (g_cfg->FORM == CFG::LINEAL_80)
	{
		PrintFaxLINEAL_80(dynReceipt, fmt);
	}
	else
	{
		BYTE channel = 0;
		BOOL mirrored;

		STR16 szRecno;
		g_Number2Str(szRecno, dynReceipt.m_r.Number, g_cfg->RECNO_DIGITS, g_cfg->RECNO_LEADING_ZEROS);

		mirrored =
		(
			g_cfg->FORM == CFG::DR_EME    ||
			g_cfg->FORM == CFG::DR_HALF   ||
			g_cfg->FORM == CFG::DR_PRE    ||
			g_cfg->FORM == CFG::DR_80     ||
			g_cfg->FORM == CFG::DR_18
		);
        // print
        if (mirrored)
        {
			if (g_cfg->FORM == CFG::DR_18)
            {
				g_spooler->Printf
				(
					channel, fmt,
					g_cfg->RECNO_LABEL, szRecno, g_cfg->RECNO_LABEL, szRecno,
					dynReceipt.m_r.Phone, dynReceipt.m_r.Phone,
					dynReceipt.m_r.City, dynReceipt.m_r.City,
					dynReceipt.m_r.Amount, dynReceipt.m_r.Amount,
					g_cfg->CURRENCY, dynReceipt.m_r.UnitaryValue, g_cfg->CURRENCY, dynReceipt.m_r.UnitaryValue,
					//
					g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax, g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax,
					g_cfg->TAX_NAME, g_cfg->TAX_PERCENT, g_cfg->CURRENCY, dynReceipt.m_r.Tax, g_cfg->TAX_NAME, g_cfg->TAX_PERCENT, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
					g_cfg->CURRENCY, dynReceipt.m_r.Value, g_cfg->CURRENCY, dynReceipt.m_r.Value
				);
            }
			else if (g_cfg->FORM == CFG::DR_PRE)
            {
				g_spooler->Printf
				(
					channel, fmt,
					g_cfg->RECNO_LABEL, szRecno, g_cfg->RECNO_LABEL, szRecno,
					dynReceipt.m_r.Phone, dynReceipt.m_r.City, dynReceipt.m_r.Phone, dynReceipt.m_r.City,
					dynReceipt.m_r.Amount, g_cfg->CURRENCY, dynReceipt.m_r.UnitaryValue,
					dynReceipt.m_r.Amount,	g_cfg->CURRENCY, dynReceipt.m_r.UnitaryValue,
					//
					g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax, g_cfg->TAX_NAME, g_cfg->TAX_PERCENT, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
					g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax, g_cfg->TAX_NAME, g_cfg->TAX_PERCENT, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
					g_cfg->CURRENCY, dynReceipt.m_r.Value, g_cfg->CURRENCY, dynReceipt.m_r.Value
				);
            }
            else
            { // DR_80, DR_EME, DR_HALF
				g_spooler->Printf
				(
					channel, fmt,
					g_cfg->RECNO_LABEL, szRecno, g_cfg->RECNO_LABEL, szRecno,
					dynReceipt.m_r.Phone, dynReceipt.m_r.City, dynReceipt.m_r.Phone, dynReceipt.m_r.City,
					dynReceipt.m_r.Amount, dynReceipt.m_r.Amount,
					g_cfg->CURRENCY, dynReceipt.m_r.UnitaryValue, g_cfg->CURRENCY, dynReceipt.m_r.UnitaryValue,
					//
					g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax, g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax,
					g_cfg->TAX_NAME, g_cfg->TAX_PERCENT, g_cfg->CURRENCY, dynReceipt.m_r.Tax, g_cfg->TAX_NAME, g_cfg->TAX_PERCENT, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
					g_cfg->CURRENCY, dynReceipt.m_r.Value, g_cfg->CURRENCY, dynReceipt.m_r.Value
				);
            }
        }
        else
        {
			g_spooler->Printf
			(
				channel, fmt,
				g_cfg->RECNO_LABEL, szRecno,
				dynReceipt.m_r.Phone, dynReceipt.m_r.City,
				dynReceipt.m_r.Amount,
				g_cfg->CURRENCY, dynReceipt.m_r.UnitaryValue,
				//
				g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax,
				g_cfg->TAX_NAME, g_cfg->TAX_PERCENT, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
				g_cfg->CURRENCY, dynReceipt.m_r.Value
			);
        }
    }
}

void CONTROLLER::PrintTelexData(DynamicReceipt& dynReceipt)
{
	char *fmt;
	prnFormatter->srGetTelexFmt(&fmt);

	if (g_cfg->FORM == CFG::LINEAL_80)
	{
		PrintTelexLINEAL_80(dynReceipt, fmt);
		return ;
	}

	BYTE channel = 0;
	BOOL mirrored;
	mirrored =
	(
		g_cfg->FORM == CFG::DR_EME    ||
		g_cfg->FORM == CFG::DR_HALF   ||
		g_cfg->FORM == CFG::DR_PRE    ||
		g_cfg->FORM == CFG::DR_80     ||
		g_cfg->FORM == CFG::DR_18
	);

	STR16 szRecno;
	g_Number2Str(szRecno, dynReceipt.m_r.Number, g_cfg->RECNO_DIGITS, g_cfg->RECNO_LEADING_ZEROS);

	// print
	if (mirrored)
	{
		if (g_cfg->FORM == CFG::DR_18)
		{
			g_spooler->Printf
			(
				channel, fmt,
				g_cfg->RECNO_LABEL, szRecno, g_cfg->RECNO_LABEL, szRecno,
				dynReceipt.m_r.Minutes, dynReceipt.m_r.Minutes,
				dynReceipt.m_r.CeilMin, dynReceipt.m_r.CeilMin,
				g_cfg->CURRENCY, dynReceipt.m_r.UnitaryValue, g_cfg->CURRENCY, dynReceipt.m_r.UnitaryValue,
				//
				g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax, g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax,
				g_cfg->TAX_NAME, g_cfg->INTERNET_TAX, g_cfg->CURRENCY, dynReceipt.m_r.Tax, g_cfg->TAX_NAME, g_cfg->INTERNET_TAX, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
				g_cfg->CURRENCY, dynReceipt.m_r.Value, g_cfg->CURRENCY, dynReceipt.m_r.Value
			);
		}
		else if (g_cfg->FORM == CFG::DR_PRE)
		{
			g_spooler->Printf
			(
				channel, fmt,
				g_cfg->RECNO_LABEL, szRecno, g_cfg->RECNO_LABEL, szRecno,
				dynReceipt.m_r.Minutes, dynReceipt.m_r.CeilMin, g_cfg->CURRENCY, dynReceipt.m_r.UnitaryValue,
				dynReceipt.m_r.Minutes, dynReceipt.m_r.CeilMin, g_cfg->CURRENCY, dynReceipt.m_r.UnitaryValue,
				//
				g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax, g_cfg->TAX_NAME, g_cfg->INTERNET_TAX, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
				g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax, g_cfg->TAX_NAME, g_cfg->INTERNET_TAX, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
				g_cfg->CURRENCY, dynReceipt.m_r.Value, g_cfg->CURRENCY, dynReceipt.m_r.Value
			);
		}
		else
		{ // DR_80, DR_EME, DR_HALF
			g_spooler->Printf
			(
				channel, fmt,
				g_cfg->RECNO_LABEL, szRecno, g_cfg->RECNO_LABEL, szRecno,
				dynReceipt.m_r.Minutes, dynReceipt.m_r.Minutes,
				dynReceipt.m_r.CeilMin, dynReceipt.m_r.CeilMin,
				g_cfg->CURRENCY, dynReceipt.m_r.UnitaryValue, g_cfg->CURRENCY, dynReceipt.m_r.UnitaryValue,
				//
				g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax, g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax,
				g_cfg->TAX_NAME, g_cfg->INTERNET_TAX, g_cfg->CURRENCY, dynReceipt.m_r.Tax, g_cfg->TAX_NAME, g_cfg->INTERNET_TAX, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
				g_cfg->CURRENCY, dynReceipt.m_r.Value, g_cfg->CURRENCY, dynReceipt.m_r.Value
			);
		}
	}
	else
	{
		g_spooler->Printf
		(
			channel, fmt,
			g_cfg->RECNO_LABEL, szRecno,
			dynReceipt.m_r.Minutes,
			dynReceipt.m_r.CeilMin,
			g_cfg->CURRENCY, dynReceipt.m_r.UnitaryValue,
			//
			g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax,
			g_cfg->TAX_NAME, g_cfg->INTERNET_TAX, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
			g_cfg->CURRENCY, dynReceipt.m_r.Value
		);
	}
}

void CONTROLLER::PrintMagneticCardData(DynamicReceipt& dynReceipt)
{
	char *fmt;
	prnFormatter->srGetMagneticCardFmt(&fmt);
	if (g_cfg->FORM == CFG::LINEAL_80)
	{
		PrintMagneticCardLINEAL_80(dynReceipt, fmt);
	}
	else
	{
		BYTE channel = 0;
		BOOL mirrored;
		mirrored =
		(
			g_cfg->FORM == CFG::DR_EME    ||
			g_cfg->FORM == CFG::DR_HALF   ||
			g_cfg->FORM == CFG::DR_PRE    ||
			g_cfg->FORM == CFG::DR_80     ||
			g_cfg->FORM == CFG::DR_18
		);

		STR16 szRecno;
		g_Number2Str(szRecno, dynReceipt.m_r.Number, g_cfg->RECNO_DIGITS, g_cfg->RECNO_LEADING_ZEROS);

		int amount = 0;
		for (int i=0; i < MAX_MAGNETIC_CARDS; i++)
			amount += dynReceipt.m_r.Cards[i];
        // print
        if (mirrored)
        {
			if (g_cfg->FORM == CFG::DR_18)
            {
				g_spooler->Printf
				(
					channel, fmt,
					g_cfg->RECNO_LABEL, szRecno, g_cfg->RECNO_LABEL, szRecno,
					dynReceipt.m_r.Cards[0], g_cfg->MCARD[0],
					dynReceipt.m_r.Cards[0], g_cfg->MCARD[0],
					dynReceipt.m_r.Cards[1], g_cfg->MCARD[1],
					dynReceipt.m_r.Cards[1], g_cfg->MCARD[1],
					dynReceipt.m_r.Cards[2], g_cfg->MCARD[2],
					dynReceipt.m_r.Cards[2], g_cfg->MCARD[2],
					dynReceipt.m_r.Cards[3], g_cfg->MCARD[3],
					dynReceipt.m_r.Cards[3], g_cfg->MCARD[3],
					amount, amount,
					//
					g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax, g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax,
					g_cfg->TAX_NAME, g_cfg->TAX_PERCENT, g_cfg->CURRENCY, dynReceipt.m_r.Tax, g_cfg->TAX_NAME, g_cfg->TAX_PERCENT, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
					g_cfg->CURRENCY, dynReceipt.m_r.Value, g_cfg->CURRENCY, dynReceipt.m_r.Value
				);
			}
			else if (g_cfg->FORM == CFG::DR_PRE)
			{
				g_spooler->Printf
				(
					channel, fmt,
					g_cfg->RECNO_LABEL, szRecno, g_cfg->RECNO_LABEL, szRecno,
					dynReceipt.m_r.Cards[0], g_cfg->MCARD[0],
					dynReceipt.m_r.Cards[1], g_cfg->MCARD[1],
					dynReceipt.m_r.Cards[2], g_cfg->MCARD[2],
					dynReceipt.m_r.Cards[3], g_cfg->MCARD[3],
					dynReceipt.m_r.Cards[0], g_cfg->MCARD[0],
					dynReceipt.m_r.Cards[1], g_cfg->MCARD[1],
					dynReceipt.m_r.Cards[2], g_cfg->MCARD[2],
					dynReceipt.m_r.Cards[3], g_cfg->MCARD[3],
					amount, amount,
					//
					g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax, g_cfg->TAX_NAME, g_cfg->TAX_PERCENT, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
					g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax, g_cfg->TAX_NAME, g_cfg->TAX_PERCENT, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
					g_cfg->CURRENCY, dynReceipt.m_r.Value, g_cfg->CURRENCY, dynReceipt.m_r.Value
				);
			}
            else
            { // DR_80, DR_EME, DR_HALF
				g_spooler->Printf
				(
					channel, fmt,
					g_cfg->RECNO_LABEL, szRecno, g_cfg->RECNO_LABEL, szRecno,
					dynReceipt.m_r.Cards[0], g_cfg->MCARD[0],
					dynReceipt.m_r.Cards[1], g_cfg->MCARD[1],
					dynReceipt.m_r.Cards[0], g_cfg->MCARD[0],
					dynReceipt.m_r.Cards[1], g_cfg->MCARD[1],
					dynReceipt.m_r.Cards[2], g_cfg->MCARD[2],
					dynReceipt.m_r.Cards[3], g_cfg->MCARD[3],
					dynReceipt.m_r.Cards[2], g_cfg->MCARD[2],
					dynReceipt.m_r.Cards[3], g_cfg->MCARD[3],
					amount, amount,
					//
					g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax, g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax,
					g_cfg->TAX_NAME, g_cfg->TAX_PERCENT, g_cfg->CURRENCY, dynReceipt.m_r.Tax, g_cfg->TAX_NAME, g_cfg->TAX_PERCENT, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
					g_cfg->CURRENCY, dynReceipt.m_r.Value, g_cfg->CURRENCY, dynReceipt.m_r.Value
				);
			}
        }
        else
        {
			g_spooler->Printf
			(
				channel, fmt,
				g_cfg->RECNO_LABEL, szRecno,
				dynReceipt.m_r.Cards[0], g_cfg->MCARD[0],
				dynReceipt.m_r.Cards[1], g_cfg->MCARD[1],
				dynReceipt.m_r.Cards[2], g_cfg->MCARD[2],
				dynReceipt.m_r.Cards[3], g_cfg->MCARD[3],
				amount,
				//
				g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax,
				g_cfg->TAX_NAME, g_cfg->TAX_PERCENT, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
				g_cfg->CURRENCY, dynReceipt.m_r.Value
			);
        }
    }
}

void CONTROLLER::PrintOtherData(DynamicReceipt& dynReceipt)
{
    char *fmt;
    prnFormatter->srGetOtherFmt(&fmt);
	if (g_cfg->FORM == CFG::LINEAL_80)
    {
		PrintOtherLINEAL_80(dynReceipt, fmt);
    }
    else
    {
        BYTE channel = 0;
        BOOL mirrored;
		mirrored =
		(
			g_cfg->FORM == CFG::DR_EME    ||
			g_cfg->FORM == CFG::DR_HALF   ||
			g_cfg->FORM == CFG::DR_PRE    ||
			g_cfg->FORM == CFG::DR_80     ||
			g_cfg->FORM == CFG::DR_18
		);

		STR16 szRecno;
		g_Number2Str(szRecno, dynReceipt.m_r.Number, g_cfg->RECNO_DIGITS, g_cfg->RECNO_LEADING_ZEROS);

		if (g_cfg->FORM == CFG::SR_40 || g_cfg->FORM == CFG::DR_40)
		{
			g_spooler->Print(channel, "\x1B\x21\x00\xFF", TRUE);
        }
        // print
        if (mirrored)
        {
			if (g_cfg->FORM == CFG::DR_18)
            {
				g_spooler->Printf
				(
					channel, fmt,
					g_cfg->RECNO_LABEL, szRecno, g_cfg->RECNO_LABEL, szRecno,
					dynReceipt.m_r.Motif, dynReceipt.m_r.Motif,
					dynReceipt.m_r.Amount, dynReceipt.m_r.Amount,
					dynReceipt.m_r.UnitaryValue, dynReceipt.m_r.UnitaryValue,
					//
					g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax, g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax,
					g_cfg->TAX_NAME, g_cfg->TAX_PERCENT, g_cfg->CURRENCY, dynReceipt.m_r.Tax, g_cfg->TAX_NAME, g_cfg->TAX_PERCENT, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
					g_cfg->CURRENCY, dynReceipt.m_r.Value, g_cfg->CURRENCY, dynReceipt.m_r.Value
				);
			}
			else if (g_cfg->FORM == CFG::DR_PRE)
            {
				g_spooler->Printf
				(
					channel, fmt,
					g_cfg->RECNO_LABEL, szRecno, g_cfg->RECNO_LABEL, szRecno,
					dynReceipt.m_r.Motif, dynReceipt.m_r.Motif,
					dynReceipt.m_r.Amount, dynReceipt.m_r.UnitaryValue,
					dynReceipt.m_r.Amount, dynReceipt.m_r.UnitaryValue,
					//
					g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax, g_cfg->TAX_NAME, g_cfg->TAX_PERCENT, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
					g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax, g_cfg->TAX_NAME, g_cfg->TAX_PERCENT, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
					g_cfg->CURRENCY, dynReceipt.m_r.Value, g_cfg->CURRENCY, dynReceipt.m_r.Value
				);
            }
            else
            { // DR_80, DR_EME, DR_HALF
				g_spooler->Printf
				(
					channel, fmt,
					g_cfg->RECNO_LABEL, szRecno, g_cfg->RECNO_LABEL, szRecno,
					dynReceipt.m_r.Motif, dynReceipt.m_r.Motif,
					dynReceipt.m_r.Amount, dynReceipt.m_r.Amount,
					dynReceipt.m_r.UnitaryValue, dynReceipt.m_r.UnitaryValue,
					//
					g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax, g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax,
					g_cfg->TAX_NAME, g_cfg->TAX_PERCENT, g_cfg->CURRENCY, dynReceipt.m_r.Tax, g_cfg->TAX_NAME, g_cfg->TAX_PERCENT, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
					g_cfg->CURRENCY, dynReceipt.m_r.Value, g_cfg->CURRENCY, dynReceipt.m_r.Value
				);
			}
		}
		else
		{
			g_spooler->Printf
			(
				channel, fmt,
				g_cfg->RECNO_LABEL, szRecno,
				dynReceipt.m_r.Motif,
				dynReceipt.m_r.Amount,
				dynReceipt.m_r.UnitaryValue,
				//
				g_cfg->CURRENCY, dynReceipt.m_r.Value - dynReceipt.m_r.Tax,
				g_cfg->TAX_NAME, g_cfg->TAX_PERCENT, g_cfg->CURRENCY, dynReceipt.m_r.Tax,
				g_cfg->CURRENCY, dynReceipt.m_r.Value
			);
		}
    }
}

void CONTROLLER::PrintSTelLINEAL_80(DynamicReceipt& dynReceipt, const char *fmt)
{
	BYTE channel = (!g_cfg->DOUBLE_PRN)?0:dynReceipt.m_r.BoothNumber%DS_MAXDOUBLEPRNENTRIES;
	// date & time
    char dateStr[0x10], timeStr[0x10];
	UI_DATE date(dynReceipt.m_r.Date);
	date.Export(dateStr, DTF_ZERO_FILL|DTF_SLASH|DTF_EUROPEAN_FORMAT);
	UI_TIME time(dynReceipt.m_r.Time);
	time.Export(timeStr, TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL|TMF_SECONDS);
	//
	int hour, minutes, seconds;
	g_Milisec2Time(dynReceipt.m_r.ElapsedTime, hour, minutes, seconds);
	time.Import(hour, minutes, seconds);
	char elapsedStr[0x10];
	time.Export(elapsedStr, TMF_TWENTY_FOUR_HOUR|TMF_SECONDS);

	STR16 szRecno;
	g_Number2Str(szRecno, dynReceipt.m_r.Number, g_cfg->RECNO_DIGITS, g_cfg->RECNO_LEADING_ZEROS);

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

void CONTROLLER::PrintFaxLINEAL_80(DynamicReceipt& dynReceipt, const char *fmt)
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

	g_spooler->Printf
	(
		channel, fmt,
		szRecno,
		dateStr,
		timeStr,
		dynReceipt.m_r.Phone,
		dynReceipt.m_r.City,
		dynReceipt.m_r.Amount,
		dynReceipt.m_r.UnitaryValue,
		g_cfg->CURRENCY,
		dynReceipt.m_r.Value
	);
}

void CONTROLLER::PrintTelexLINEAL_80(DynamicReceipt& dynReceipt, const char *fmt)
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

	g_spooler->Printf
	(
		channel, fmt,
		szRecno,
		dateStr,
		timeStr ,
		dynReceipt.m_r.Minutes,
		dynReceipt.m_r.CeilMin,
		dynReceipt.m_r.UnitaryValue,
		g_cfg->CURRENCY,
		dynReceipt.m_r.Value
	);
}

void CONTROLLER::PrintMagneticCardLINEAL_80(DynamicReceipt& dynReceipt, const char *fmt)
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

	g_spooler->Printf
	(
		channel, fmt,
		szRecno,
		dateStr,
		timeStr,
		dynReceipt.m_r.Cards[0], g_cfg->MCARD[0],
		dynReceipt.m_r.Cards[1], g_cfg->MCARD[1],
		dynReceipt.m_r.Cards[2], g_cfg->MCARD[2],
		dynReceipt.m_r.Cards[3], g_cfg->MCARD[3],
		g_cfg->CURRENCY,
		dynReceipt.m_r.Value
	);
}

void CONTROLLER::PrintOtherLINEAL_80(DynamicReceipt& dynReceipt, const char *fmt)
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

	g_spooler->Printf
	(
		channel, fmt,
		szRecno,
		dateStr,
		timeStr,
		dynReceipt.m_r.Motif,
		dynReceipt.m_r.Amount,
		dynReceipt.m_r.UnitaryValue,
		g_cfg->CURRENCY,
		dynReceipt.m_r.Value
	);
}
