//
// [ MB_EXT.CPP ]
//

#include "stdst.h"

#include <ui_win.hpp>
#include <db_eng.h>
#include <spooler.h>
#include <menubar.h>
#include <events.h>
#include <hb_ids.h>
#include <info.h>
#include <sid.h>
#include <cstr.h>
#include <plain_pr.h>
#include <res.hpp>

#ifndef USE_HELP_CONTEXTS
#define USE_HELP_CONTEXTS
#include <help.hpp>
#endif

extern CFG        	*g_cfg;
extern DB_ENGINE  	*g_dbEngine;
extern BOOL       	g_extAreChangeable;
extern SPOOLER    	*g_spooler;

static void PrintConfig(void);
static void PrintHeader(void);
static void PrintFooter(void);
static void PrintFF(void);

// --------------------------------------------------------------------------
//								E_ACCOUNT
// --------------------------------------------------------------------------

static WORD lastExt = 0;

E_ACCOUNT::E_ACCOUNT(void) : UIW_WINDOW("E_ACCOUNT", defaultStorage)
{
	if (g_cfg->E_FIRST_EXT)
        eventManager->Put(UI_EVENT(UE_VIEW, 0)); // force the first event ...
    windowManager->Center(this);
    lastExt = 0; // to avoid repaint or delete the current extension
    //	helpContext = H_E_ACCOUNT;
}



E_ACCOUNT::~E_ACCOUNT(void)
{
}

