//
// [ PLAIN_PR.CPP ]
//

#include "stdst.h"

#include <db_eng.h>
#include <spooler.h>
#include <events.h>
#include <plain_pr.h>

extern CFG        *g_cfg;
extern DB_ENGINE  *g_dbEngine;
extern SPOOLER    *g_spooler;

// --------------------------------------------------------------------------
//     PLAIN_PRINTOUT
// --------------------------------------------------------------------------

PLAIN_PRINTOUT::PLAIN_PRINTOUT
(
	DWORD from,
	DWORD numOfReceipts,
	SHORT boothNum,
	BOOL fromExt,
	DWORD virtualFrom
)
	:
	UIW_WINDOW("W_LEVEL", defaultStorage)
{
    From = from;
    NumOfReceipts = numOfReceipts;
    FromExt = fromExt;
    BoothNum = boothNum;
    VirtualFrom = virtualFrom;
	((UIW_PROMPT *)Get("W_MSG"))->DataSet("Imprimiendo ...");
    windowManager->Center(this);
}

EVENT_TYPE PLAIN_PRINTOUT::Event(const UI_EVENT &event)
{
    BYTE channel = 0;
    static DWORD higher;
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case S_CREATE:
        UIW_WINDOW::Event(event);
		//
		g_dbEngine->Flush();
		Count = 0;
		if (FromExt)
			higher = g_dbEngine->ExtGetHigherNumber();
		else
			higher = g_dbEngine->GetHigherNumber();
		PrintConfig();
		PrintHeader();
		eventManager->Put(UI_EVENT(UE_VIEW, 0));
		break;
	case UE_CANCEL:
		woAdvancedFlags &= ~WOAF_LOCKED; // disconnect lock !!!
		eventManager->Put(UI_EVENT(S_CLOSE,0));
		break;
	case UE_VIEW:
		{
			// By number
			if (Count < NumOfReceipts && From <= higher)
			{
				Receipt receipt;
				BOOL found = FALSE;
				if (FromExt)
					found = g_dbEngine->ExtGet(receipt, From, BoothNum);
				else
					found = g_dbEngine->Get(receipt, From, BoothNum);
				if (found)
				{
					if (g_spooler->HasSpace(channel))
					{
						if (!receipt.Stat.Printed)
						{
							DynamicReceipt dynReceipt;
							dynReceipt.m_r = receipt;
							dynReceipt.m_r.Stat.Printed = TRUE; // printed !!!
							// begin 2.21.8
							if (!g_dbEngine->Update(dynReceipt))
							{
								if (TraceInfo::s_bTest)
								{
									UI_WINDOW_OBJECT::errorSystem->ReportError
									(
										UI_WINDOW_OBJECT::windowManager, WOS_NO_STATUS,
										"Recibo no actualizado (CTRL_EV::561)!\r\n"
									);
								}
							}
							// end 2.21.8
						}
						PrintData(receipt);
						// go to the next
						UIW_GROUP *wGroup = (UIW_GROUP *) this->Get("W_REF");
						wGroup->Destroy();
						Count++;
						DWORD percent = (Count*100)/NumOfReceipts;
						char msg[0x0A];
						ltoa(percent, msg, 10);
						strcat(msg, " %");
						UIW_BUTTON *wButton = new UIW_BUTTON(0, 1, ((WORD)percent*5)/10, msg);
						*wGroup + wButton;
						wButton->Event(UI_EVENT(S_REDISPLAY, 0));
						From++;
					}
				}
				eventManager->Put(UI_EVENT(UE_VIEW,0), Q_END); // feedback, no contention !!!
			}
			else
			{
				PrintFF();
				woAdvancedFlags &= ~WOAF_LOCKED; // disconnect lock !!!
				eventManager->Put(UI_EVENT(S_CLOSE,0));
			}

			break;
		}
	default:
		{
			ccode = UIW_WINDOW::Event(event);
			break;
		}
	}
	return ccode;
}

