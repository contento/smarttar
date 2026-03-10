//
// [ MB_PRINT.CPP ]
//

#include "stdst.h"

#include <strstream.h>
#include <classlib\strng.h>

#include <ph_eng.h>
#include <db_eng.h>
#include <spooler.h>
#include <menubar.h>
#include <events.h>

#ifndef USE_HELP_CONTEXTS
#define USE_HELP_CONTEXTS
#include <help.hpp>
#endif

extern CFG        	*g_cfg;
extern DB_ENGINE 	*g_dbEngine;
extern PH_ENGINE 	*g_phEngine;
extern SPOOLER    	*g_spooler;

// --------------------------------------------------------------------------
//								UIW_SACCUM
// --------------------------------------------------------------------------

static BOOL fromDBView = FALSE;
static BOOL fromTurn   = FALSE;

UIW_SACCUM::UIW_SACCUM(BOOL fromDBView, BOOL fromTurn):
        UIW_WINDOW("W_SACCUM", defaultStorage)
{
    ::fromDBView = fromDBView;
    ::fromTurn   = fromTurn;
    // use userFlags to communicate messages
    userFlags = fromTurn;
    // change Tax prompts
	((UIW_STRING *)Get("W_P_TAX1"))->DataSet(g_cfg->TAX_NAME);
	((UIW_STRING *)Get("W_P_TAX2"))->DataSet(g_cfg->TAX_NAME);
	((UIW_STRING *)Get("W_P_TAX3"))->DataSet(g_cfg->TAX_NAME);
    windowManager->Center(this);
    eventManager->Put(UI_EVENT(UE_VIEW, 0)); // force the first event ...
    helpContext = H_SACCUM;
}