EVENT_TYPE E_ACCOUNT::Event(const UI_EVENT &event)
{
    // Switch on the type of event.
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case S_CREATE:
        {
            UIW_WINDOW::Event(event);
            //
			((UIW_INTEGER *)Get(SID_BOOTH))->DataSet((int *)&g_cfg->E_FIRST_EXT);
			if (!g_cfg->E_FIRST_EXT)
            {
                Get(SID_BOOTH)->woFlags |= WOF_NON_SELECTABLE;
                Get(SID_OK)->woFlags    |= WOF_NON_SELECTABLE;
                Get(SID_PRINT)->woFlags |= WOF_NON_SELECTABLE;
                Get(SID_ACTIVE)->woFlags |= WOF_NON_SELECTABLE;
                Get(SID_RANGE)->woFlags  |= WOF_NON_SELECTABLE;
                Get(SID_BOOTH)->Event(UI_EVENT(S_NON_CURRENT, 0));
                ((UIW_BUTTON *)Get(SID_CANCEL))->DataSet((char *)CSTR_CLOSE);
                SetCurrent(Get(SID_OK));
                Get(SID_OK)->Event(UI_EVENT(S_CURRENT, 0));
            }
            else
                Get(SID_BOOTH)->userFunction  = ProcessExt;
			if (!g_extAreChangeable)
            {
                Get(SID_OK)->woFlags |= WOF_NON_SELECTABLE;
                Get(SID_ACTIVE)->woFlags |= WOF_NON_SELECTABLE;
                ((UIW_BUTTON *)Get(SID_CANCEL))->DataSet((char *)CSTR_CLOSE);
            }
            Get(SID_FROM)->userFunction = ProcessFrom;
            Get(SID_TO)->userFunction   = ProcessTo;
            //
            UIW_GROUP *group;
            group = (UIW_GROUP *)Get(SID_CREDITS);
            for (int i = 0; i < 3; i++)
            {
                *group
                + (WCredits[i].Text = new UIW_STRING(3, 2+i, 24, (char *)CSTR_EMPTY, 20))
                + (WCredits[i].Time = new UIW_TIME(28, 2+i, 7, &UI_TIME(), NULL, TMF_SYSTEM|TMF_TWENTY_FOUR_HOUR))
                + (WCredits[i].Date = new UIW_DATE(36, 2+i, 11, &UI_DATE(), NULL, DTF_EUROPEAN_FORMAT))
                + (WCredits[i].Value= new UIW_BIGNUM(48, 2+i, 17, &UI_BIGNUM(), NULL, NMF_CURRENCY|NMF_DECIMAL(2),
                                                     WOF_BORDER|WOF_AUTO_CLEAR, ProcessValue))
                ;
            }
			if (!g_extAreChangeable || !g_cfg->E_FIRST_EXT)
                group->woFlags |= WOF_NON_SELECTABLE;
            group = (UIW_GROUP *)Get(SID_DEBITS);
            for (i = 0; i < 2; i++)
            {
                *group
                + (WDebits[i].Text = new UIW_STRING(3, 2+i, 24, (char *)CSTR_EMPTY, 20))
                + (WDebits[i].Time = new UIW_TIME(28, 2+i, 7, &UI_TIME(), NULL, TMF_SYSTEM|TMF_TWENTY_FOUR_HOUR))
                + (WDebits[i].Date = new UIW_DATE(36, 2+i, 11, &UI_DATE(), NULL, DTF_EUROPEAN_FORMAT))
                + (WDebits[i].Value= new UIW_BIGNUM(48, 2+i, 17, &UI_BIGNUM(), NULL, NMF_CURRENCY|NMF_DECIMAL(2),
                                                    WOF_BORDER|WOF_AUTO_CLEAR, ProcessValue))
                ;
            }
			if (!g_extAreChangeable || !g_cfg->E_FIRST_EXT)
                group->woFlags |= WOF_NON_SELECTABLE;
            group = (UIW_GROUP *)Get(SID_OTHERS);
            for (i = 0; i < 2; i++)
            {
                *group
                + (WOthers[i].Text = new UIW_STRING(3, 2+i, 24, (char *)CSTR_EMPTY, 20))
                + (WOthers[i].Time = new UIW_TIME(28, 2+i, 7, &UI_TIME(), NULL, TMF_SYSTEM|TMF_TWENTY_FOUR_HOUR))
                + (WOthers[i].Date = new UIW_DATE(36, 2+i, 11, &UI_DATE(), NULL, DTF_EUROPEAN_FORMAT))
                + (WOthers[i].Value= new UIW_BIGNUM(48, 2+i, 17, &UI_BIGNUM(), NULL, NMF_CURRENCY|NMF_DECIMAL(2),
                                                    WOF_BORDER|WOF_AUTO_CLEAR, ProcessValue))
                ;
            }
			if (!g_extAreChangeable || !g_cfg->E_FIRST_EXT)
                group->woFlags |= WOF_NON_SELECTABLE;
            //
            break;
        }
    case UE_ACCEPT:
        {
            WORD ext = ((UIW_INTEGER *)Get(SID_BOOTH))->DataGet()-1;
			if (!g_extAreChangeable || !g_cfg->E_FIRST_EXT || !(g_cfg->BoothInfo[ext].Attr & CFG::ACTIVE_EXT))
            {
                eventManager->Put(UI_EVENT(S_CLOSE,0));
                break;
            }
            DXS_NON_CRITICAL_ENTRY *ncEntry;
            ncEntry = new DXS_NON_CRITICAL_ENTRY;
            ncEntry->Init();
            // --- Credits
            for (int i = 0; i < 3; i++)
            {
                strcpy(ncEntry->Credits[i].Text, WCredits[i].Text->DataGet());
                WCredits[i].Time->DataGet()->Export((int *)&ncEntry->Credits[i].Time);
                WCredits[i].Date->DataGet()->Export((int *)&ncEntry->Credits[i].Date);
                WCredits[i].Value->DataGet()->Export(&ncEntry->Credits[i].Value);
            }
            // --- Debits
            for (i = 0; i < 2; i++)
            {
                strcpy(ncEntry->Debits[i].Text, WDebits[i].Text->DataGet());
                WDebits[i].Time->DataGet()->Export((int *)&ncEntry->Debits[i].Time);
                WDebits[i].Date->DataGet()->Export((int *)&ncEntry->Debits[i].Date);
                WDebits[i].Value->DataGet()->Export(&ncEntry->Debits[i].Value);
            }
            // --- Others
            for (i = 0; i < 2; i++)
            {
                strcpy(ncEntry->Others[i].Text, WOthers[i].Text->DataGet());
                WOthers[i].Time->DataGet()->Export((int *)&ncEntry->Others[i].Time);
                WOthers[i].Date->DataGet()->Export((int *)&ncEntry->Others[i].Date);
                WOthers[i].Value->DataGet()->Export(&ncEntry->Others[i].Value);
            }
            //
			g_dbEngine->ExtPutNonCriticalEntry(ext, *ncEntry);
            delete ncEntry;
            //
            eventManager->Put(UI_EVENT(S_CLOSE,0));
            break;
        }
    case UE_CANCEL:
        eventManager->Put(UI_EVENT(S_CLOSE,0));
        break;
    case UE_VIEW:
        {
            WORD ext = ((UIW_INTEGER *)Get(SID_BOOTH))->DataGet()-1;
			if (!(g_cfg->BoothInfo[ext].Attr & CFG::ACTIVE_EXT))
            {
                Get(SID_OK)->woFlags      |= WOF_NON_SELECTABLE;
                Get(SID_OK)->Event(UI_EVENT(S_REDISPLAY, 0));
                Get(SID_CREDITS)->woFlags |= WOF_NON_SELECTABLE;
                Get(SID_DEBITS)->woFlags |= WOF_NON_SELECTABLE;
                Get(SID_OTHERS)->woFlags |= WOF_NON_SELECTABLE;
                Get(SID_PRINT)->woFlags |= WOF_NON_SELECTABLE;
                Get(SID_PRINT)->Event(UI_EVENT(S_REDISPLAY, 0));
                ((UIW_BUTTON *)Get(SID_CANCEL))->DataSet((char *)CSTR_CLOSE);
            }
            else
            {
                Get(SID_PRINT)->woFlags &= ~WOF_NON_SELECTABLE;
                Get(SID_PRINT)->Event(UI_EVENT(S_REDISPLAY, 0));
				if (g_extAreChangeable)
                {
                    Get(SID_OK)->woFlags      &= ~WOF_NON_SELECTABLE;
                    Get(SID_OK)->Event(UI_EVENT(S_REDISPLAY, 0));
                    Get(SID_CREDITS)->woFlags &= ~WOF_NON_SELECTABLE;
                    Get(SID_DEBITS)->woFlags &= ~WOF_NON_SELECTABLE;
                    Get(SID_OTHERS)->woFlags &= ~WOF_NON_SELECTABLE;
                    ((UIW_BUTTON *)Get(SID_CANCEL))->DataSet((char *)CSTR_CANCEL);
                }
            }
            //
            DXS_NON_CRITICAL_ENTRY *ncEntry;
			((UIW_STRING *)Get(SID_NAME))->DataSet(g_cfg->BoothInfo[ext].Name);
			ncEntry = g_dbEngine->ExtGetNonCriticalEntry(ext);
            WORD time, date;
            // --- Credits
            for (int i = 0; i < 3; i++)
            {
                if (ncEntry->Credits[i].Value)
                {
                    time = ncEntry->Credits[i].Time;
                    date = ncEntry->Credits[i].Date;
                }
                else
                {
                    time = _GetSysTime();
                    date = _GetSysDate();
                }
                WCredits[i].Text->DataSet(ncEntry->Credits[i].Text);
                WCredits[i].Time->DataSet(&UI_TIME(time));
                WCredits[i].Date->DataSet(&UI_DATE(date));
                WCredits[i].Value->DataSet(&UI_BIGNUM(ncEntry->Credits[i].Value));
            }
            // --- Debits
            for (i = 0; i < 2; i++)
            {
                if (ncEntry->Debits[i].Value)
                {
                    time = ncEntry->Debits[i].Time;
                    date = ncEntry->Debits[i].Date;
                }
                else
                {
                    time = _GetSysTime();
                    date = _GetSysDate();
                }
                WDebits[i].Text->DataSet(ncEntry->Debits[i].Text);
                WDebits[i].Time->DataSet(&UI_TIME(time));
                WDebits[i].Date->DataSet(&UI_DATE(date));
                WDebits[i].Value->DataSet(&UI_BIGNUM(ncEntry->Debits[i].Value));
            }
            // --- Others
            for (i = 0; i < 2; i++)
            {
                if (ncEntry->Others[i].Value)
                {
                    time = ncEntry->Others[i].Time;
                    date = ncEntry->Others[i].Date;
                }
                else
                {
                    time = _GetSysTime();
                    date = _GetSysDate();
                }
                WOthers[i].Text->DataSet(ncEntry->Others[i].Text);
                WOthers[i].Time->DataSet(&UI_TIME(time));
                WOthers[i].Date->DataSet(&UI_DATE(date));
                WOthers[i].Value->DataSet(&UI_BIGNUM(ncEntry->Others[i].Value));
            }
            // update totals
            double total = 0;
			if (g_cfg->BoothInfo[ext].Attr & CFG::ACTIVE_EXT)
            {
                for (i=0; i < 3; i++)
                    total += ncEntry->Credits[i].Value;
                for (i=0; i < 2; i++)
                    total += ncEntry->Debits[i].Value;
            }
            ((UIW_BIGNUM *)Get(SID_TOTAL))->DataSet(&UI_BIGNUM(total));
            double charges = 0;
			if (g_cfg->BoothInfo[ext].Attr & CFG::ACTIVE_EXT)
                for (i=0; i < 2; i++)
                    charges += ncEntry->Others[i].Value;
            DXS_CRITICAL_ENTRY::ONLINE_ENTRY *olEntry;
			olEntry = &g_dbEngine->ExtGetCritical()->Online[ext];
            double costOfCalls = 0;
			if (g_cfg->BoothInfo[ext].Attr & CFG::ACTIVE_EXT)
                costOfCalls = olEntry->DDN.Cost + olEntry->DDI.Cost;
            ((UIW_BIGNUM *)Get(SID_CALLS))->DataSet(&UI_BIGNUM(costOfCalls));
			if (g_cfg->BoothInfo[ext].Attr & CFG::ACTIVE_EXT)
				charges += (g_cfg->E_INSTALL_COST + g_cfg->E_LINE_COST);
            ((UIW_BIGNUM *)Get(SID_CHARGES))->DataSet(&UI_BIGNUM(charges));
			double discount = (g_cfg->E_DISCOUNT/100)*costOfCalls;
            ((UIW_BIGNUM *)Get(SID_DISCOUNT))->DataSet(&UI_BIGNUM(discount));
			if (g_cfg->BoothInfo[ext].Attr & CFG::ACTIVE_EXT)
                total -= (costOfCalls+charges-discount);
            ((UIW_BIGNUM *)Get(SID_AVAIL))->DataSet(&UI_BIGNUM(total));
            // put range of receipts
            DWORD numOfCalls = olEntry->DDN.NumOfCalls+olEntry->DDI.NumOfCalls;
            if (numOfCalls)
            {
                ((UIW_BIGNUM *)Get(SID_FROM))->DataSet(&UI_BIGNUM(1L));
                ((UIW_BIGNUM *)Get(SID_TO))->DataSet(&UI_BIGNUM((long)numOfCalls));
                Get(SID_RANGE)->woFlags &= ~WOF_NON_SELECTABLE;
            }
            else
            {
                ((UIW_BIGNUM *)Get(SID_FROM))->DataSet(&UI_BIGNUM(0L));
                ((UIW_BIGNUM *)Get(SID_TO))->DataSet(&UI_BIGNUM(0L));
                Get(SID_RANGE)->woFlags |= WOF_NON_SELECTABLE;
            }
            // update "Activar"
			if (g_cfg->BoothInfo[ext].Attr & CFG::ACTIVE_EXT)
                ((UIW_BUTTON *)Get(SID_ACTIVE))->DataSet((char *)CSTR_DEACTIVATE);
            else
                ((UIW_BUTTON *)Get(SID_ACTIVE))->DataSet((char *)CSTR_ACTIVATE);
            //
            break;
        }
    case UE_PAY:
        { // use for "Activar"/"Desactivar"
            // request confirmation
            if (errorSystem->ReportError(windowManager, WOS_INVALID, (char *)CSTR_CRITICAL) != WOS_INVALID)
                break;
            WORD ext = ((UIW_INTEGER *)Get(SID_BOOTH))->DataGet()-1;
			if (g_cfg->BoothInfo[ext].Attr & CFG::ACTIVE_EXT)
            {
                UIW_WINDOW *msgWindow = new UIW_WINDOW("MESSAGE", UI_WINDOW_OBJECT::defaultStorage);
                UI_WINDOW_OBJECT::windowManager->Center(msgWindow);
                (void *)&(*UI_WINDOW_OBJECT::windowManager + msgWindow);
                eventManager->DeviceState(E_MOUSE, DM_WAIT);
                // zap records !!!
                DXS_CRITICAL_ENTRY::ONLINE_ENTRY *olEntry;
				olEntry = &g_dbEngine->ExtGetCritical()->Online[ext];
				DWORD higher = g_dbEngine->ExtGetLastNumber();
                for (DWORD number = olEntry->FirstReceipt; number <= higher; number++)
					g_dbEngine->ExtDelete(number, ext);
                //
                (void *)&(*UI_WINDOW_OBJECT::windowManager - msgWindow);
                delete msgWindow;
                eventManager->DeviceState(E_MOUSE, DM_VIEW);
                // Adjust statistics
				g_dbEngine->ExtStore(ext);
                //
                ((UIW_BUTTON *)Get(SID_ACTIVE))->DataSet((char *)CSTR_ACTIVATE);
				g_cfg->BoothInfo[ext].Attr &= ~CFG::ACTIVE_EXT;
            }
            else
            {
                ((UIW_BUTTON *)Get(SID_ACTIVE))->DataSet((char *)CSTR_DEACTIVATE);
				g_cfg->BoothInfo[ext].Attr |= CFG::ACTIVE_EXT;
            }
			g_cfg->Save(NULL, FALSE);
            eventManager->Put(UI_EVENT(UE_VIEW, 0)); // redisplay ...
            //
            break;
        }
    case UE_PRINT:
        {
            //
            // Print Accout
            //
            PrintConfig();
            PrintHeader();
            PrintData();
            PrintFooter();
            PrintFF();
            //
            // Print receipts
            //
            if (FlagSet(((UIW_BUTTON *)Get(SID_PR_RECS))->woStatus, WOS_SELECTED))
            {
                DWORD virtualFrom;
                ((UIW_BIGNUM *)Get(SID_FROM))->DataGet()->Export((long *)&virtualFrom);
                DWORD from = virtualFrom-1;
                DWORD numOfReceipts;
                ((UIW_BIGNUM *)Get(SID_TO))->DataGet()->Export((long *)&numOfReceipts);
                numOfReceipts -= from;
                WORD ext = ((UIW_INTEGER *)Get(SID_BOOTH))->DataGet()-1;
                DXS_CRITICAL_ENTRY::ONLINE_ENTRY *olEntry;
				olEntry = &g_dbEngine->ExtGetCritical()->Online[ext];
                from += olEntry->FirstReceipt;
				*windowManager
					+ new PLAIN_PRINTOUT
					(
						from,
						numOfReceipts,
						ext,
						TRUE,
						virtualFrom
					);
            }
            break;
        }
    default:
        ccode = UIW_WINDOW::Event(event);
        break;
    }
    return ccode;
}

