//
// [ CT_PR_ST.CPP ]
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

void CONTROLLER::printStatData(WORD type, BOOL fromTurn)
{
    BYTE channel = 0;
    char fromDateStr[0x0A], fromTimeStr[0x0A];
    char toDateStr[0x0A], toTimeStr[0x0A];
    UI_TIME time;
    UI_DATE date;
    WORD timeFlags = TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL;
    WORD dateFlags = DTF_SLASH|DTF_EUROPEAN_FORMAT;
    DS_ENTRY *entry;
    DS_CELLULARENTRY *cellularEntry;
    if (fromTurn)
    {
			entry = (*g_dbEngine)[type];
			cellularEntry = g_dbEngine->GetCellularEntry(type);
		}
		else
		{
			entry = g_dbEngine->GetArcStatistics(type);
			cellularEntry = g_dbEngine->GetArcCellularEntry(type);
		}
		time.Import(entry->From.Time);
		time.Export(fromTimeStr, timeFlags);
		date.Import(entry->From.Date);
		date.Export(fromDateStr, dateFlags);
		time.Import(entry->To.Time)  ;
		time.Export(toTimeStr, timeFlags);
		date.Import(entry->To.Date)  ;
		date.Export(toDateStr, dateFlags);
		//
	char *statisticsStr[] =
	{
		"Comprobante de informe anual",
		"Comprobante de informe Mensual",
		"Comprobante de informe Semanal",
		"Comprobante de informe Diario",
		"De turno"
	};
		// mirroring
		BOOL mirrored;
	mirrored =
	(
		g_cfg->FORM == CFG::LINEAL_80 ||
		g_cfg->FORM == CFG::DR_EME    ||
		g_cfg->FORM == CFG::DR_HALF   ||
		g_cfg->FORM == CFG::DR_PRE    ||
		g_cfg->FORM == CFG::DR_80     ||
		g_cfg->FORM == CFG::DR_18
	);
    // prepare formats
    char *titleFmt;
    if (type == DB_STATISTICS::TURN)
        prnFormatter->stGetTurnTitleFmt(&titleFmt);
    else
        prnFormatter->stGetNotTurnTitleFmt(&titleFmt);
    char *notPaidFmt;
    prnFormatter->stGetNotPaidFmt(&notPaidFmt);
    char *normalFmt;
    prnFormatter->stGetNormalFmt(&normalFmt);
    char *nalSpecialFmt;
    prnFormatter->stGetNalSpecialFmt(&nalSpecialFmt);
    char *interSpecialFmt;
    prnFormatter->stGetInterSpecialFmt(&interSpecialFmt);
    char *otherSpecialFmt;
    prnFormatter->stGetOtherSpecialFmt(&otherSpecialFmt);
#if defined(__EDA__)
    char *edaTotalFmt;
	prnFormatter->stGetEDATotalFmt(&edaTotalFmt);
#endif
	char *specialTotalFmt;
	prnFormatter->stGetSpecialTotalFmt(&specialTotalFmt);
	char *totalFmt;
	prnFormatter->stGetTotalFmt(&totalFmt);
	char *doublePrnFmt; // for double prn
	prnFormatter->stGetDoublePrnFmt(&doublePrnFmt);
	WORD mCards = 0;
	for (WORD i=0; i<MAX_MAGNETIC_CARDS; i++)
		mCards += entry->Cards.Cards[i];
	DS_DOUBLEPRNENTRY *doublePrnEntry0, *doublePrnEntry1;
	doublePrnEntry0 = g_dbEngine->GetDoublePrnEntry(0);
	doublePrnEntry1 = g_dbEngine->GetDoublePrnEntry(1);
	//
	if (g_cfg->FORM == CFG::SR_40 || g_cfg->FORM == CFG::DR_40)
	{
		g_spooler->Print(channel, "\x1B\x21\x00\xFF", TRUE);
	}
	// calc num of receipts based on each entry item
	WORD nReceipts = 0;
	nReceipts =
#if !defined(__EDA__)
		entry->Tel.Nal.Receipts +
#else
		entry->Tel.EDA2EDA.Receipts +
		entry->Tel.EDA2EPM.Receipts +
		entry->Tel.EDA2TEL.Receipts +
#endif
		entry->Tel.Inter.Receipts +
#if !defined(__EDA__)
		entry->SpecialTel.Nal.Receipts +
#else
		entry->SpecialTel.EDA2EDA.Receipts +
		entry->SpecialTel.EDA2EPM.Receipts +
		entry->SpecialTel.EDA2TEL.Receipts +
#endif
#if !defined(__EDA__)
		entry->Fax.Nal.Receipts +
#else
		entry->Fax.EDA2EDA.Receipts +
		entry->Fax.EDA2EPM.Receipts +
		entry->Fax.EDA2TEL.Receipts +
#endif
#if !defined(__EDA__)
		entry->Internet.Nal.Receipts +
#else
		entry->Internet.EDA2EDA.Receipts +
		entry->Internet.EDA2EPM.Receipts +
		entry->Internet.EDA2TEL.Receipts +
#endif
		entry->Cards.Receipts +
		entry->Other.Receipts +

		cellularEntry->Tel.Receipts
	;
	// print
	if (mirrored)
	{
		if (g_cfg->FORM == CFG::DR_18)
		{
			// titles
			if (type == DB_STATISTICS::TURN)
				g_spooler->Printf
				(
					channel, titleFmt,
					statisticsStr[type], statisticsStr[type],
					g_cfg->OP_NAME, g_cfg->OP_NAME,
					fromDateStr, fromTimeStr,
					fromDateStr, fromTimeStr,
					toDateStr, toTimeStr,
					toDateStr, toTimeStr,
					g_cfg->RECNO_LABEL, nReceipts, entry->From.Number, entry->To.Number,
					g_cfg->RECNO_LABEL, nReceipts, entry->From.Number, entry->To.Number
				);
			else
				g_spooler->Printf
				(
					channel, titleFmt,
					statisticsStr[type],
					statisticsStr[type],
					fromDateStr, fromTimeStr,
					fromDateStr, fromTimeStr,
					toDateStr, toTimeStr,
					toDateStr, toTimeStr,
					g_cfg->RECNO_LABEL, nReceipts, entry->From.Number, entry->To.Number,
					g_cfg->RECNO_LABEL, nReceipts, entry->From.Number, entry->To.Number
				);
		}
		else
		{
			// titles
			if (type == DB_STATISTICS::TURN)
				g_spooler->Printf
				(
					channel, titleFmt,
					statisticsStr[type], statisticsStr[type],
					g_cfg->OP_NAME, g_cfg->OP_NAME,
					fromDateStr, fromTimeStr, toDateStr, toTimeStr,
					fromDateStr, fromTimeStr, toDateStr, toTimeStr,
					g_cfg->RECNO_LABEL, nReceipts, entry->From.Number, entry->To.Number,
					g_cfg->RECNO_LABEL, nReceipts, entry->From.Number, entry->To.Number
				);
			else
				g_spooler->Printf
				(
					channel, titleFmt,
					statisticsStr[type], statisticsStr[type],
					fromDateStr, fromTimeStr, toDateStr, toTimeStr, fromDateStr, fromTimeStr, toDateStr, toTimeStr,
					g_cfg->RECNO_LABEL, nReceipts, entry->From.Number, entry->To.Number,
					g_cfg->RECNO_LABEL, nReceipts, entry->From.Number, entry->To.Number
				);
		}
		// not paid
		g_spooler->Printf
		(
			channel, notPaidFmt,
			entry->NotPaid.Receipts, entry->NotPaid.Value,
			entry->NotPaid.Receipts, entry->NotPaid.Value,
			entry->TollFree.Receipts, entry->TollFree.Value,
			entry->TollFree.Receipts, entry->TollFree.Value,
			entry->Total.NotPaid, entry->Total.NotPaid
		);
		// automatic services
		if (g_cfg->FORM == CFG::DR_18)
		{
			g_spooler->Printf
			(
				channel, normalFmt,
#if !defined(__EDA__)
				entry->Tel.Nal.Receipts, entry->Tel.Nal.TalkMin, entry->Tel.Nal.PaidMin, entry->Tel.Nal.Value,
				entry->Tel.Nal.Receipts, entry->Tel.Nal.TalkMin, entry->Tel.Nal.PaidMin, entry->Tel.Nal.Value,
#else
				entry->Tel.EDA2EDA.Receipts, entry->Tel.EDA2EDA.TalkMin, entry->Tel.EDA2EDA.PaidMin, entry->Tel.EDA2EDA.Value,
				entry->Tel.EDA2EDA.Receipts, entry->Tel.EDA2EDA.TalkMin, entry->Tel.EDA2EDA.PaidMin, entry->Tel.EDA2EDA.Value,
				entry->Tel.EDA2EPM.Receipts, entry->Tel.EDA2EPM.TalkMin, entry->Tel.EDA2EPM.PaidMin, entry->Tel.EDA2EPM.Value,
				entry->Tel.EDA2EPM.Receipts, entry->Tel.EDA2EPM.TalkMin, entry->Tel.EDA2EPM.PaidMin, entry->Tel.EDA2EPM.Value,
				entry->Tel.EDA2TEL.Receipts, entry->Tel.EDA2TEL.TalkMin, entry->Tel.EDA2TEL.PaidMin, entry->Tel.EDA2TEL.Value,
				entry->Tel.EDA2TEL.Receipts, entry->Tel.EDA2TEL.TalkMin, entry->Tel.EDA2TEL.PaidMin, entry->Tel.EDA2TEL.Value,
#endif
				//
				cellularEntry->Tel.Receipts, cellularEntry->Tel.TalkMin, cellularEntry->Tel.PaidMin, cellularEntry->Tel.Value,
				cellularEntry->Tel.Receipts, cellularEntry->Tel.TalkMin, cellularEntry->Tel.PaidMin, cellularEntry->Tel.Value,
				//
				entry->Tel.Inter.Receipts, entry->Tel.Inter.TalkMin, entry->Tel.Inter.PaidMin, entry->Tel.Inter.Value,
				entry->Tel.Inter.Receipts, entry->Tel.Inter.TalkMin, entry->Tel.Inter.PaidMin, entry->Tel.Inter.Value,
				entry->Total.Tel-entry->Tax.Tel, entry->Total.Tel-entry->Tax.Tel,
				g_cfg->TAX_NAME, g_cfg->TAX_NAME,
				entry->Tax.Tel, entry->Tax.Tel,
				entry->Total.Tel, entry->Total.Tel
			);
		}
		else
		{
			g_spooler->Printf
			(
				channel, normalFmt,
#if !defined(__EDA__)
				entry->Tel.Nal.Receipts, entry->Tel.Nal.TalkMin, entry->Tel.Nal.PaidMin, entry->Tel.Nal.Value,
				entry->Tel.Nal.Receipts, entry->Tel.Nal.TalkMin, entry->Tel.Nal.PaidMin, entry->Tel.Nal.Value,
#else
				entry->Tel.EDA2EDA.Receipts, entry->Tel.EDA2EDA.TalkMin, entry->Tel.EDA2EDA.PaidMin, entry->Tel.EDA2EDA.Value,
				entry->Tel.EDA2EDA.Receipts, entry->Tel.EDA2EDA.TalkMin, entry->Tel.EDA2EDA.PaidMin, entry->Tel.EDA2EDA.Value,
				entry->Tel.EDA2EPM.Receipts, entry->Tel.EDA2EPM.TalkMin, entry->Tel.EDA2EPM.PaidMin, entry->Tel.EDA2EPM.Value,
				entry->Tel.EDA2EPM.Receipts, entry->Tel.EDA2EPM.TalkMin, entry->Tel.EDA2EPM.PaidMin, entry->Tel.EDA2EPM.Value,
				entry->Tel.EDA2TEL.Receipts, entry->Tel.EDA2TEL.TalkMin, entry->Tel.EDA2TEL.PaidMin, entry->Tel.EDA2TEL.Value,
				entry->Tel.EDA2TEL.Receipts, entry->Tel.EDA2TEL.TalkMin, entry->Tel.EDA2TEL.PaidMin, entry->Tel.EDA2TEL.Value,
#endif
				//
				cellularEntry->Tel.Receipts, cellularEntry->Tel.TalkMin, cellularEntry->Tel.PaidMin, cellularEntry->Tel.Value,
				cellularEntry->Tel.Receipts, cellularEntry->Tel.TalkMin, cellularEntry->Tel.PaidMin, cellularEntry->Tel.Value,
				//
				entry->Tel.Inter.Receipts, entry->Tel.Inter.TalkMin, entry->Tel.Inter.PaidMin, entry->Tel.Inter.Value,
				entry->Tel.Inter.Receipts, entry->Tel.Inter.TalkMin, entry->Tel.Inter.PaidMin, entry->Tel.Inter.Value,
				entry->Total.Tel-entry->Tax.Tel, entry->Total.Tel-entry->Tax.Tel,
				g_cfg->TAX_NAME,  entry->Tax.Tel,
				g_cfg->TAX_NAME,entry->Tax.Tel,
				entry->Total.Tel, entry->Total.Tel
			);
		}
		// Nal special services
		g_spooler->Printf
		(
			channel, nalSpecialFmt,
#if !defined(__EDA__)
			entry->SpecialTel.Nal.Receipts, entry->SpecialTel.Nal.Value,
			entry->SpecialTel.Nal.Receipts, entry->SpecialTel.Nal.Value,
			//
			cellularEntry->SpecialTel.Receipts, cellularEntry->SpecialTel.Value,
			cellularEntry->SpecialTel.Receipts, cellularEntry->SpecialTel.Value,
			//
			entry->Fax.Nal.Receipts, entry->Fax.Nal.Amount, entry->Fax.Nal.Value,
			entry->Fax.Nal.Receipts, entry->Fax.Nal.Amount, entry->Fax.Nal.Value,
#else
			entry->SpecialTel.EDA2EDA.Receipts, entry->SpecialTel.EDA2EDA.Value,
			entry->SpecialTel.EDA2EDA.Receipts, entry->SpecialTel.EDA2EDA.Value,
			entry->SpecialTel.EDA2EPM.Receipts, entry->SpecialTel.EDA2EPM.Value,
			entry->SpecialTel.EDA2EPM.Receipts, entry->SpecialTel.EDA2EPM.Value,
			entry->SpecialTel.EDA2TEL.Receipts, entry->SpecialTel.EDA2TEL.Value,
			entry->SpecialTel.EDA2TEL.Receipts, entry->SpecialTel.EDA2TEL.Value,
			//
			cellularEntry->SpecialTel.Receipts, cellularEntry->SpecialTel.Value,
			cellularEntry->SpecialTel.Receipts, cellularEntry->SpecialTel.Value,
			//
			entry->Fax.EDA2EDA.Receipts, entry->Fax.EDA2EDA.TalkMin, entry->Fax.EDA2EDA.Value,
			entry->Fax.EDA2EDA.Receipts, entry->Fax.EDA2EDA.TalkMin, entry->Fax.EDA2EDA.Value,
			entry->Fax.EDA2EPM.Receipts, entry->Fax.EDA2EPM.TalkMin, entry->Fax.EDA2EPM.Value,
			entry->Fax.EDA2EPM.Receipts, entry->Fax.EDA2EPM.TalkMin, entry->Fax.EDA2EPM.Value,
			entry->Fax.EDA2TEL.Receipts, entry->Fax.EDA2TEL.TalkMin, entry->Fax.EDA2TEL.Value,
			entry->Fax.EDA2TEL.Receipts, entry->Fax.EDA2TEL.TalkMin, entry->Fax.EDA2TEL.Value,
#endif
			entry->Internet.Nal.Receipts, entry->Internet.Nal.PaidMin, entry->Internet.Nal.Value,
			entry->Internet.Nal.Receipts, entry->Internet.Nal.PaidMin, entry->Internet.Nal.Value
		);
		// Inter special services
		g_spooler->Printf
		(
			channel, interSpecialFmt,
			entry->SpecialTel.Inter.Receipts, entry->SpecialTel.Inter.Value,
			entry->SpecialTel.Inter.Receipts, entry->SpecialTel.Inter.Value,
			entry->Fax.Inter.Receipts, entry->Fax.Inter.Amount, entry->Fax.Inter.Value,
			entry->Fax.Inter.Receipts, entry->Fax.Inter.Amount, entry->Fax.Inter.Value
		);
		// other services
		if (g_cfg->FORM == CFG::DR_18)
		{
			g_spooler->Printf
			(
				channel, otherSpecialFmt,
				mCards, entry->Cards.Value,
				mCards, entry->Cards.Value,
				entry->Cards.Cards[0], g_cfg->MCARD[0],
				entry->Cards.Cards[0], g_cfg->MCARD[0],
				entry->Cards.Cards[1], g_cfg->MCARD[1],
				entry->Cards.Cards[1], g_cfg->MCARD[1],
				entry->Cards.Cards[2], g_cfg->MCARD[2],
				entry->Cards.Cards[2], g_cfg->MCARD[2],
				entry->Cards.Cards[3], g_cfg->MCARD[3],
				entry->Cards.Cards[3], g_cfg->MCARD[3],
				entry->Other.Receipts, entry->Other.Value,
				entry->Other.Receipts, entry->Other.Value
			);
			// total for special services
			g_spooler->Printf
			(
				channel, specialTotalFmt,
				entry->Total.Special-entry->Tax.Special, entry->Total.Special-entry->Tax.Special,
				g_cfg->TAX_NAME, g_cfg->TAX_NAME,
				entry->Tax.Special, entry->Tax.Special,
				entry->Total.Special, entry->Total.Special
			);
		}
		else
		{
			g_spooler->Printf
			(
				channel, otherSpecialFmt,
				mCards, entry->Cards.Value,
				mCards, entry->Cards.Value,
				entry->Cards.Cards[0], g_cfg->MCARD[0],
				entry->Cards.Cards[1], g_cfg->MCARD[1],
				entry->Cards.Cards[0], g_cfg->MCARD[0],
				entry->Cards.Cards[1], g_cfg->MCARD[1],
				entry->Cards.Cards[2], g_cfg->MCARD[2],
				entry->Cards.Cards[3], g_cfg->MCARD[3],
				entry->Cards.Cards[2], g_cfg->MCARD[2],
				entry->Cards.Cards[3], g_cfg->MCARD[3],
				entry->Other.Receipts, entry->Other.Value,
				entry->Other.Receipts, entry->Other.Value
			);
			// total for special services
			g_spooler->Printf
			(
				channel, specialTotalFmt,
				entry->Total.Special-entry->Tax.Special, entry->Total.Special-entry->Tax.Special,
				g_cfg->TAX_NAME, entry->Tax.Special,
				g_cfg->TAX_NAME, entry->Tax.Special,
				entry->Total.Special, entry->Total.Special
			);
		}
#if defined(__EDA__)
		// EDA totals
		g_spooler->Printf
		(
			channel, edaTotalFmt,
			entry->Total.EDA2EDA-entry->Tax.EDA2EDA, entry->Total.EDA2EDA-entry->Tax.EDA2EDA,
			g_cfg->TAX_NAME, entry->Tax.EDA2EDA, g_cfg->TAX_NAME, entry->Tax.EDA2EDA,
			entry->Total.EDA2EDA, entry->Total.EDA2EDA,
			//
			entry->Total.EDA2EPM-entry->Tax.EDA2EPM, entry->Total.EDA2EPM-entry->Tax.EDA2EPM,
			g_cfg->TAX_NAME, entry->Tax.EDA2EPM, g_cfg->TAX_NAME, entry->Tax.EDA2EPM,
			entry->Total.EDA2EPM, entry->Total.EDA2EPM,
			//
			entry->Total.EDA2TEL-entry->Tax.EDA2TEL, entry->Total.EDA2TEL-entry->Tax.EDA2TEL,
			g_cfg->TAX_NAME, entry->Tax.EDA2TEL, g_cfg->TAX_NAME, entry->Tax.EDA2TEL,
			entry->Total.EDA2TEL, entry->Total.EDA2TEL,
			//
			(cellularEntry->Tel.Value+cellularEntry->SpecialTel.Value)-(cellularEntry->Tel.Tax+cellularEntry->SpecialTel.Tax),
			(cellularEntry->Tel.Value+cellularEntry->SpecialTel.Value)-(cellularEntry->Tel.Tax+cellularEntry->SpecialTel.Tax),
			g_cfg->TAX_NAME, (cellularEntry->Tel.Tax+cellularEntry->SpecialTel.Tax),
			g_cfg->TAX_NAME, (cellularEntry->Tel.Tax+cellularEntry->SpecialTel.Tax),
			(cellularEntry->Tel.Value+cellularEntry->SpecialTel.Value), (cellularEntry->Tel.Value+cellularEntry->SpecialTel.Value)
		);
#endif
		// total automatic and special services
		if (g_cfg->FORM == CFG::DR_18)
		{
			g_spooler->Printf
			(
				channel, totalFmt,
				g_cfg->CURRENCY, entry->Total.General-entry->Tax.General, g_cfg->CURRENCY, entry->Total.General-entry->Tax.General,
				g_cfg->TAX_NAME, g_cfg->TAX_NAME,
				g_cfg->CURRENCY, entry->Tax.General, g_cfg->CURRENCY, entry->Tax.General,
				g_cfg->CURRENCY, entry->Total.General, g_cfg->CURRENCY, entry->Total.General
			);
		}
		else
		{
			g_spooler->Printf
			(
				channel, totalFmt,
				g_cfg->CURRENCY, entry->Total.General-entry->Tax.General, g_cfg->CURRENCY, entry->Total.General-entry->Tax.General,
				g_cfg->TAX_NAME, g_cfg->CURRENCY, entry->Tax.General,
				g_cfg->TAX_NAME, g_cfg->CURRENCY, entry->Tax.General,
				g_cfg->CURRENCY, entry->Total.General, g_cfg->CURRENCY, entry->Total.General
			);
		}
		if (fromTurn && type == DB_STATISTICS::TURN && g_cfg->DOUBLE_PRN)
			g_spooler->Printf
			(
				channel, doublePrnFmt,
				doublePrnEntry0->Receipts, doublePrnEntry0->Value, doublePrnEntry0->Receipts, doublePrnEntry0->Value,
				doublePrnEntry1->Receipts, doublePrnEntry1->Value, doublePrnEntry1->Receipts, doublePrnEntry1->Value
			);
	}
	else
	{
		// titles
		if (type == DB_STATISTICS::TURN)
			g_spooler->Printf
			(
				channel, titleFmt,
				statisticsStr[type],
				g_cfg->OP_NAME,
				fromDateStr, fromTimeStr, toDateStr, toTimeStr,
				g_cfg->RECNO_LABEL, nReceipts, entry->From.Number, entry->To.Number
			);
        else
			g_spooler->Printf
			(
				channel, titleFmt,
				statisticsStr[type],
				fromDateStr, fromTimeStr, toDateStr, toTimeStr,
				g_cfg->RECNO_LABEL, nReceipts, entry->From.Number, entry->To.Number
			);
		// not paid
		g_spooler->Printf
		(
			channel, notPaidFmt,
			entry->NotPaid.Receipts, entry->NotPaid.Value,
			entry->TollFree.Receipts, entry->TollFree.Value,
			entry->Total.NotPaid
		);
		// automatic services
		g_spooler->Printf
		(
			channel, normalFmt,
#if !defined(__EDA__)
			entry->Tel.Nal.Receipts, entry->Tel.Nal.TalkMin, entry->Tel.Nal.PaidMin, entry->Tel.Nal.Value,
#else
			entry->Tel.EDA2EDA.Receipts, entry->Tel.EDA2EDA.TalkMin, entry->Tel.EDA2EDA.PaidMin, entry->Tel.EDA2EDA.Value,
			entry->Tel.EDA2EPM.Receipts, entry->Tel.EDA2EPM.TalkMin, entry->Tel.EDA2EPM.PaidMin, entry->Tel.EDA2EPM.Value,
			entry->Tel.EDA2TEL.Receipts, entry->Tel.EDA2TEL.TalkMin, entry->Tel.EDA2TEL.PaidMin, entry->Tel.EDA2TEL.Value,
#endif
			//
			cellularEntry->Tel.Receipts, cellularEntry->Tel.TalkMin, cellularEntry->Tel.PaidMin, cellularEntry->Tel.Value,
			//
			entry->Tel.Inter.Receipts, entry->Tel.Inter.TalkMin, entry->Tel.Inter.PaidMin, entry->Tel.Inter.Value,
			entry->Total.Tel-entry->Tax.Tel,
			g_cfg->TAX_NAME, entry->Tax.Tel,
			entry->Total.Tel
		);
		// Nal special services
		g_spooler->Printf
		(
			channel, nalSpecialFmt,
#if !defined(__EDA__)
			entry->SpecialTel.Nal.Receipts, entry->SpecialTel.Nal.Value,
			//
			cellularEntry->SpecialTel.Receipts, cellularEntry->SpecialTel.Value,
			//
			entry->Fax.Nal.Receipts, entry->Fax.Nal.Amount, entry->Fax.Nal.Value,
#else
			entry->SpecialTel.EDA2EDA.Receipts, entry->SpecialTel.EDA2EDA.Value,
			entry->SpecialTel.EDA2EPM.Receipts, entry->SpecialTel.EDA2EPM.Value,
			entry->SpecialTel.EDA2TEL.Receipts, entry->SpecialTel.EDA2TEL.Value,
			//
			cellularEntry->SpecialTel.Receipts, cellularEntry->SpecialTel.Value,
			//
			entry->Fax.EDA2EDA.Receipts, entry->Fax.EDA2EDA.TalkMin, entry->Fax.EDA2EDA.Value,
			entry->Fax.EDA2EPM.Receipts, entry->Fax.EDA2EPM.TalkMin, entry->Fax.EDA2EPM.Value,
			entry->Fax.EDA2TEL.Receipts, entry->Fax.EDA2TEL.TalkMin, entry->Fax.EDA2TEL.Value,
#endif
			//
			entry->Internet.Nal.Receipts, entry->Internet.Nal.PaidMin, entry->Internet.Nal.Value
		);
		// Inter special services
		g_spooler->Printf
		(
			channel, interSpecialFmt,
			entry->SpecialTel.Inter.Receipts, entry->SpecialTel.Inter.Value,
			entry->Fax.Inter.Receipts, entry->Fax.Inter.Amount, entry->Fax.Inter.Value
		);
		// other services
		g_spooler->Printf
		(
			channel, otherSpecialFmt,
			mCards, entry->Cards.Value,
			entry->Cards.Cards[0], g_cfg->MCARD[0],
			entry->Cards.Cards[1], g_cfg->MCARD[1],
			entry->Cards.Cards[2], g_cfg->MCARD[2],
			entry->Cards.Cards[3], g_cfg->MCARD[3],
			entry->Other.Receipts, entry->Other.Value
		);
        // total for special services
		g_spooler->Printf
		(
			channel, specialTotalFmt,
			entry->Total.Special-entry->Tax.Special,
			g_cfg->TAX_NAME, entry->Tax.Special,
			entry->Total.Special
		);
#if defined(__EDA__)
        // EDA totals
		g_spooler->Printf
		(
			channel, edaTotalFmt,
			entry->Total.EDA2EDA-entry->Tax.EDA2EDA,
			g_cfg->TAX_NAME, entry->Tax.EDA2EDA,
			entry->Total.EDA2EDA,
			//
			entry->Total.EDA2EPM-entry->Tax.EDA2EPM,
			g_cfg->TAX_NAME, entry->Tax.EDA2EPM,
			entry->Total.EDA2EPM,
			//
			entry->Total.EDA2TEL-entry->Tax.EDA2TEL,
			g_cfg->TAX_NAME, entry->Tax.EDA2TEL,
			entry->Total.EDA2TEL,
			//
			(cellularEntry->Tel.Value+cellularEntry->SpecialTel.Value)-(cellularEntry->Tel.Tax+cellularEntry->SpecialTel.Tax),
			g_cfg->TAX_NAME, (cellularEntry->Tel.Tax+cellularEntry->SpecialTel.Tax),
			(cellularEntry->Tel.Value+cellularEntry->SpecialTel.Value)
		);
#endif
        // total automatic and special services
		g_spooler->Printf
		(
			channel, totalFmt,
			g_cfg->CURRENCY, entry->Total.General-entry->Tax.General,
			g_cfg->TAX_NAME, g_cfg->CURRENCY, entry->Tax.General,
			g_cfg->CURRENCY, entry->Total.General
		);
		if (fromTurn && type == DB_STATISTICS::TURN && g_cfg->DOUBLE_PRN)
			g_spooler->Printf
			(
				channel, doublePrnFmt,
				doublePrnEntry0->Receipts, doublePrnEntry0->Value,
				doublePrnEntry1->Receipts, doublePrnEntry1->Value
			);
	}
}