void PLAIN_PRINTOUT::PrintConfig(void)
{
	char *cfgStr;
	switch (g_cfg->FORM)
	{
	case CFG::DR_EME:
	case CFG::DR_HALF:
	case CFG::DR_PRE   :
	case CFG::DR_80    :
	case CFG::LINEAL_80:
		cfgStr =
			"\x1B\x40"         // init
			"\x1B\x21\x04"     // style
			"\x1B\x44\x68\x00" // tab for total
			"\xFF"
		;
		break;
	case CFG::DR_40    :
	case CFG::SR_40    :
		cfgStr =
			"\x1B\x40" // init
//			"\x1B\x21\x00" // style
			"\xFF"
		;
		break;
	case CFG::SR_80    :
	case CFG::DR_18    :
	case CFG::SR_28    :
		return ; // nothing to do
	}

	BYTE channel = 0;
	g_spooler->Print(channel, cfgStr, TRUE);
}

void PLAIN_PRINTOUT::PrintHeader(void)
{
    char *headerFmt;
	switch (g_cfg->FORM)
    {
	case CFG::DR_EME:
    case CFG::DR_HALF:
    case CFG::DR_PRE:
	case CFG::DR_80:
    case CFG::LINEAL_80:
    case CFG::SR_40:
    case CFG::DR_40:
        headerFmt =
            "\n\n"
            "%s\n"                 // ciudad
            "[ Recibos %s %s ]\n"  // hora y fecha del informe
            "%s %.1f%%\n\n"        // Impuesto
            "\n"
		;
		break;
	case CFG::SR_80    :
	case CFG::DR_18    :
	case CFG::SR_28    :
		return ; // nothing to do
	}

	char dateStr[0x0F];
	_GetSysDate(dateStr);
	char timeStr[0x0F];
	_GetSysTime(timeStr);

	BYTE channel = 0;
	g_spooler->Printf
	(
		channel, headerFmt,
		g_cfg->CITY,
		timeStr, dateStr,
		g_cfg->TAX_NAME, g_cfg->TAX_PERCENT
	);
}

void PLAIN_PRINTOUT::PrintData(Receipt& receipt)
{
	switch (g_cfg->FORM)
	{
    case CFG::DR_EME:
    case CFG::DR_HALF:
    case CFG::DR_PRE:
	case CFG::DR_80    :
    case CFG::LINEAL_80:
        PrintFX(receipt);
        break;
    case CFG::DR_40    :
    case CFG::SR_40    :
        PrintTM(receipt);
        break;
    case CFG::SR_80    :
    case CFG::DR_18    :
    case CFG::SR_28    :
        break;
    }
}

void PLAIN_PRINTOUT::PrintFX(Receipt& receipt)
{
    switch (receipt.Tag)
    {
	case Receipt::TEL :
		PrintFXTel(receipt);
        break;
	case Receipt::SPECIAL_TEL:
        PrintFXSTel(receipt);
        break;
	case Receipt::FAX:
        PrintFXFax(receipt);
        break;
	case Receipt::TELEX:
		PrintFXTelex(receipt);
        break;
	case Receipt::CARD:
        PrintFXMCard(receipt);
        break;
	case Receipt::OTHER:
        PrintFXOther(receipt);
		break;
	}
}

void PLAIN_PRINTOUT::PrintFXTel(Receipt& receipt)
{
	// date & time
	char dateStr[0x0F], timeStr[0x0F];
	UI_DATE date(receipt.Date);
	date.Export(dateStr, DTF_ZERO_FILL|DTF_SLASH|DTF_EUROPEAN_FORMAT);
	UI_TIME time(receipt.Time);
	time.Export(timeStr, TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL|TMF_SECONDS);
	DWORD number;
	if (FromExt)
		number = VirtualFrom+Count;
	else
		number = receipt.Number;
	int hour, minutes, seconds;
	g_Milisec2Time(receipt.ElapsedTime, hour, minutes, seconds);
	time.Import(hour, minutes, seconds);
	char elapsedStr[0x10];
	time.Export(elapsedStr, TMF_TWENTY_FOUR_HOUR|TMF_SECONDS);

	STR16 szRecno;
	g_Number2Str(szRecno, number, g_cfg->RECNO_DIGITS, g_cfg->RECNO_LEADING_ZEROS);

	BYTE channel = 0;
	g_spooler->Printf
	(
		channel,
		"%s "    	  	// numero de recibo
		"(%-2d) "       // cabina
		"%-8s "         // fecha
		"%-8s "         // hora
		" Tel:"
		" %-16s " 	// telefono
		"%-20s "        // ciudad(destino)
		"%-8s "         // duracion
		"(%5.1f) "      // minutos cobrados
		"%8.1f(%3d%%) " // valor minuto, porcentaje cobrado
		"\t%s%10.2f\n"  // total
		,
		szRecno,
		receipt.BoothNumber+1,
		dateStr,
		timeStr,
		receipt.Phone,
		receipt.City,
		elapsedStr,
		receipt.CeilMin,
		receipt.ValuePerMin, receipt.Percent,
		g_cfg->CURRENCY,
		receipt.Value
   );
}