EVENT_TYPE E_ACCOUNT::ProcessExt(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
    if (ccode != S_NON_CURRENT && ccode != L_SELECT)
        return ccode;
    WORD ext = ((UIW_INTEGER *)object)->DataGet();
	if (ext < g_cfg->E_FIRST_EXT || ext > g_cfg->ACTIVE_CLUSTERS*CLUSTER_SIZE)
    {
        errorSystem->ReportError(windowManager, WOS_NO_STATUS, (char *)CSTR_BAD_EXT);
        return -1;
    }
    if (ext != lastExt)
    {
        lastExt = ext;
        eventManager->Put(UI_EVENT(UE_VIEW, 0));
    }
    return ccode;
}

EVENT_TYPE E_ACCOUNT::ProcessValue(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
    if (ccode != S_NON_CURRENT && ccode != L_SELECT)
        return ccode;
    // update totals
    UI_WINDOW_OBJECT *parent = object->parent->parent;
    double value;
    double total = 0;
    // --- Credits
    for (int i = 0; i < 3; i++)
    {
        ((E_ACCOUNT *)parent)->WCredits[i].Value->DataGet()->Export(&value);
        total += value;
    }
    // --- Debits
    for (i = 0; i < 2; i++)
    {
        ((E_ACCOUNT *)parent)->WDebits[i].Value->DataGet()->Export(&value);
        total += value;
    }
    ((UIW_BIGNUM *)parent->Get(SID_TOTAL))->DataSet(&UI_BIGNUM(total));
    // --- Others + charges
    double charges = 0;
    for (i = 0; i < 2; i++)
    {
        ((E_ACCOUNT *)parent)->WOthers[i].Value->DataGet()->Export(&value);
        charges += value;
    }
    WORD ext = ((UIW_INTEGER *)parent->Get(SID_BOOTH))->DataGet()-1;
    DXS_CRITICAL_ENTRY::ONLINE_ENTRY *olEntry;
	olEntry = &g_dbEngine->ExtGetCritical()->Online[ext];
    double costOfCalls = olEntry->DDN.Cost + olEntry->DDI.Cost;
    ((UIW_BIGNUM *)parent->Get(SID_CALLS))->DataSet(&UI_BIGNUM(costOfCalls));
	charges += (g_cfg->E_INSTALL_COST + g_cfg->E_LINE_COST);
    ((UIW_BIGNUM *)parent->Get(SID_CHARGES))->DataSet(&UI_BIGNUM(charges));
	double discount = (g_cfg->E_DISCOUNT/100)*costOfCalls;
    ((UIW_BIGNUM *)parent->Get(SID_DISCOUNT))->DataSet(&UI_BIGNUM(discount));
    total -= (costOfCalls+charges-discount);
    ((UIW_BIGNUM *)parent->Get(SID_AVAIL))->DataSet(&UI_BIGNUM(total));
    //
    return ccode;
}

EVENT_TYPE E_ACCOUNT::ProcessFrom(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
    if (ccode != S_NON_CURRENT && ccode != L_SELECT)
        return ccode;
    UI_WINDOW_OBJECT *parent = object->parent->parent;
    DWORD to;
    ((UIW_BIGNUM *)parent->Get(SID_TO))->DataGet()->Export((long *)&to);
    DXS_CRITICAL_ENTRY::ONLINE_ENTRY *olEntry;
    WORD ext = ((UIW_INTEGER *)parent->Get(SID_BOOTH))->DataGet()-1;
	olEntry = &g_dbEngine->ExtGetCritical()->Online[ext];
    DWORD numOfCalls = olEntry->DDN.NumOfCalls+olEntry->DDI.NumOfCalls;
    DWORD from;
    ((UIW_BIGNUM *)object)->DataGet()->Export((long *)&from);
    if (!from || from > numOfCalls || from > to)
    {
        errorSystem->ReportError(windowManager, WOS_NO_STATUS, (char *)CSTR_BAD_RANGE);
        return -1;
    }
    return ccode;
}

EVENT_TYPE E_ACCOUNT::ProcessTo(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
    if (ccode != S_NON_CURRENT && ccode != L_SELECT)
        return ccode;
    UI_WINDOW_OBJECT *parent = object->parent->parent;
    DWORD from;
    ((UIW_BIGNUM *)parent->Get(SID_FROM))->DataGet()->Export((long *)&from);
    DXS_CRITICAL_ENTRY::ONLINE_ENTRY *olEntry;
    WORD ext = ((UIW_INTEGER *)parent->Get(SID_BOOTH))->DataGet()-1;
	olEntry = &g_dbEngine->ExtGetCritical()->Online[ext];
    DWORD numOfCalls = olEntry->DDN.NumOfCalls+olEntry->DDI.NumOfCalls;
    DWORD to;
    ((UIW_BIGNUM *)object)->DataGet()->Export((long *)&to);
    if (!to || to > numOfCalls || to < from)
    {
        errorSystem->ReportError(windowManager, WOS_NO_STATUS, (char *)CSTR_BAD_RANGE);
        return -1;
    }
    return ccode;
}