EVENT_TYPE UIW_SACCUM::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_VIEW:
        {
            UIW_BUTTON  *wButton;
            // send a message into the queue directed to controller
            UI_EVENT tmpEvent;
            tmpEvent.type = E_CONTROLLER;
            // use the raw code to put service
            tmpEvent.rawCode = UE_VIEW_FROM_UIW_SACCUM;
            tmpEvent.data    = this;
            wButton = (UIW_BUTTON *) Get("W_AD");
            if (FlagSet(wButton->woStatus, WOS_SELECTED))
                tmpEvent.modifiers = DB_STATISTICS::DAY;
            else
            {
                wButton = (UIW_BUTTON *) Get("W_AW");
                if (FlagSet(wButton->woStatus, WOS_SELECTED))
                    tmpEvent.modifiers = DB_STATISTICS::WEEK;
                else
                {
                    wButton = (UIW_BUTTON *) Get("W_AM");
                    if (FlagSet(wButton->woStatus, WOS_SELECTED))
                        tmpEvent.modifiers = DB_STATISTICS::MONTH;
                    else
                        tmpEvent.modifiers = DB_STATISTICS::YEAR;
                }
            }
            eventManager->Event(tmpEvent, E_CONTROLLER);
            break;
        }
    case UE_PRINT:
        {
            UIW_BUTTON  *wButton;
            // send a message into the queue directed to controller
            UI_EVENT tmpEvent;
            tmpEvent.type = E_CONTROLLER;
            // use the raw code to put service
            tmpEvent.rawCode = UE_PRINT_FROM_UIW_SACCUM;
            tmpEvent.data    = this;
            wButton = (UIW_BUTTON *) Get("W_AD");
            if (FlagSet(wButton->woStatus, WOS_SELECTED))
                tmpEvent.modifiers = DB_STATISTICS::DAY;
            else
            {
                wButton = (UIW_BUTTON *) Get("W_AW");
                if (FlagSet(wButton->woStatus, WOS_SELECTED))
                    tmpEvent.modifiers = DB_STATISTICS::WEEK;
                else
                {
                    wButton = (UIW_BUTTON *) Get("W_AM");
                    if (FlagSet(wButton->woStatus, WOS_SELECTED))
                        tmpEvent.modifiers = DB_STATISTICS::MONTH;
                    else
                        tmpEvent.modifiers = DB_STATISTICS::YEAR;
                }
            }
            eventManager->Event(tmpEvent, E_CONTROLLER);
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

// --------------------------------------------------------------------------
//								UIW_ZPRINT
// --------------------------------------------------------------------------

static int CompareWString(void *leftWString, void *rightWString)
{
    return (strcmp(((UIW_STRING *)leftWString)->DataGet(), ((UIW_STRING *)rightWString)->DataGet()));
}

UIW_ZPRINT::UIW_ZPRINT(void) : UIW_WINDOW("W_ZPRINT", defaultStorage)
{
    UIW_VT_LIST *wList = (UIW_VT_LIST *) Get("W_LIST");
    wList->compareFunction = CompareWString;
    UIW_INTEGER *wInteger = (UIW_INTEGER *) Get("W_AREA");
    wInteger->userFunction = processArea;
    windowManager->Center(this);
    this->helpContext = H_ZPRINT;
}

EVENT_TYPE UIW_ZPRINT::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_VIEW:
        {
            UIW_VT_LIST *wList = (UIW_VT_LIST *) Get("W_LIST");
            wList->Destroy();
            int entryNum = 0;
            UIW_BUTTON *wButton = (UIW_BUTTON *) Get("W_LOCAL");
            if (FlagSet(wButton->woStatus, WOS_SELECTED))
            {
                for (int slot = 0; slot < MAX_INFO_SLOTS; slot++)
                {
					PH_ENGINE::PLACE_ENTRY_LIST_ITERATOR iterator(g_phEngine->GetPlaceList(PH_ENGINE::LOCAL_SOURCE, slot));
                    while (iterator)
                    {
                        ostrstream s;
						g_phEngine->ToInfLine(s, iterator) << '\0';
                        *wList
                        + new UIW_STRING(0, entryNum,  80, s.str(), 80, STF_NO_FLAGS, WOF_VIEW_ONLY);
                        iterator++;
                        entryNum++;
                    }
                }
            }
            else
            {
                UIW_INTEGER *wInteger = (UIW_INTEGER *) Get("W_AREA");
                int slot = wInteger->DataGet();
				PH_ENGINE::PLACE_ENTRY_LIST_ITERATOR iterator(g_phEngine->GetPlaceList(PH_ENGINE::DDN_SOURCE, slot));
                while (iterator)
                {
                    ostrstream s;
					g_phEngine->ToInfLine(s, iterator) << '\0';
                    *wList
                    + new UIW_STRING(0, entryNum,  80, s.str(), 80, STF_NO_FLAGS, WOF_VIEW_ONLY);
                    iterator++;
                    entryNum++;
                }
            }
            wList->Event(UI_EVENT(S_REDISPLAY, 0));
            break;
        }
    case UE_CONTINUE:
        { // to select alpha or numberic sort
            UIW_VT_LIST *wList = (UIW_VT_LIST *) Get("W_LIST");
            UIW_BUTTON *wButton = (UIW_BUTTON *) Get("W_ALPHA");
            wList->compareFunction = (FlagSet(wButton->woStatus, WOS_SELECTED))?CompareWString:NULL;
            eventManager->Put(UI_EVENT(UE_VIEW,0));
            break;
        }
    case UE_PRINT:
        {
            BYTE channel = 0;
            UIW_BUTTON *wButton = (UIW_BUTTON *) Get("W_LOCAL");
            if (FlagSet(wButton->woStatus, WOS_SELECTED))
            {
				g_spooler->Print(channel, "\n\n[ Abreviadas ]\n\n");
            }
            else
            {
                UIW_INTEGER *wInteger = (UIW_INTEGER *) Get("W_AREA");
				g_spooler->Printf(channel, "\n\n[ Area %d ]\n\n", wInteger->DataGet());
            }
            *windowManager
            + new UIW_PRINT_AREA((UIW_VT_LIST *)Get("W_LIST"))
            ;
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

EVENT_TYPE UIW_ZPRINT::processArea(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
    if (ccode != S_NON_CURRENT && ccode != L_SELECT)
        return ccode;
    WORD integer = ((UIW_INTEGER *)object)->DataGet();
    if (!(integer > 0 && integer < 10))
    {
        errorSystem->ReportError(windowManager, WOS_NO_STATUS, "Zona entre 1 y 9.");
        integer = 1;
        ((UIW_INTEGER *)object)->DataSet((int *)&integer);
        return -1;
    }
    eventManager->Put(UI_EVENT(UE_VIEW,0));
    return ccode;
}

// --------------------------------------------------------------------------
//								UIW_IPRINT
// --------------------------------------------------------------------------

UIW_IPRINT::UIW_IPRINT(void) : UIW_WINDOW("W_IPRINT", defaultStorage)
{
    UIW_INTEGER *wInteger = (UIW_INTEGER *) Get("W_AREA");
    UIW_VT_LIST *wList = (UIW_VT_LIST *) Get("W_LIST");
    wList->compareFunction = CompareWString;
    wInteger->userFunction = processArea;
    windowManager->Center(this);
    this->helpContext = H_IPRINT;
}

EVENT_TYPE UIW_IPRINT::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_VIEW:
        {
            UIW_VT_LIST *wList = (UIW_VT_LIST *) Get("W_LIST");
            wList->Destroy();
            int entryNum = 0;
            UIW_INTEGER *wInteger = (UIW_INTEGER *) Get("W_AREA");
            int slot = wInteger->DataGet();
			PH_ENGINE::PLACE_ENTRY_LIST_ITERATOR iterator(g_phEngine->GetPlaceList(PH_ENGINE::DDI_SOURCE, slot));
            while (iterator)
            {
                ostrstream s;
                g_phEngine->ToInfLine(s, iterator) << '\0';
                *wList
                + new UIW_STRING(0, entryNum,  80, s.str(), 80, STF_NO_FLAGS, WOF_VIEW_ONLY);
                iterator++;
                entryNum++;
            }
            wList->Event(UI_EVENT(S_REDISPLAY, 0));
            break;
        }
    case UE_CONTINUE:
        { // to select alpha or numberic sort
            UIW_VT_LIST *wList = (UIW_VT_LIST *) Get("W_LIST");
            UIW_BUTTON *wButton = (UIW_BUTTON *) Get("W_ALPHA");
            wList->compareFunction = (FlagSet(wButton->woStatus, WOS_SELECTED))?CompareWString:NULL;
            eventManager->Put(UI_EVENT(UE_VIEW,0));
            break;
        }
    case UE_PRINT:
        {
            BYTE channel = 0;
            UIW_INTEGER *wInteger = (UIW_INTEGER *) Get("W_AREA");
			g_spooler->Printf(channel, "\n\n[ Area %d ]\n\n", wInteger->DataGet());
            *windowManager
            + new UIW_PRINT_AREA((UIW_VT_LIST *)Get("W_LIST"))
            ;
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

EVENT_TYPE UIW_IPRINT::processArea(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
    if (ccode != S_NON_CURRENT && ccode != L_SELECT)
        return ccode;
    WORD integer = ((UIW_INTEGER *)object)->DataGet();
    if (!(integer > 0 && integer < 10))
    {
        errorSystem->ReportError(windowManager, WOS_NO_STATUS, "Zona entre 1 y 9.");
        integer = 0;
        ((UIW_INTEGER *)object)->DataSet((int *)&integer);
        return -1;
    }
    eventManager->Put(UI_EVENT(UE_VIEW,0));
    return ccode;
}

// --------------------------------------------------------------------------
//								UIW_PRINT_AREA
// --------------------------------------------------------------------------

UIW_PRINT_AREA::UIW_PRINT_AREA(UIW_VT_LIST *wList) : UIW_WINDOW("W_LEVEL", defaultStorage),
        WList(wList)
{
    WStr = wList->First();
    UIW_PROMPT *wPrompt = (UIW_PROMPT *) Get("W_MSG");
    wPrompt->DataSet("Imprimiendo ...");
    eventManager->Put(UI_EVENT(UE_VIEW,0));
    windowManager->Center(this);
}

EVENT_TYPE UIW_PRINT_AREA::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_CANCEL:
        woAdvancedFlags &= ~WOAF_LOCKED; // disconnect lock !!!
        eventManager->Put(UI_EVENT(S_CLOSE,0));
        break;
    case UE_VIEW:
        {
            BYTE channel = 0;
            if (WStr)
            {
                // try to Print
				if (g_spooler->Print(channel, ((UIW_STRING *)WStr)->DataGet()))
                {
					g_spooler->Print(channel, '\n');
                    WStr = WStr->Next();
                    UIW_GROUP *wGroup = (UIW_GROUP *) Get("W_REF");
                    wGroup->Destroy();
                    WORD percent = ((WList->Index(WStr)+1)*100)/WList->Count();
                    char msg[0x0A];
                    itoa(percent, msg, 10);
                    strcat(msg, " %");
                    UIW_BUTTON *wButton = new UIW_BUTTON(0, 1, (percent/10)*5, msg);
                    *wGroup + wButton;
                    wButton->Event(UI_EVENT(S_REDISPLAY, 0));
                }
                eventManager->Put(UI_EVENT(UE_VIEW,0), Q_END); // feedback, no contention !!!
            }
			else
			{
				g_spooler->Print(channel, "\n\n\n");
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

// --------------------------------------------------------------------------
//								UIW_FOOTER
// --------------------------------------------------------------------------

UIW_FOOTER::UIW_FOOTER(void) : UIW_WINDOW("W_FOOTER", defaultStorage)
{
    UIW_STRING *wString1, *wString2;
    wString1 = (UIW_STRING *) Get("W_MSG1");
    wString2 = (UIW_STRING *) Get("W_MSG2");
	wString1->DataSet(g_cfg->P_FOOTER1);
	wString2->DataSet(g_cfg->P_FOOTER2);
    //
    windowManager->Center(this);
    this->helpContext = H_FOOTER;
}

EVENT_TYPE UIW_FOOTER::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_ACCEPT:
        UIW_STRING *wString1, *wString2;
        wString1 = (UIW_STRING *) Get("W_MSG1");
        wString2 = (UIW_STRING *) Get("W_MSG2");
		strcpy(g_cfg->P_FOOTER1, wString1->DataGet());
		strcpy(g_cfg->P_FOOTER2, wString2->DataGet());
		g_cfg->AdjustFooter();
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

// --------------------------------------------------------------------------
//								UIW_ADM_REC
// --------------------------------------------------------------------------

UIW_ADM_REC::UIW_ADM_REC(void) : UIW_WINDOW("W_ADM_REC", defaultStorage)
{
    UIW_BIGNUM *wBignum = (UIW_BIGNUM *) Get("W_REC");
    WORD integer = 9;
    wBignum->Information(SET_TEXT_LENGTH, &integer);
    windowManager->Center(this);
    this->helpContext = H_ADM_REC;
}

EVENT_TYPE UIW_ADM_REC::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_ACCEPT:
        // send a message into the queue directed to controller
        UI_EVENT tmpEvent;
        tmpEvent.type = E_CONTROLLER;
        // use the raw code to put service
        tmpEvent.rawCode = UE_ADM_REC;
        // use modifiers to put flags
        UIW_BUTTON *wButton = (UIW_BUTTON *) Get("W_PAY");
        if (FlagSet(wButton->woStatus, WOS_SELECTED))
        {
            tmpEvent.modifiers = PAID_CALL;
        }
        else
        {
            wButton = (UIW_BUTTON *) Get("W_NO_PAY");
            if (FlagSet(wButton->woStatus, WOS_SELECTED))
                tmpEvent.modifiers = NOT_PAID_CALL;
            else
                tmpEvent.modifiers = TOLL_FREE_CALL;
        }
        // use the data to put the rec number
        UIW_BIGNUM *wBignum = (UIW_BIGNUM *) Get("W_REC");
        static LONG receiptNum;
        wBignum->DataGet()->Export(&receiptNum);
        tmpEvent.data = &receiptNum;
        eventManager->Event(tmpEvent, E_CONTROLLER);
        wBignum->DataSet(&(UI_BIGNUM(0.0)));
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