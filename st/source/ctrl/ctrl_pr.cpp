//
// [ CTRL_PR.CPP ]
//

#include "stdst.h"

#include <control.h>
#include <db_eng.h>
#include <ph_eng.h>
#include <spooler.h>
#include <info.h>

extern CFG        *g_cfg;
extern PH_ENGINE  *g_phEngine;
extern SPOOLER    *g_spooler;
extern DB_ENGINE  *g_dbEngine;
extern APP_INFO    g_appInfo;

BOOL CONTROLLER::PrintReceipt(DynamicReceipt& dynReceipt)
{
    BOOL printed = FALSE;
	switch (dynReceipt.m_r.Tag)
    {
	case Receipt::TEL:
        {
			printed = PrintNReceipt(dynReceipt);
            break;
        }
	case Receipt::SPECIAL_TEL:
	case Receipt::TELEX:
	case Receipt::FAX:
	case Receipt::CARD:
	case Receipt::OTHER:
        {
			printed = PrintSReceipt(dynReceipt);
            break;
        }
    }
    return printed;
}

BOOL CONTROLLER::PrintNReceipt(DynamicReceipt& dynReceipt)
{
	BYTE channel = (!g_cfg->DOUBLE_PRN)?0:dynReceipt.m_r.BoothNumber%DS_MAXDOUBLEPRNENTRIES;
	if (!g_spooler->HasSpace(channel))
        return FALSE;
    //
    PrintConfig(channel);
	if (dynReceipt.Attr_.HeaderOn)
		PrintHeader(dynReceipt);
    else
		PrintShortHeader(dynReceipt);
	PrintNData(dynReceipt);
	if (dynReceipt.Attr_.SummaryOn)
		PrintSummary(dynReceipt);
    else
		PrintRem(dynReceipt);
	if (dynReceipt.Attr_.FooterOn)
    {
        PrintFooter(channel);
        PrintFF(channel);
    }
    else
    {
        PrintLF(channel);
    }
    return TRUE;
}

BOOL CONTROLLER::PrintSReceipt(DynamicReceipt& dynReceipt)
{
    BYTE channel = 0;
	if (!g_spooler->HasSpace(channel))
        return FALSE;
    PrintConfig(channel);
	PrintHeader(dynReceipt);
	switch (dynReceipt.m_r.Tag)
    {
	case Receipt::SPECIAL_TEL:
		PrintSTelData (dynReceipt);
        break;
	case Receipt::FAX        :
		PrintFaxData  (dynReceipt);
        break;
	case Receipt::TELEX      :
		PrintTelexData(dynReceipt);
        break;
	case Receipt::CARD       :
		PrintMagneticCardData(dynReceipt);
        break;
	case Receipt::OTHER      :
		PrintOtherData(dynReceipt);
        break;
    }
    PrintFooter(channel);
    PrintFF(channel);
    return TRUE;
}

void CONTROLLER::printStatistics(WORD type, BOOL fromTurn)
{
    BYTE channel = 0;
	if (!g_spooler->HasSpace(channel))
        return ;
    PrintConfig(channel, TRUE);
	DynamicReceipt dynReceipt;
    UI_TIME time;
	time.Export(&dynReceipt.m_r.Time);
    UI_DATE date;
	date.Export(&dynReceipt.m_r.Date);
	dynReceipt.m_r.BoothNumber = 0; // to force channel 0
	PrintHeader(dynReceipt, TRUE);
	if (fromTurn && g_cfg->RELAY_NUMBER)
    {
        printLog();
    }
    printStatData(type, fromTurn);
    PrintFooter(channel, TRUE);
    PrintFF(channel, TRUE);
}

void CONTROLLER::printPrePaidReceipt(WORD boothCount, double value)
{
    BYTE channel = 0;
	if (!g_spooler->HasSpace(channel))
        return ;
    PrintConfig(channel);
	DynamicReceipt dynReceipt;
    UI_TIME time;
	time.Export(&dynReceipt.m_r.Time);
    UI_DATE date;
	date.Export(&dynReceipt.m_r.Date);
	dynReceipt.m_r.BoothNumber = boothCount;
	PrintHeader(dynReceipt);
    printPrePaidData(boothCount, value);
    PrintFooter(channel);
    PrintFF(channel);
}