void PLAIN_PRINTOUT::PrintFXSTel(Receipt& receipt)
{
    // date & time
	char dateStr[0x0F], timeStr[0x0F];
	UI_DATE date(receipt.Date);
	date.Export(dateStr, DTF_ZERO_FILL|DTF_SLASH|DTF_EUROPEAN_FORMAT);
    UI_TIME time(receipt.Time);
    time.Export(timeStr, TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL|TMF_SECONDS);
	DWORD number;
    if (FromExt)
        number = VirtualFrom+Count;
    else
        number = receipt.Number;

	int hour, minutes, seconds;
	g_Milisec2Time(receipt.ElapsedTime, hour, minutes, seconds);
	time.Import(hour, minutes, seconds);
	char elapsedStr[0x10];
    time.Export(elapsedStr, TMF_TWENTY_FOUR_HOUR|TMF_SECONDS);

	STR16 szRecno;
	g_Number2Str(szRecno, number, g_cfg->RECNO_DIGITS, g_cfg->RECNO_LEADING_ZEROS);

	BYTE channel = 0;
	g_spooler->Printf
	(
		channel,
		"%s "    		// numero de recibo
		"(%-2d) "       // cabina
		"%-8s "         // fecha
		"%-8s "         // hora
		"Stel:"
		" %-16s "  		// telefono
		"%-20s "        // ciudad(destino)
		"%-8s "         // duracion
		"(%5.1f) "      // minutos cobrados
		"%8.1f(%3d%%) " // valor minuto, porcentaje cobrado
		"\t%s%10.2f\n"  // total
		,
		szRecno,
		receipt.BoothNumber+1,
		dateStr,
		timeStr,
		receipt.Phone,
		receipt.City,
		elapsedStr,
		receipt.CeilMin,
		receipt.ValuePerMin, receipt.Percent,
		g_cfg->CURRENCY,
		receipt.Value
   );
}

void PLAIN_PRINTOUT::PrintFXFax(Receipt& receipt)
{
    // date & time
	char dateStr[0x0F], timeStr[0x0F];
	UI_DATE date(receipt.Date);
	date.Export(dateStr, DTF_ZERO_FILL|DTF_SLASH|DTF_EUROPEAN_FORMAT);
    UI_TIME time(receipt.Time);
    time.Export(timeStr, TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL|TMF_SECONDS);
    DWORD number;
    if (FromExt)
        number = VirtualFrom+Count;
    else
        number = receipt.Number;

	int hour, minutes, seconds;
	g_Milisec2Time(receipt.ElapsedTime, hour, minutes, seconds);
	time.Import(hour, minutes, seconds);
	char elapsedStr[0x10];
    time.Export(elapsedStr, TMF_TWENTY_FOUR_HOUR|TMF_SECONDS);

	STR16 szRecno;
	g_Number2Str(szRecno, number, g_cfg->RECNO_DIGITS, g_cfg->RECNO_LEADING_ZEROS);

	BYTE channel = 0;
	g_spooler->Printf
	(
		channel,
		"%s "    		 // numero de recibo
		"     "          // relleno
		"%-8s "          // fecha
		"%-8s "          // hora
		" Fax:"
		" %-16s "   	 // telefono
		"%-20s "         // ciudad(destino)
		"[%3d] "         // paginas
		"%-8.1f "        // valor pagina
		"\t"
		"\t      %s%10.2f\n"   // total
		,
		szRecno,
		dateStr,
		timeStr,
		receipt.Phone,
		receipt.City,
		receipt.Amount,
		receipt.UnitaryValue,
		g_cfg->CURRENCY,
		receipt.Value
   );
}