void E_ACCOUNT::PrintData(void)
{
    char *extFmt;
    char *creditsFmt0, *creditsFmt1, *creditsFmt2;
    char *debitsFmt0, *debitsFmt1;
    char *othersFmt0, *othersFmt1;
    char *totalFmt;
    BOOL mirrored = FALSE;
    //
	switch (g_cfg->FORM)
    {
    case CFG::SR_80:
        {
            extFmt =
                "\tExtensi�n""\n"
                "\t  [%d] %s""\n"
                ;
            creditsFmt0 =
                "\tAbonos"         "\n"
                "\t  %s %s %s %.2f""\n"
                ;
            creditsFmt1 =
                "\t  %s %s %s %.2f""\n"
                ;
            creditsFmt2 =
                "\t  %s %s %s %.2f""\n"
                ;
            debitsFmt0 =
                "\tCuenta Cte."    "\n"
                "\t  %s %s %s %.2f""\n"
                ;
            debitsFmt1 =
                "\t  %s %s %s %.2f""\n"
                ;
            othersFmt0 =
                "\tOtros Cargos"   "\n"
                "\t  %s %s %s %.2f""\n"
                ;
            othersFmt1 =
                "\t  %s %s %s %.2f""\n"
                ;
            totalFmt =
                "\tTotales"                   "\n"
                "\t  Abonos/cuenta cte.: %.2f""\n"
                "\t  Cargos: %.2f"            "\n"
                "\t  LLamadas: [%ld] %.2f"    "\n"
                "\t  Descuento: %.2f"         "\n"
                "\t  Disponible: %.2f"        "\n"
                ;
            mirrored = FALSE;
            break;
        }
    case CFG::DR_EME:
    case CFG::DR_HALF:
    case CFG::DR_80:
    case CFG::DR_PRE:
    case CFG::LINEAL_80:
        {
            extFmt =
                "\tExtensi�n""\tExtensi�n""\n"
                "\t  [%d] %s""\t  [%d] %s""\n"
                ;
            creditsFmt0 =
                "\tAbonos"         "\tAbonos""\n"
                "\t  %s %s %s %.2f""\t  %s %s %s %.2f""\n"
                ;
            creditsFmt1 =
                "\t  %s %s %s %.2f""\t  %s %s %s %.2f""\n"
                ;
            creditsFmt2 =
                "\t  %s %s %s %.2f""\t  %s %s %s %.2f""\n"
                ;
            debitsFmt0 =
                "\tCuenta Cte."    "\tCuenta Cte.""\n"
                "\t  %s %s %s %.2f""\t  %s %s %s %.2f""\n"
                ;
            debitsFmt1 =
                "\t  %s %s %s %.2f""\t  %s %s %s %.2f""\n"
                ;
            othersFmt0 =
                "\tOtros Cargos"   "\tOtros Cargos\n"
                "\t  %s %s %s %.2f""\t  %s %s %s %.2f""\n"
                ;
            othersFmt1 =
                "\t  %s %s %s %.2f""\t  %s %s %s %.2f""\n"
                ;
            totalFmt =
                "\tTotales"                   "\tTotales""\n"
                "\t  Abonos/cuenta cte.: %.2f""\t  Abonos/cuenta cte.: %.2f""\n"
                "\t  Cargos: %.2f"            "\t  Cargos: %.2f""\n"
                "\t  LLamadas: [%ld] %.2f"    "\t  LLamadas: [%ld] %.2f""\n"
                "\t  Descuento: %.2f"         "\t  Descuento: %.2f""\n"
                "\t  Disponible: %.2f"        "\t  Disponible: %.2f""\n"
                ;
            mirrored = TRUE;
            break;
        }
    case CFG::DR_40:
    case CFG::SR_40:
        {
            extFmt =
                "Extensi�n""\n"
                "  [%d] %s""\n"
                ;
            creditsFmt0 =
                "Abonos""\n"
                "  %s %s %s %.2f""\n"
                ;
            creditsFmt1 =
                "  %s %s %s %.2f""\n"
                ;
            creditsFmt2 =
                "  %s %s %s %.2f""\n"
                ;
            debitsFmt0 =
                "Cuenta Cte.""\n"
                "  %s %s %s %.2f""\n"
                ;
            debitsFmt1 =
                "  %s %s %s %.2f""\n"
                ;
            othersFmt0 =
                "Otros Cargos""\n"
                "  %s %s %s %.2f""\n"
                ;
            othersFmt1 =
                "  %s %s %s %.2f""\n"
                ;
            totalFmt =
                "Totales""\n"
                "  Abonos/cuenta cte.: %.2f""\n"
                "  Cargos: %.2f""\n"
                "  LLamadas: [%ld] %.2f""\n"
                "  Descuento: %.2f""\n"
                "  Disponible: %.2f""\n"
                ;
            break;
        }
    case CFG::DR_18:
    case CFG::SR_28:
        return ; // nothing to do
    }
    //


    BYTE channel = 0;
    WORD ext = ((UIW_INTEGER *)Get(SID_BOOTH))->DataGet()-1;
    //
    DXS_CRITICAL_ENTRY::ONLINE_ENTRY *olEntry;
	olEntry = &g_dbEngine->ExtGetCritical()->Online[ext];
    double costOfCalls = olEntry->DDN.Cost + olEntry->DDI.Cost;
    DWORD  numOfCalls = olEntry->DDN.NumOfCalls + olEntry->DDI.NumOfCalls;
	double discount = (g_cfg->E_DISCOUNT/100)*costOfCalls;
    //
    double total = 0, avail;
	double charges = g_cfg->E_INSTALL_COST + g_cfg->E_LINE_COST;
    DXS_NON_CRITICAL_ENTRY *ncEntry;
    ncEntry = new DXS_NON_CRITICAL_ENTRY;
    // --- Credits
    for (int i = 0; i < 3; i++)
    {
        strcpy(ncEntry->Credits[i].Text, WCredits[i].Text->DataGet());
        WCredits[i].Time->DataGet()->Export((int *)&ncEntry->Credits[i].Time);
        WCredits[i].Date->DataGet()->Export((int *)&ncEntry->Credits[i].Date);
        WCredits[i].Value->DataGet()->Export(&ncEntry->Credits[i].Value);
        total += ncEntry->Credits[i].Value;
    }
    // --- Debits
    for (i = 0; i < 2; i++)
    {
        strcpy(ncEntry->Debits[i].Text, WDebits[i].Text->DataGet());
        WDebits[i].Time->DataGet()->Export((int *)&ncEntry->Debits[i].Time);
        WDebits[i].Date->DataGet()->Export((int *)&ncEntry->Debits[i].Date);
        WDebits[i].Value->DataGet()->Export(&ncEntry->Debits[i].Value);
        total += ncEntry->Debits[i].Value;
    }
    // --- Others
    for (i = 0; i < 2; i++)
    {
        strcpy(ncEntry->Others[i].Text, WOthers[i].Text->DataGet());
        WOthers[i].Time->DataGet()->Export((int *)&ncEntry->Others[i].Time);
        WOthers[i].Date->DataGet()->Export((int *)&ncEntry->Others[i].Date);
        WOthers[i].Value->DataGet()->Export(&ncEntry->Others[i].Value);
        charges += ncEntry->Others[i].Value;
    }
    //
    char dateStr[0x0F], timeStr[0x0F];
    UI_TIME time;
    int tmFlags = TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL|TMF_SECONDS;
    UI_DATE date;
    int dtFlags = DTF_SLASH|DTF_EUROPEAN_FORMAT;
    if (mirrored)
    {
		g_spooler->Printf(channel, extFmt,
						 ext+1, g_cfg->BoothInfo[ext].Name,
						 ext+1, g_cfg->BoothInfo[ext].Name
                        );
        if (ncEntry->Credits[0].Value)
        {
            time.Import(ncEntry->Credits[0].Time);
            time.Export(timeStr, tmFlags);
            date.Import(ncEntry->Credits[0].Date);
            date.Export(dateStr, dtFlags);
			g_spooler->Printf(channel, creditsFmt0,
                             ncEntry->Credits[0].Text, timeStr, dateStr, ncEntry->Credits[0].Value,
                             ncEntry->Credits[0].Text, timeStr, dateStr, ncEntry->Credits[0].Value
                            );
        }
        if (ncEntry->Credits[1].Value)
        {
            time.Import(ncEntry->Credits[1].Time);
            time.Export(timeStr, tmFlags);
            date.Import(ncEntry->Credits[1].Date);
            date.Export(dateStr, dtFlags);
			g_spooler->Printf(channel, creditsFmt1,
                             ncEntry->Credits[1].Text, timeStr, dateStr, ncEntry->Credits[1].Value,
                             ncEntry->Credits[1].Text, timeStr, dateStr, ncEntry->Credits[1].Value
                            );
        }
        if (ncEntry->Credits[2].Value)
        {
            time.Import(ncEntry->Credits[2].Time);
            time.Export(timeStr, tmFlags);
            date.Import(ncEntry->Credits[2].Date);
            date.Export(dateStr, dtFlags);
			g_spooler->Printf(channel, creditsFmt2,
                             ncEntry->Credits[2].Text, timeStr, dateStr, ncEntry->Credits[2].Value,
                             ncEntry->Credits[2].Text, timeStr, dateStr, ncEntry->Credits[2].Value
                            );
        }
        if (ncEntry->Debits[0].Value)
        {
            time.Import(ncEntry->Debits[0].Time);
            time.Export(timeStr, tmFlags);
            date.Import(ncEntry->Debits[0].Date);
            date.Export(dateStr, dtFlags);
			g_spooler->Printf(channel, debitsFmt0,
                             ncEntry->Debits[0].Text, timeStr, dateStr, ncEntry->Debits[0].Value,
                             ncEntry->Debits[0].Text, timeStr, dateStr, ncEntry->Debits[0].Value
                            );
        }
        if (ncEntry->Debits[1].Value)
        {
            time.Import(ncEntry->Debits[1].Time);
            time.Export(timeStr, tmFlags);
            date.Import(ncEntry->Debits[1].Date);
            date.Export(dateStr, dtFlags);
			g_spooler->Printf(channel, debitsFmt1,
                             ncEntry->Debits[1].Text, timeStr, dateStr, ncEntry->Debits[1].Value,
                             ncEntry->Debits[1].Text, timeStr, dateStr, ncEntry->Debits[1].Value
                            );
        }
        if (ncEntry->Others[0].Value)
        {
            time.Import(ncEntry->Others[0].Time);
            time.Export(timeStr, tmFlags);
            date.Import(ncEntry->Others[0].Date);
            date.Export(dateStr, dtFlags);
			g_spooler->Printf(channel, othersFmt0,
                             ncEntry->Others[0].Text, timeStr, dateStr, ncEntry->Others[0].Value,
                             ncEntry->Others[0].Text, timeStr, dateStr, ncEntry->Others[0].Value
                            );
        }
        if (ncEntry->Others[1].Value)
        {
            time.Import(ncEntry->Others[1].Time);
            time.Export(timeStr, tmFlags);
            date.Import(ncEntry->Others[1].Date);
            date.Export(dateStr, dtFlags);
			g_spooler->Printf(channel, othersFmt1,
                             ncEntry->Others[1].Text, timeStr, dateStr, ncEntry->Others[1].Value,
                             ncEntry->Others[1].Text, timeStr, dateStr, ncEntry->Others[1].Value
                            );
        }
        avail = total - costOfCalls - charges + discount;
		g_spooler->Printf(channel, totalFmt,
                         total, total,
                         charges, charges,
                         numOfCalls, costOfCalls, numOfCalls, costOfCalls,
                         discount, discount,
                         avail, avail
                        );
    }
    else
    {
		g_spooler->Printf(channel, extFmt,
						 ext+1, g_cfg->BoothInfo[ext].Name
                        );
        if (ncEntry->Credits[0].Value)
        {
            time.Import(ncEntry->Credits[0].Time);
            time.Export(timeStr, tmFlags);
            date.Import(ncEntry->Credits[0].Date);
            date.Export(dateStr, dtFlags);
			g_spooler->Printf(channel, creditsFmt0,
                             ncEntry->Credits[0].Text, timeStr, dateStr, ncEntry->Credits[0].Value
                            );
        }
        if (ncEntry->Credits[1].Value)
        {
            time.Import(ncEntry->Credits[1].Time);
            time.Export(timeStr, tmFlags);
            date.Import(ncEntry->Credits[1].Date);
            date.Export(dateStr, dtFlags);
			g_spooler->Printf(channel, creditsFmt1,
                             ncEntry->Credits[1].Text, timeStr, dateStr, ncEntry->Credits[1].Value
                            );
        }
        if (ncEntry->Credits[2].Value)
        {
            time.Import(ncEntry->Credits[2].Time);
            time.Export(timeStr, tmFlags);
            date.Import(ncEntry->Credits[2].Date);
            date.Export(dateStr, dtFlags);
			g_spooler->Printf(channel, creditsFmt2,
                             ncEntry->Credits[2].Text, timeStr, dateStr, ncEntry->Credits[2].Value
                            );
        }
        if (ncEntry->Debits[0].Value)
        {
            time.Import(ncEntry->Debits[0].Time);
            time.Export(timeStr, tmFlags);
            date.Import(ncEntry->Debits[0].Date);
            date.Export(dateStr, dtFlags);
			g_spooler->Printf(channel, debitsFmt0,
                             ncEntry->Debits[0].Text, timeStr, dateStr, ncEntry->Debits[0].Value
                            );
        }
        if (ncEntry->Debits[1].Value)
        {
            time.Import(ncEntry->Debits[1].Time);
            time.Export(timeStr, tmFlags);
            date.Import(ncEntry->Debits[1].Date);
            date.Export(dateStr, dtFlags);
			g_spooler->Printf(channel, debitsFmt1,
                             ncEntry->Debits[1].Text, timeStr, dateStr, ncEntry->Debits[1].Value
                            );
        }
        if (ncEntry->Others[0].Value)
        {
            time.Import(ncEntry->Others[0].Time);
            time.Export(timeStr, tmFlags);
            date.Import(ncEntry->Others[0].Date);
            date.Export(dateStr, dtFlags);
			g_spooler->Printf(channel, othersFmt0,
                             ncEntry->Others[0].Text, timeStr, dateStr, ncEntry->Others[0].Value			);
        }
        if (ncEntry->Others[1].Value)
        {
            time.Import(ncEntry->Others[1].Time);
            time.Export(timeStr, tmFlags);
            date.Import(ncEntry->Others[1].Date);
            date.Export(dateStr, dtFlags);
			g_spooler->Printf(channel, othersFmt1,
                             ncEntry->Others[1].Text, timeStr, dateStr, ncEntry->Others[1].Value			);
        }
        avail = total - costOfCalls - charges + discount;
		g_spooler->Printf(channel, totalFmt,
                         total,
                         charges,
                         numOfCalls, costOfCalls,
                         discount,
                         avail
                        );
    }
    delete ncEntry;
}