void CONTROLLER::printPrePaidData(WORD boothCount, double value)
{
    BYTE channel = 0;
    BOOL mirrored;
    mirrored = (
				   g_cfg->FORM == CFG::DR_EME    ||
				   g_cfg->FORM == CFG::DR_HALF   ||
				   g_cfg->FORM == CFG::DR_PRE    ||
				   g_cfg->FORM == CFG::DR_80     ||
				   g_cfg->FORM == CFG::DR_18
               );
    char *fmt;
    prnFormatter->getPrePaidFmt(&fmt);
    // print
	if (g_cfg->FORM == CFG::DR_40 ||	g_cfg->FORM == CFG::SR_40)
    {
		g_spooler->Print(channel, "\x1B\x21\x00\xFF", TRUE);
    }
    if (mirrored)
    {
		g_spooler->Printf(channel, fmt,
                         boothCount, boothCount,
						 g_cfg->CURRENCY, value, g_cfg->CURRENCY, value
                        );
    }
    else
    {
		g_spooler->Printf(channel, fmt,
                         boothCount,
						 g_cfg->CURRENCY, value
                        );
    }
}

void CONTROLLER::PrintConfig(BYTE channel, BOOL fromStatistics)
{
    char *fmt;
	if (g_cfg->FORM == CFG::LINEAL_80 && fromStatistics)
    {
        fmt = // tabs, special case v.220
            "\x1B\x40"                 // init
            "\x1B\x21\x00"             // style
            "\x1B\x44\x02\x30\x00\xFF"
            ;
		g_spooler->Print(channel, fmt, TRUE);
    }
    else
    {
        prnFormatter->getConfigFmt(&fmt);
		g_spooler->Print(channel, fmt, TRUE);
    }
}