void PLAIN_PRINTOUT::PrintFXTelex(Receipt& receipt)
{
	// date & time
	char dateStr[0x0F], timeStr[0x0F];
	UI_DATE date(receipt.Date);
	date.Export(dateStr, DTF_ZERO_FILL|DTF_SLASH|DTF_EUROPEAN_FORMAT);
	UI_TIME time(receipt.Time);
	time.Export(timeStr, TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL|TMF_SECONDS);
	DWORD number;
	if (FromExt)
		number = VirtualFrom+Count;
	else
		number = receipt.Number;

	int hour, minutes, seconds;
	g_Milisec2Time(receipt.ElapsedTime, hour, minutes, seconds);
	time.Import(hour, minutes, seconds);
	char elapsedStr[0x10];
	time.Export(elapsedStr, TMF_TWENTY_FOUR_HOUR|TMF_SECONDS);

	STR16 szRecno;
	g_Number2Str(szRecno, number, g_cfg->RECNO_DIGITS, g_cfg->RECNO_LEADING_ZEROS);

	BYTE channel = 0;
	g_spooler->Printf
	(
		channel,
		"%s "    		// numero de recibo
		"     "         // relleno
		"%-8s "         // fecha
		"%-8s "         // hora
		"Inet: "   		//
		"%-8.1f "       // minutos
		"[%-8.1f] "    	// cobrados
		"%-8.1f "       // tarifa
		"\t"
		"\t      %s%10.2f\n"  // total
		,
		szRecno,
		dateStr,
		timeStr ,
		receipt.Minutes,
		receipt.CeilMin,
		receipt.UnitaryValue,
		g_cfg->CURRENCY,
		receipt.Value
	);
}

void PLAIN_PRINTOUT::PrintFXMCard(Receipt& receipt)
{
	// date & time
	char dateStr[0x0F], timeStr[0x0F];
	UI_DATE date(receipt.Date);
	date.Export(dateStr, DTF_ZERO_FILL|DTF_SLASH|DTF_EUROPEAN_FORMAT);
	UI_TIME time(receipt.Time);
	time.Export(timeStr, TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL|TMF_SECONDS);
    DWORD number;
    if (FromExt)
        number = VirtualFrom+Count;
    else
		number = receipt.Number;

	int hour, minutes, seconds;
	g_Milisec2Time(receipt.ElapsedTime, hour, minutes, seconds);
	time.Import(hour, minutes, seconds);
	char elapsedStr[0x10];
    time.Export(elapsedStr, TMF_TWENTY_FOUR_HOUR|TMF_SECONDS);

	STR16 szRecno;
	g_Number2Str(szRecno, number, g_cfg->RECNO_DIGITS, g_cfg->RECNO_LEADING_ZEROS);

	BYTE channel = 0;
	g_spooler->Printf
	(
		channel,
		"%s " 		     // numero de recibo
		"     "          // relleno
		"%-8s "          // fecha
		"%-8s "          // hora
		"Tarj: [%02d] %8.2f, [%02d] %8.2f, [%02d] %8.2f, [%02d] %8.2f " // tarjetas
		"\t"
		"\t      %s%10.2f\n"   // total
		,
		szRecno,
		dateStr,
		timeStr,
		receipt.Cards[0], g_cfg->MCARD[0],
		receipt.Cards[1], g_cfg->MCARD[1],
		receipt.Cards[2], g_cfg->MCARD[2],
		receipt.Cards[3], g_cfg->MCARD[3],
		g_cfg->CURRENCY,
		receipt.Value
   );
}