// --------------------------------------------------------------------------
//     E_ACCUM
// --------------------------------------------------------------------------

E_ACCUM::E_ACCUM(void) : UIW_WINDOW("E_ACCUM", defaultStorage)
{
    windowManager->Center(this);
    //	helpContext = H_E_ACCUM;
}



E_ACCUM::~E_ACCUM(void)
{}



EVENT_TYPE E_ACCUM::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case S_CREATE:
        {
            UIW_WINDOW::Event(event);
			((UIW_STRING *)Get(SID_TAX_NAME))->DataSet(g_cfg->TAX_NAME);
			if (!g_cfg->E_FIRST_EXT)
                break;
            //
            Get(SID_PRINT)->woFlags &= ~WOF_NON_SELECTABLE;
			if (g_extAreChangeable)
                Get(SID_ARCHIVE)->woFlags &= ~WOF_NON_SELECTABLE;
            // range
            char range[0x10];
			sprintf(range, "%d..%d", g_cfg->E_FIRST_EXT, g_cfg->ACTIVE_CLUSTERS*CLUSTER_SIZE);
            ((UIW_STRING *)Get(SID_RANGE))->DataSet(range);
            //
            DXS_CRITICAL_ENTRY::ONLINE_ENTRY *online;
			online = g_dbEngine->ExtGetCritical()->Online;
            DXS_CRITICAL_ENTRY::STORED_ENTRY *stored;
			stored = &g_dbEngine->ExtGetCritical()->Stored;
            DXS_NON_CRITICAL_ENTRY *nonCritical;
			nonCritical = g_dbEngine->ExtGetNonCriticalEntry();
            //
            DWORD ddnCalls = stored->DDN.NumOfCalls;
            double ddnCost = stored->DDN.Cost;
            double ddnTax = stored->DDN.Tax;
            DWORD ddiCalls = stored->DDI.NumOfCalls;
            double ddiCost = stored->DDI.Cost;
            double ddiTax = stored->DDI.Tax;
            double credits = stored->Credits;
            double debits = stored->Debits;
            double others = stored->Others;
            double line = stored->Line;
            double install = stored->Install;
            for (int i = 0; i < MAX_BOOTH; i++)
            {
                ddnCalls += online[i].DDN.NumOfCalls;
                ddnCost  += online[i].DDN.Cost;
                ddnTax   += online[i].DDN.Tax;
                //
                ddiCalls += online[i].DDI.NumOfCalls;
                ddiCost  += online[i].DDI.Cost;
                ddiTax   += online[i].DDI.Tax;
                //
				if (g_cfg->BoothInfo[i].Attr & CFG::ACTIVE_EXT)
                {
                    for (int j=0; j<3; j++)
                    {
                        credits += nonCritical[i].Credits[j].Value;
                        debits += nonCritical[i].Debits[j].Value;
                        others += nonCritical[i].Others[j].Value;
                    }
					line += g_cfg->E_LINE_COST;
					install += g_cfg->E_INSTALL_COST;
                }
            }
            ((UIW_BIGNUM *)Get(SID_N_AMOUNT))->DataSet(&UI_BIGNUM((long)ddnCalls));
            ((UIW_BIGNUM *)Get(SID_N_COST))->DataSet(&UI_BIGNUM(ddnCost));
            ((UIW_BIGNUM *)Get(SID_N_TAX))->DataSet(&UI_BIGNUM(ddnTax));
            //
            ((UIW_BIGNUM *)Get(SID_I_AMOUNT))->DataSet(&UI_BIGNUM((long)ddiCalls));
            ((UIW_BIGNUM *)Get(SID_I_COST))->DataSet(&UI_BIGNUM(ddiCost));
            ((UIW_BIGNUM *)Get(SID_I_TAX))->DataSet(&UI_BIGNUM(ddiTax));
            //
            double cost = ddnCost+ddiCost;
            ((UIW_BIGNUM *)Get(SID_AMOUNT))->DataSet(&UI_BIGNUM((long)(ddnCalls+ddiCalls)));
            ((UIW_BIGNUM *)Get(SID_COST))->DataSet(&UI_BIGNUM(cost));
            ((UIW_BIGNUM *)Get(SID_TAX))->DataSet(&UI_BIGNUM(ddnTax+ddiTax));
            //
            ((UIW_BIGNUM *)Get(SID_CREDITS))->DataSet(&UI_BIGNUM(credits));
            ((UIW_BIGNUM *)Get(SID_DEBITS))->DataSet(&UI_BIGNUM(debits));
            ((UIW_BIGNUM *)Get(SID_LINE))->DataSet(&UI_BIGNUM(line));
            ((UIW_BIGNUM *)Get(SID_INSTALL))->DataSet(&UI_BIGNUM(install));
            ((UIW_BIGNUM *)Get(SID_OTHERS))->DataSet(&UI_BIGNUM(others));
            ((UIW_BIGNUM *)Get(SID_CALLS))->DataSet(&UI_BIGNUM(cost));
			double discount = (g_cfg->E_DISCOUNT/100)*cost;
            ((UIW_BIGNUM *)Get(SID_DISCOUNT))->DataSet(&UI_BIGNUM(discount));
            double remainder;
            remainder = credits + debits - cost - line - install - others + discount;
            ((UIW_BIGNUM *)Get(SID_REMAINDER))->DataSet(&UI_BIGNUM(remainder));
            //
            break;
        }
    case UE_PAY: // use for "Archivar"
        BOOL error = FALSE;
		for (int i = g_cfg->E_FIRST_EXT-1; !error && i < g_cfg->ACTIVE_CLUSTERS*CLUSTER_SIZE; i++)
			if (g_cfg->BoothInfo[i].Attr & CFG::ACTIVE_EXT)
            {
				errorSystem->ReportError(windowManager, WOS_NO_STATUS, "La extensi�n \"%s\" est� en operaci�n.", g_cfg->BoothInfo[i].Name);
                error = TRUE;
            }
        if (error)
            break;
        // request confirmation
        if (errorSystem->ReportError(windowManager, WOS_INVALID, (char *)CSTR_CRITICAL) != WOS_INVALID)
            break;
        // archivar
        g_dbEngine->ExtArchive();
        //
        eventManager->Put(UI_EVENT(S_CLOSE,0));
        break;
    case UE_PRINT:
        {
            PrintConfig();
            PrintHeader();
            PrintData();
            PrintFooter();
            PrintFF();
            eventManager->Put(UI_EVENT(S_CLOSE,0));
            break;
        }
    case UE_CANCEL:
        eventManager->Put(UI_EVENT(S_CLOSE,0));
        break;
    default:
        ccode = UIW_WINDOW::Event(event);
        break;
    }
    return ccode;
}