void CONTROLLER::PrintHeader(DynamicReceipt& dynReceipt, BOOL fromStatistics)
{
    BYTE channel = 0;

    //
    // Compose legal header
    //
    STR256 szLegalHeader;
	strcpy(szLegalHeader, g_cfg->HEADER_LINE);
	if (g_cfg->HEADER_PRINT_TAXNAME)
    {
    	strcat(szLegalHeader, " ");
		strcat(szLegalHeader, g_cfg->TAX_NAME);
    }
	else if (g_cfg->HEADER_PRINT_RECNO && !fromStatistics)
    {
		strcat(szLegalHeader, " ");

		STR16 szRecno;
		g_Number2Str(szRecno, dynReceipt.m_r.Number, g_cfg->RECNO_DIGITS, g_cfg->RECNO_LEADING_ZEROS);

		strcat(szLegalHeader, szRecno);
    }

    // prepare time and date
    char dateStr[0x10], timeStr[0x10];
    UI_TIME time;
	time.Import(dynReceipt.m_r.Time);
    time.Export(timeStr, TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL|TMF_SECONDS);

    UI_DATE date;
	date.Import(dynReceipt.m_r.Date);
    date.Export(dateStr, DTF_SLASH|DTF_EUROPEAN_FORMAT);

    // prepare header
    char *fmt;
	if (g_cfg->FORM == CFG::LINEAL_80 && fromStatistics)
    {
        fmt = // special case . v.220
            "\x1B\x21\x21""\t%s\n" // empresa
            "\x1B\x21\x05""\t%s\n" // id
            "\t%s <%s> %s %s\t%s <%s> %s %s\n" // ciudad, serial, fecha, hora
            "%s\n"
            /*
            "\tRecibo de pago de servicios públicos\n"
            "\tSomos agente retenedor del %s\n" */;

		g_spooler->Printf(channel, fmt,
			g_cfg->COMPANY, g_cfg->ID,
			g_cfg->CITY, g_appInfo.ShortSerial, dateStr, timeStr,
			g_cfg->CITY, g_appInfo.ShortSerial, dateStr, timeStr,
            szLegalHeader);
    }
    else
    {
        prnFormatter->getHeaderFmt(&fmt);
        BOOL mirrored;
        mirrored = (
					   g_cfg->FORM == CFG::DR_EME    ||
					   g_cfg->FORM == CFG::DR_HALF   ||
					   g_cfg->FORM == CFG::DR_PRE    ||
					   g_cfg->FORM == CFG::DR_80     ||
					   g_cfg->FORM == CFG::DR_18
                   );
        // print
		if (dynReceipt.m_r.Tag == Receipt::TEL)
			channel = (!g_cfg->DOUBLE_PRN)?0:dynReceipt.m_r.BoothNumber%DS_MAXDOUBLEPRNENTRIES;
        if (mirrored)
        {
			if (g_cfg->FORM == CFG::DR_18)
            {
				g_spooler->Printf(channel, fmt,
					g_cfg->COMPANY,
					g_cfg->ID,
					g_cfg->CITY, g_cfg->CITY,
					g_appInfo.ShortSerial, g_appInfo.ShortSerial,
                    dateStr, timeStr,
                    dateStr, timeStr,
                    szLegalHeader);
            }
			else if (g_cfg->FORM == CFG::DR_EME)
            {
				g_spooler->Printf(channel, fmt,
					g_cfg->COMPANY, g_cfg->ID,
					g_cfg->CITY, g_appInfo.ShortSerial, g_cfg->CITY, g_appInfo.ShortSerial,
                    dateStr, timeStr, dateStr, timeStr);
            }
			else if (g_cfg->FORM == CFG::DR_HALF)
            {
				g_spooler->Printf(channel, fmt,
					g_cfg->COMPANY, g_cfg->ID,
					g_cfg->CITY, g_appInfo.ShortSerial, dateStr, timeStr,
					g_cfg->CITY, g_appInfo.ShortSerial, dateStr, timeStr);
            }
			else if (g_cfg->FORM == CFG::DR_80)
            {
				g_spooler->Printf(channel, fmt,
					g_cfg->COMPANY, g_cfg->ID,
					g_cfg->CITY, g_appInfo.ShortSerial, dateStr, timeStr,
					g_cfg->CITY, g_appInfo.ShortSerial, dateStr, timeStr,
                    szLegalHeader);
            }
			else if (g_cfg->FORM == CFG::DR_PRE)
            {
				g_spooler->Printf(channel, fmt,
					g_cfg->COMPANY,
					g_cfg->CITY, g_appInfo.ShortSerial, dateStr, timeStr,
					g_cfg->CITY, g_appInfo.ShortSerial, dateStr, timeStr);
            }
        }
        else
        {
			if (g_cfg->FORM == CFG::SR_40 || g_cfg->FORM == CFG::DR_40)
            {
				g_spooler->Printf(channel, fmt,
					g_cfg->COMPANY, g_cfg->ID,
					g_cfg->CITY, dateStr, timeStr,
                    szLegalHeader,
					g_appInfo.ShortSerial);
            }
			else if (g_cfg->FORM == CFG::SR_80)
            {
				g_spooler->Printf(channel, fmt,
					g_cfg->CITY, g_appInfo.ShortSerial, dateStr, timeStr);
            }
            else
            {
				g_spooler->Printf(channel, fmt,
					g_cfg->COMPANY, g_cfg->ID,
					g_cfg->CITY, g_appInfo.ShortSerial, dateStr, timeStr,
                    szLegalHeader);
            }
        }
    }
}

void CONTROLLER::PrintShortHeader(DynamicReceipt& dynReceipt)
{
	if (g_cfg->FORM != CFG::LINEAL_80)
    {
        // prepare time and date
        char dateStr[0x10], timeStr[0x10];
        UI_TIME time;
		time.Import(dynReceipt.m_r.Time);
        time.Export(timeStr, TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL|TMF_SECONDS);
        UI_DATE date;
		date.Import(dynReceipt.m_r.Date);
        date.Export(dateStr, DTF_SLASH|DTF_EUROPEAN_FORMAT);
        // prepare header format
        char *fmt;
        prnFormatter->getShortHeaderFmt(&fmt);
        BOOL mirrored;
        mirrored = (
					   g_cfg->FORM == CFG::DR_EME    ||
					   g_cfg->FORM == CFG::DR_HALF   ||
					   g_cfg->FORM == CFG::DR_PRE    ||
					   g_cfg->FORM == CFG::DR_80     ||
					   g_cfg->FORM == CFG::DR_18
                   );
        // print
		BYTE channel = (!g_cfg->DOUBLE_PRN)?0:dynReceipt.m_r.BoothNumber%DS_MAXDOUBLEPRNENTRIES;
        if (mirrored)
        {
			if (g_cfg->FORM == CFG::DR_18)
            {
				g_spooler->Printf(channel, fmt,
								 g_cfg->CITY, g_cfg->CITY,
                                 dateStr, timeStr,
                                 dateStr, timeStr
                                );
            }
			else if (g_cfg->FORM == CFG::DR_EME)
            {
				g_spooler->Printf(channel, fmt,
								 g_cfg->CITY,	g_cfg->CITY,
                                 dateStr, timeStr, dateStr, timeStr
                                );
            }
            else
            {
				g_spooler->Printf(channel, fmt,
								 g_cfg->CITY, dateStr, timeStr,
								 g_cfg->CITY, dateStr, timeStr
                                );
            }
        }
        else
        {
			g_spooler->Printf(channel, fmt,
							 g_cfg->CITY, dateStr, timeStr
                            );
        }
    }
}