void PLAIN_PRINTOUT::PrintFXOther(Receipt& receipt)
{
    // date & time
    char dateStr[0x0F], timeStr[0x0F];
    UI_DATE date(receipt.Date);
	date.Export(dateStr, DTF_ZERO_FILL|DTF_SLASH|DTF_EUROPEAN_FORMAT);
	UI_TIME time(receipt.Time);
    time.Export(timeStr, TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL|TMF_SECONDS);
    DWORD number;
    if (FromExt)
        number = VirtualFrom+Count;
    else
        number = receipt.Number;

	int hour, minutes, seconds;
	g_Milisec2Time(receipt.ElapsedTime, hour, minutes, seconds);
	time.Import(hour, minutes, seconds);
	char elapsedStr[0x10];
    time.Export(elapsedStr, TMF_TWENTY_FOUR_HOUR|TMF_SECONDS);

	STR16 szRecno;
	g_Number2Str(szRecno, number, g_cfg->RECNO_DIGITS, g_cfg->RECNO_LEADING_ZEROS);

	BYTE channel = 0;
	g_spooler->Printf
	(
		channel,
		"%s "    	    // numero de recibo
		"     "         // relleno
		"%-8s "         // fecha
		"%-8s "         // hora
		"Otro: "   		//
		"%-20s "        // motivo
		"[%3d] "        // cantidad
		"%-8.1f "       // valor unitario
		"\t"
		"\t      %s%10.2f\n"  // total
		,
		szRecno,
		dateStr,
		timeStr,
		receipt.Motif,
		receipt.Amount,
		receipt.UnitaryValue,
		g_cfg->CURRENCY,
		receipt.Value
   );
}

void PLAIN_PRINTOUT::PrintTM(Receipt& receipt)
{
	switch (receipt.Tag)
	{
	case Receipt::TEL :
		PrintTMTel(receipt);
		break;
	case Receipt::SPECIAL_TEL:
		PrintTMSTel(receipt);
		break;
	case Receipt::FAX:
		PrintTMFax(receipt);
		break;
	case Receipt::TELEX:
		PrintTMTelex(receipt);
		break;
	case Receipt::CARD:
		PrintTMMCard(receipt);
		break;
	case Receipt::OTHER:
        PrintTMOther(receipt);
        break;
    }
}

void PLAIN_PRINTOUT::PrintTMTel(Receipt& receipt)
{
    DWORD number;
    if (FromExt)
		number = VirtualFrom+Count;
    else
        number = receipt.Number;
    char timeStr[0x0F];
    UI_TIME time(receipt.Time);
    time.Export(timeStr, TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL|TMF_SECONDS);
    BYTE channel = 0;

	STR16 szRecno;
	g_Number2Str(szRecno, number, g_cfg->RECNO_DIGITS, g_cfg->RECNO_LEADING_ZEROS);

	g_spooler->Printf
	(
		channel,
		"%s "	 	// numero de recibo
		"%-8s "    	// hora
		"(%-2d) " 	// cabina
		"Tele:\n"
		"  %s "     // telefono
		"%.1f  " 	// minutos cobrados
		"%d "      	// tarifa
		"%10.2f\n"  // total
		,
		szRecno,
		timeStr,
		receipt.BoothNumber+1,
		receipt.Phone,
		receipt.CeilMin,
		receipt.Tariff,
		receipt.Value
   );
}

void PLAIN_PRINTOUT::PrintTMSTel(Receipt& receipt)
{
    DWORD number;
    if (FromExt)
        number = VirtualFrom+Count;
	else
        number = receipt.Number;
    char timeStr[0x0F];
    UI_TIME time(receipt.Time);
    time.Export(timeStr, TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL|TMF_SECONDS);
    BYTE channel = 0;

	STR16 szRecno;
	g_Number2Str(szRecno, number, g_cfg->RECNO_DIGITS, g_cfg->RECNO_LEADING_ZEROS);

	g_spooler->Printf
	(
		channel,
		"%s "		// numero de recibo
		"%-8s "    	// hora
		"(%-2d) Stel:\n"  	// cabina
		"  %s "    	// telefono
		"%.1f "  	// minutos cobrados
		"%d "      	// tarifa
		"%10.2f\n"  // total
		,
		szRecno,
		timeStr,
		receipt.BoothNumber+1,
		receipt.Phone,
		receipt.CeilMin,
		receipt.Tariff,
		receipt.Value
   );
}

void PLAIN_PRINTOUT::PrintTMFax(Receipt& receipt)
{
    DWORD number;
    if (FromExt)
        number = VirtualFrom+Count;
	else
        number = receipt.Number;
    char timeStr[0x0F];
	UI_TIME time(receipt.Time);
    time.Export(timeStr, TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL|TMF_SECONDS);
    BYTE channel = 0;

	STR16 szRecno;
	g_Number2Str(szRecno, number, g_cfg->RECNO_DIGITS, g_cfg->RECNO_LEADING_ZEROS);

	g_spooler->Printf
	(
		channel,
		"%s "	 	// numero de recibo
		"%-8s "    	// hora
		"Fax: %s\n" // telefono
		"  [%d] "   // paginas
		"%.1f "     // tarifa
		"%10.2f\n" 	// total
		,
		szRecno,
		timeStr,
		receipt.Phone,
		receipt.Amount,
		receipt.UnitaryValue,
		receipt.Value
   );
}