void E_ACCUM::PrintData(void)
{
    char   range[0x10];
    DWORD  ddnCalls;
    double ddnCost;
    double ddnTax;
    DWORD  ddiCalls;
    double ddiCost;
    double ddiTax;
    DWORD  calls;
    double cost;
    double tax;
    double credits;
    double debits;
    double others;
    double line;
    double install;
    double discount;
    double remainder;
    //
    strcpy(range, ((UIW_STRING *)Get(SID_RANGE))->DataGet());
    ((UIW_BIGNUM *)Get(SID_N_AMOUNT))->DataGet()->Export((long *)&ddnCalls);
    ((UIW_BIGNUM *)Get(SID_N_COST))->DataGet()->Export(&ddnCost);
    ((UIW_BIGNUM *)Get(SID_N_TAX))->DataGet()->Export(&ddnTax);
    ((UIW_BIGNUM *)Get(SID_I_AMOUNT))->DataGet()->Export((long *)&ddiCalls);
    ((UIW_BIGNUM *)Get(SID_I_COST))->DataGet()->Export(&ddiCost);
    ((UIW_BIGNUM *)Get(SID_I_TAX))->DataGet()->Export(&ddiTax);
    ((UIW_BIGNUM *)Get(SID_AMOUNT))->DataGet()->Export((long *)&calls);
    ((UIW_BIGNUM *)Get(SID_COST))->DataGet()->Export(&cost);
    ((UIW_BIGNUM *)Get(SID_TAX))->DataGet()->Export(&tax);
    ((UIW_BIGNUM *)Get(SID_CREDITS))->DataGet()->Export(&credits);
    ((UIW_BIGNUM *)Get(SID_DEBITS))->DataGet()->Export(&debits);
    ((UIW_BIGNUM *)Get(SID_LINE))->DataGet()->Export(&line);
    ((UIW_BIGNUM *)Get(SID_INSTALL))->DataGet()->Export(&install);
    ((UIW_BIGNUM *)Get(SID_OTHERS))->DataGet()->Export(&others);
    ((UIW_BIGNUM *)Get(SID_DISCOUNT))->DataGet()->Export(&discount);
    ((UIW_BIGNUM *)Get(SID_REMAINDER))->DataGet()->Export(&remainder);
    // Print
    char *rangeFmt;
    char *callsFmt;
    char *balanceFmt;
    BOOL mirrored = FALSE;
	switch (g_cfg->FORM)
    {
    case CFG::SR_80:
        {
            rangeFmt =
                "\tExtensiones""\n"
                "\t  %s"       "\n"
                ;
            callsFmt =
                "\tLLamadas"         "\n"
                "\t    Nal.: [%ld] %.2f""\n"
                "\t  Inter.: [%ld] %.2f""\n"
                "\t%s"                  "\n"
                "\t  %.2f"              "\n"
                ;
            balanceFmt =
                "\tBalance"                   "\n"
                "\t        Abonos: %.2f"      "\n"
                "\t    Cuenta cte: %.2f"      "\n"
                "\t   Instalaci�n: %.2f"      "\n"
                "\t         L�nea: %.2f"      "\n"
                "\t  Otros cargos: %.2f"      "\n"
                "\t      Llamadas: [%ld] %.2f""\n"
                "\t    Descuentos: %.2f"      "\n"
                "\t         Saldo: %.2f"      "\n"
                ;
            mirrored = FALSE;
            break;
        }
    case CFG::DR_EME:
    case CFG::DR_HALF:
    case CFG::DR_80:
    case CFG::DR_PRE:
    case CFG::LINEAL_80:
        {
            rangeFmt =
                "\tExtensiones""\tExtensiones""\n"
                "\t  %s"       "\t  %s""\n"
                ;
            callsFmt =
                "\tLLamadas"         "\tLLamadas""\n"
                "\t    Nal.: [%ld] %.2f""\t    Nal.: [%ld] %.2f""\n"
                "\t  Inter.: [%ld] %.2f""\t  Inter.: [%ld] %.2f""\n"
                "\t%s"                  "\t%s""\n"
                "\t  %.2f"              "\t  %.2f""\n"
                ;
            balanceFmt =
                "\tBalance"                   "\tBalance""\n"
                "\t        Abonos: %.2f"      "\t        Abonos: %.2f""\n"
                "\t    Cuenta cte: %.2f"      "\t   Cuenta cte.: %.2f""\n"
                "\t   Instalaci�n: %.2f"      "\t   Instalaci�n: %.2f""\n"
                "\t         L�nea: %.2f"      "\t         L�nea: %.2f""\n"
                "\t  Otros cargos: %.2f"      "\t  Otros cargos: %.2f""\n"
                "\t      Llamadas: [%ld] %.2f""\t      Llamadas: [%ld] %.2f""\n"
                "\t    Descuentos: %.2f"      "\t    Descuentos: %.2f""\n"
                "\t         Saldo: %.2f"      "\t         Saldo: %.2f""\n"
                ;
            mirrored = TRUE;
            break;
        }
    case CFG::DR_40:
    case CFG::SR_40:
        {
            rangeFmt =
                "\tExtensiones""\n"
                "\t  %s""\n"
                ;
            callsFmt =
                "\tLLamadas""\n"
                "\t    Nal: [%ld] %.2f""\n"
                "\t  Inter: [%ld] %.2f""\n"
                "\t%s""\n"
                "\t  %.2f""\n"
                ;
            balanceFmt =
                "\tBalance""\n"
                "\t        Abonos: %.2f""\n"
                "\t    Cuenta cte: %.2f""\n"
                "\t   Instalaci�n: %.2f""\n"
                "\t         L�nea: %.2f""\n"
                "\t  Otros cargos: %.2f""\n"
                "\t      Llamadas: [%ld] %.2f""\n"
                "\t    Descuentos: %.2f""\n"
                "\t         Saldo: %.2f""\n"
                ;
            break;
        }
    case CFG::DR_18:
    case CFG::SR_28:
        return ; // nothing to do
    }


    BYTE channel = 0;
    if (mirrored)
    {
		g_spooler->Printf(channel, rangeFmt,
                         range, range
                        );
		g_spooler->Printf(channel, callsFmt,
                         ddnCalls, ddnCost, ddnCalls, ddnCost,
                         ddiCalls, ddiCost, ddiCalls, ddiCost,
						 g_cfg->TAX_NAME, g_cfg->TAX_NAME,
                         tax, tax
                        );
		g_spooler->Printf(channel, balanceFmt,
                         credits, credits,
                         debits, debits,
                         install, install,
                         line, line,
                         others, others,
                         calls, cost, calls, cost,
                         discount, discount,
                         remainder, remainder
                        );
    }
    else
    {
		g_spooler->Printf(channel, rangeFmt,
                         range
                        );
		g_spooler->Printf(channel, callsFmt,
                         ddnCalls, ddnCost,
                         ddiCalls, ddiCost,
						 g_cfg->TAX_NAME,
                         tax
                        );
		g_spooler->Printf(channel, balanceFmt,
                         credits,
                         debits,
                         install,
                         line,
                         others,
                         calls, cost,
                         discount,
                         remainder
                        );
    }
}