void CONTROLLER::PrintRem(DynamicReceipt& dynReceipt)
{
	if (dynReceipt.MoneyBack_ > 0)
    {
		if (g_cfg->FORM != CFG::LINEAL_80)
        {
			BYTE channel = (!g_cfg->DOUBLE_PRN)?0:dynReceipt.m_r.BoothNumber%DS_MAXDOUBLEPRNENTRIES;
            // print
            char *fmt;
            prnFormatter->getRemFmt(&fmt);
			g_spooler->Printf(channel, fmt, g_cfg->CURRENCY, dynReceipt.MoneyBack_);
        }
    }
}

void CONTROLLER::PrintSummary(DynamicReceipt& dynReceipt)
{
	if (g_cfg->FORM != CFG::LINEAL_80)
    {
		BYTE channel = (!g_cfg->DOUBLE_PRN)?0:dynReceipt.m_r.BoothNumber%DS_MAXDOUBLEPRNENTRIES;
        char *fmt;
        prnFormatter->getSummaryFmt(&fmt);
		g_spooler->Printf(channel, fmt,
						 dynReceipt.NumOfCalls_,
						 g_cfg->CURRENCY,
						 dynReceipt.Total_,
						 g_cfg->CURRENCY, dynReceipt.MoneyBack_
                        );
    }
}

void CONTROLLER::PrintFooter(BYTE channel, BOOL fromStatistics)
{
	if (!(g_cfg->FORM == CFG::LINEAL_80 && !fromStatistics))
    {
		if (strlen(g_cfg->P_FOOTER) != 0)
        {
            char *fmt;
            prnFormatter->getFooterFmt(&fmt);
            // print
			g_spooler->Printf(channel, fmt, g_cfg->P_FOOTER);
        }
    }
}

void CONTROLLER::PrintFF(BYTE channel, BOOL fromStatistics)
{
	if (!(g_cfg->FORM == CFG::LINEAL_80 && !fromStatistics))
    {
        char *fmt;
        // print
        prnFormatter->getFFFmt(&fmt);
		g_spooler->Print(channel, fmt);
    }
}

void CONTROLLER::PrintLF(BYTE channel)
{
    char *fmt;
    // print
    prnFormatter->getLFFmt(&fmt);
	g_spooler->Print(channel, fmt);
}

void CONTROLLER::printLog(void)
{
    BYTE channel = 0;
    char *fmt;
    prnFormatter->getLogTitleFmt(&fmt);
	g_spooler->Print(channel, fmt);
    WORD hour, minutes, seconds;
    STR128 line;
    char timeStr[16];
    char motifStr[64];
    Log log(Log::IN);
    Log::Entry entry;
    while (log.get(entry))
    {
        _UnpackTime(entry.time, hour, minutes, seconds);
        _Time2Str(timeStr, hour, minutes, seconds, TRUE);
        strcpy(line, " ");
        strcat(line, timeStr);
        if (g_cfg->FORM == CFG::DR_18)
        {
            log.motif2Str(motifStr, entry.motif, TRUE);
            strcat(line, "\n");
        }
        else
        {
            log.motif2Str(motifStr, entry.motif);
        }
        strcat(line, " ");
        strcat(line, motifStr);
        strcat(line, "\n");
		g_spooler->Print(channel, line);
    }
    g_spooler->Print(channel, "\n");
}