void PLAIN_PRINTOUT::PrintTMTelex(Receipt& receipt)
{
	DWORD number;
	if (FromExt)
		number = VirtualFrom+Count;
	else
		number = receipt.Number;
	char timeStr[0x0F];
	UI_TIME time(receipt.Time);
	time.Export(timeStr, TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL|TMF_SECONDS);
	BYTE channel = 0;

	STR16 szRecno;
	g_Number2Str(szRecno, number, g_cfg->RECNO_DIGITS, g_cfg->RECNO_LEADING_ZEROS);

	g_spooler->Printf
	(
		channel,
		"%s "	  		// numero de recibo
		"%-8s "     	// hora
		"Inet: "
		"%-8.1f\n" 		// minutos
		"  [%-8.1f] " 	// cobrados
		"%-8.1f "  		// tarifa
		"%10.2f\n"  	// total
		,
		szRecno,
		timeStr,
		receipt.Minutes,
		receipt.CeilMin,
		receipt.UnitaryValue,
		receipt.Value
   );
}

void PLAIN_PRINTOUT::PrintTMMCard(Receipt& receipt)
{
	DWORD number;
	if (FromExt)
		number = VirtualFrom+Count;
	else
		number = receipt.Number;
	char timeStr[0x0F];
	UI_TIME time(receipt.Time);
	time.Export(timeStr, TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL|TMF_SECONDS);
	BYTE channel = 0;

	STR16 szRecno;
	g_Number2Str(szRecno, number, g_cfg->RECNO_DIGITS, g_cfg->RECNO_LEADING_ZEROS);

	g_spooler->Printf
	(
		channel,
		"%s "		 // numero de recibo
		"%-8s "    // hora
		"Tarj\n"
		"  [%d]%.0f [%d]%.0f [%d]%.0f\n" // cantidad de tarjetas
		"  [%d]%.0f "
		"%10.2f\n"              // total
		,
		szRecno,
		timeStr,
		receipt.Cards[0], g_cfg->MCARD[0], receipt.Cards[1], g_cfg->MCARD[1],
		receipt.Cards[2], g_cfg->MCARD[2], receipt.Cards[3], g_cfg->MCARD[3],
		receipt.Value
	);
}

void PLAIN_PRINTOUT::PrintTMOther(Receipt& receipt)
{
	DWORD number;
	if (FromExt)
		number = VirtualFrom+Count;
	else
		number = receipt.Number;
	char timeStr[0x0F];
	UI_TIME time(receipt.Time);
	time.Export(timeStr, TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL|TMF_SECONDS);
	BYTE channel = 0;

	STR16 szRecno;
	g_Number2Str(szRecno, number, g_cfg->RECNO_DIGITS, g_cfg->RECNO_LEADING_ZEROS);

	g_spooler->Printf
	(
		channel,
		"%s "     	 	// numero de recibo
		"%-8s "   		// hora
		"Otros: %s\n"   // motivo
		"  [%d] "       // cantidad
		"%.1f "         // tarifa
		"%10.2f\n"      // total
		,
		szRecno,
		timeStr,
		receipt.Motif,
		receipt.Amount,
		receipt.UnitaryValue,
		receipt.Value
	);
}

void PLAIN_PRINTOUT::PrintFF(void)
{
	char *formFeed;
	switch (g_cfg->FORM)
	{
	case CFG::LINEAL_80:
	case CFG::DR_EME:
	case CFG::DR_HALF:
	case CFG::DR_PRE:
	case CFG::DR_80:
		formFeed = "\x0C";
		break;
	case CFG::DR_40:
	case CFG::SR_40:
		formFeed = "\n\n\n\n\n\n\n\n\n\x1B\x69";
		break;
	case CFG::SR_80:
	case CFG::DR_18:
	case CFG::SR_28:
		return; // nothing to do
	}

	BYTE channel = 0;
	g_spooler->Print(channel, formFeed);
}