// --------------------------------------------------------------------------
//     E_PARAMETERS
// --------------------------------------------------------------------------

E_PARAMETERS::E_PARAMETERS(void) : UIW_WINDOW("E_PARAMETERS", defaultStorage)
{
	int integer = g_cfg->E_FIRST_EXT;
    UIW_INTEGER *wInt = (UIW_INTEGER *)Get(SID_BOOTH);
    wInt->userFunction = Process;
	if (g_cfg->E_FIRST_EXT)
    {
        integer--;
        wInt->DataSet(&integer);
		((UIW_INTEGER *)Get(SID_FIRST))->DataSet((int *)&g_cfg->E_FIRST_EXT);
		integer = g_cfg->ACTIVE_CLUSTERS*CLUSTER_SIZE;
        ((UIW_INTEGER *)Get(SID_LAST))->DataSet(&integer);
    }
    else
    {
		integer = g_cfg->ACTIVE_CLUSTERS*CLUSTER_SIZE;
        wInt->DataSet(&integer);
    }
	((UIW_BIGNUM *)Get(SID_INSTALL))->DataSet(&UI_BIGNUM(g_cfg->E_INSTALL_COST));
	((UIW_BIGNUM *)Get(SID_LINE))->DataSet(&UI_BIGNUM(g_cfg->E_LINE_COST));
	((UIW_BIGNUM *)Get(SID_DISCOUNT))->DataSet(&UI_BIGNUM(g_cfg->E_DISCOUNT));
	((UIW_BIGNUM *)Get(SID_MIN))->DataSet(&UI_BIGNUM(g_cfg->E_MIN_AVAILABLE));
	if (g_cfg->E_SHOW_PHONE)
        ((UIW_BUTTON *)Get(SID_SHOW))->woStatus |= WOS_SELECTED;
    wnFlags |= WNF_SELECT_MULTIPLE;
    //
    if (!g_extAreChangeable)
    {
        Get(SID_RANGE)->woFlags   |= WOF_NON_SELECTABLE;
        Get(SID_CHARGES)->woFlags |= WOF_NON_SELECTABLE;
        Get(SID_OTHERS)->woFlags  |= WOF_NON_SELECTABLE;
        Get(SID_OK)->woFlags      |= WOF_NON_SELECTABLE;
        ((UIW_BUTTON *)Get(SID_CANCEL))->DataSet((char *)CSTR_CLOSE);
    }
    //
    windowManager->Center(this);
    //	helpContext = H_E_INFO;
}



E_PARAMETERS::~E_PARAMETERS(void)
{}



EVENT_TYPE E_PARAMETERS::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_ACCEPT:
		g_cfg->E_FIRST_EXT = ((UIW_INTEGER *)Get(SID_FIRST))->DataGet();
		((UIW_BIGNUM *)Get(SID_INSTALL))->DataGet()->Export(&g_cfg->E_INSTALL_COST);
		((UIW_BIGNUM *)Get(SID_LINE))->DataGet()->Export(&g_cfg->E_LINE_COST);
		((UIW_BIGNUM *)Get(SID_DISCOUNT))->DataGet()->Export(&g_cfg->E_DISCOUNT);
		((UIW_BIGNUM *)Get(SID_MIN))->DataGet()->Export(&g_cfg->E_MIN_AVAILABLE);
        if (FlagSet(((UIW_BUTTON *)Get(SID_SHOW))->woStatus, WOS_SELECTED))
			g_cfg->E_SHOW_PHONE = 1;
        else
			g_cfg->E_SHOW_PHONE = 0;
		g_cfg->Save();
        //
        eventManager->Put(UI_EVENT(S_CLOSE,0));
        break;
    case UE_CANCEL:
        eventManager->Put(UI_EVENT(S_CLOSE,0));
        break;
    default:
        ccode = UIW_WINDOW::Event(event);
        break;
    }
    return ccode;
}

EVENT_TYPE E_PARAMETERS::Process(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode != S_NON_CURRENT && ccode != L_SELECT)
        return ccode;
    UIW_INTEGER *wFirstExt = (UIW_INTEGER *)object->parent->Get(SID_FIRST);
    UIW_INTEGER *wLastExt  = (UIW_INTEGER *)object->parent->Get(SID_LAST);
    int booth = ((UIW_INTEGER *)object)->DataGet();
	int maxBooth = g_cfg->ACTIVE_CLUSTERS*CLUSTER_SIZE;
    int integer = 0;
    wFirstExt->DataSet(&integer);
    wLastExt->DataSet(&integer);
    if (!(booth > 0 && booth <= maxBooth))
    {
        errorSystem->ReportError(windowManager, WOS_NO_STATUS, "N�mero de cabina entre 1 y %d.", maxBooth);
        ((UIW_INTEGER *)object)->DataSet(&maxBooth);
        return -1;
    }
    integer = booth+1;
    if (booth < maxBooth)
    {
		wFirstExt->DataSet(&integer);
        wLastExt->DataSet(&maxBooth);
    }
    eventManager->Put(UI_EVENT(UE_VIEW,0));
    return ccode;
}

//
// --- Misc. to Print ---
//

extern APP_INFO g_appInfo;

void PrintConfig(void)
{
    BYTE channel = 0;
    char *cfgStr;
	switch (g_cfg->FORM)
	{
	case CFG::DR_EME:
	case CFG::DR_HALF:
	case CFG::DR_PRE:
	case CFG::SR_80    :
	case CFG::DR_80    :
	case CFG::LINEAL_80:
		cfgStr =
			"\x1B\x40"                 // init
			"\x1B\x41\x06"             // line spacing
			"\n\n"
			"\x1B\x40"                 // init
			"\x1B\x43\x20"             // page size
			"\x1B\x21\x00"             // style
			"\x1B\x44\x02\x30\x00" 	   // tabs
			"\n\n\n\n\n"               // spaces
			"\xFF"
		;

		g_spooler->Print(channel, cfgStr, TRUE);
		break;
	case CFG::DR_40    :
	case CFG::SR_40    :
		cfgStr =
            "\x1B\x40"     // init
            "\x1B\x21\x00" // style
            "\n\n"         // spaces
            "\xFF"
            ;
		g_spooler->Print(channel, cfgStr, TRUE);
        break;
    case CFG::DR_18    :
    case CFG::SR_28    :
        break;
    }
}

void PrintHeader(void)
{
    // prepare header
    char *headerFmt;
    BOOL mirrored;
    mirrored = FALSE;
	switch (g_cfg->FORM)
    {
    case CFG::SR_80    :
        headerFmt =
            "\x1B\x21\x21\t%s \x1B\x21\x05%s\n" // empresa, id
            "\t%s <%s> %s %s\n"  // ciudad, serial, fecha, hora
            "\n"
            ;
        mirrored = FALSE;
        break;
    case CFG::DR_EME:
    case CFG::DR_HALF:
	case CFG::LINEAL_80:
    case CFG::DR_80    :
        headerFmt =
            "\x1B\x21\x21\t%s \x1B\x21\x05%s\n" // empresa, id
            "\t%s <%s> %s %s\t%s <%s> %s %s\n"  // ciudad, serial, fecha, hora
            "\n"
            ;
        mirrored = TRUE;
        break;
    case CFG::DR_PRE    :
        {
            headerFmt =
                "\x1B\x21\x21\t%s\x1B\x21\x05\n" // empresa
                "\t%s <%s> %s %s\t%s <%s> %s %s\n"  // ciudad, serial, fecha, hora
                "\n"
                ;
            mirrored = TRUE;
            break;
		}
    case CFG::SR_40    :
    case CFG::DR_40    :
        headerFmt =
            "\x1B\x21\x31%s \x1B\x21\x01%s\n"
            "\x1B\x63\x30\x03\x1B\x7A\x01%s <%s> %s %s\n"
            "\n"
            ;
        break;
    case CFG::DR_18    :
    case CFG::SR_28    :
        return ; // nothing to do
    }
    // prepare time and date


    char dateStr[0x0F];
    _GetSysDate(dateStr);
	char timeStr[0x0F];
    _GetSysTime(timeStr);
    // Print
    BYTE channel = 0;
    if (mirrored)
    {
		g_spooler->Printf
		(
			channel, headerFmt,
			g_cfg->COMPANY, g_cfg->ID,
			g_cfg->CITY, g_appInfo.ShortSerial, dateStr, timeStr,
			g_cfg->CITY, g_appInfo.ShortSerial, dateStr, timeStr
		);
    }
    else
    {
		g_spooler->Printf
		(
			channel, headerFmt,
			g_cfg->COMPANY, g_cfg->ID,
			g_cfg->CITY, g_appInfo.ShortSerial, dateStr, timeStr
		);
    }
}

void PrintFooter(void)
{
    char *footerFmt;
	switch (g_cfg->FORM)
    {
    case CFG::DR_EME:
    case CFG::DR_HALF:
    case CFG::DR_PRE:
    case CFG::LINEAL_80:
    case CFG::SR_80:
    case CFG::DR_80:
		if (strlen(g_cfg->P_FOOTER) == 0)
            return; // avoid bad spaces
        footerFmt =
            "\n"
            "\x1B\x21\x04""\t%s\n"
            ;
        break;
    case CFG::DR_40:
    case CFG::SR_40:
        footerFmt =
			"\n"
            "\x1B\x21\x01\x1B\x63\x30\x02%s\n"
            ;
        break;
    case CFG::DR_18    :
    case CFG::SR_28    :
        return ; // nothing to do
    }

    BYTE channel = 0;
	g_spooler->Printf(channel, footerFmt, g_cfg->P_FOOTER);
}

void PrintFF(void)
{
    char *formFeed;
	switch (g_cfg->FORM)
    {
    case CFG::LINEAL_80:
    case CFG::DR_EME:
    case CFG::DR_HALF:
    case CFG::SR_80:
    case CFG::DR_80:
    case CFG::DR_PRE:
        formFeed = "\x0C";
        break;
    case CFG::DR_40:
    case CFG::SR_40:
        formFeed = "\n\n\n\n\n\n\n\n\n\x1B\x69";
        break;
    case CFG::DR_18:
    case CFG::SR_28:
        return; // nothing to do
    }

    BYTE channel = 0;
    g_spooler->Print(channel, formFeed);
}
