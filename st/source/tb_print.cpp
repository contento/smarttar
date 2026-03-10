//
// [ TB_PRINT.CPP ]
//

#include "stdst.h"

#include <db_eng.h>
#include <toolbar.h>
#include <events.h>
#include <hb_ids.h>

#include <cstr.h>
#include <plain_pr.h>

#include <res.hpp>

#ifndef USE_HELP_CONTEXTS
#define USE_HELP_CONTEXTS
#include <help.hpp>
#endif

extern CFG        	*g_cfg;
extern DB_ENGINE	*g_dbEngine;

// --------------------------------------------------------------------------
//								UIW_RECEIPT
// --------------------------------------------------------------------------

UIW_RECEIPT::UIW_RECEIPT(void) : UIW_WINDOW("W_RECEIPT", defaultStorage)
{
    // get the address of the window used for displaying each kind of receipt ...
    WBooth  = (UIW_INTEGER *) Get("W_BOOTH");
    WNumber = (UIW_BIGNUM  *) Get("W_NUMBER");
    WList   = (UIW_VT_LIST *) Get("W_LIST");
    WPay    = (UIW_BUTTON  *) Get("W_PAY");
    WBooth->userFunction  = processBooth;
    WNumber->userFunction = processNumber;
    int integer = 3;
    WBooth->Information(SET_TEXT_LENGTH, &integer);
    integer = 7;
    WNumber->Information(SET_TEXT_LENGTH, &integer);
    //
    windowManager->Center(this);
    eventManager->Put(UI_EVENT(UE_RNPNP,0)); // force the first event ...
    helpContext = H_RECEIPT;
}

long UIW_RECEIPT::Number = 0;
int  UIW_RECEIPT::Booth  = 0;

EVENT_TYPE UIW_RECEIPT::processNumber(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
    if (ccode != L_SELECT)
        return ccode;
    if (((UIW_BIGNUM *)(object))->Validate(FALSE) != NMI_OK)
        return ccode;
    UI_EVENT tmpEvent;
    tmpEvent.type    = E_CONTROLLER;
    tmpEvent.rawCode = UE_NUMBER_FROM_UIW_RECEIPT;
    ((UIW_BIGNUM *)(object))->DataGet()->Export(&Number);
    tmpEvent.data    = object->parent; // send the parent
    eventManager->Event(tmpEvent, E_CONTROLLER);
    //
    return ccode;
}

EVENT_TYPE UIW_RECEIPT::processBooth(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
    if (ccode != L_SELECT)
        return ccode;
    if (((UIW_INTEGER *)(object))->Validate(FALSE) != NMI_OK)
        return ccode;
    UI_EVENT tmpEvent;
    tmpEvent.type    = E_CONTROLLER;
    tmpEvent.rawCode = UE_BOOTH_FROM_UIW_RECEIPT;
    tmpEvent.data    = object->parent; // send the parent
    Booth = ((UIW_INTEGER *)(object))->DataGet() - 1;
    eventManager->Event(tmpEvent, E_CONTROLLER);
    //
    return ccode;
}

EVENT_TYPE UIW_RECEIPT::Event(const UI_EVENT &event)
{
    // Switch on the type of event.
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_RNPNP:
        WPay->woFlags    &= ~WOF_NON_SELECTABLE;
        WBooth->woFlags  &= ~WOF_NON_SELECTABLE;
        WNumber->woFlags |= WOF_NON_SELECTABLE;
        Option = ccode;
        WList->Destroy();
        WList->Event(UI_EVENT(S_REDISPLAY, 0));
        break;
    case UE_RNPP:
        WPay->woFlags    |= WOF_NON_SELECTABLE;
        WBooth->woFlags  &= ~WOF_NON_SELECTABLE;
        WNumber->woFlags |= WOF_NON_SELECTABLE;
        Option = ccode;
        WList->Destroy();
        WList->Event(UI_EVENT(S_REDISPLAY, 0));
        break;
    case UE_RP:
        WPay->woFlags    |= WOF_NON_SELECTABLE;
        WBooth->woFlags  |= WOF_NON_SELECTABLE;
        WNumber->woFlags &= ~WOF_NON_SELECTABLE;
        Option = ccode;
        WList->Destroy();
        WList->Event(UI_EVENT(S_REDISPLAY, 0));
        break;
    case UE_SELECT_ALL:
        {
            UI_WINDOW_OBJECT *wStr = WList->First();
            while (wStr)
            {
                wStr->woStatus |= WOS_SELECTED;
                wStr->Event(UI_EVENT(S_REDISPLAY, 0));
                wStr = wStr->Next();
            }
            break;
        }
    case UE_PRINT_ALL:
        {
			*windowManager
				+ new PLAIN_PRINTOUT
				(
					g_dbEngine->GetFirstNumber(),
					g_dbEngine->GetEntries()
				);
            break;
        }
    case UE_PRINT:
        UI_EVENT tmpEvent;
        tmpEvent.type    = E_CONTROLLER;
        tmpEvent.rawCode = UE_PRINT_FROM_UIW_RECEIPT;
        tmpEvent.data    = this;
        eventManager->Event(tmpEvent, E_CONTROLLER);
        break;
    case UE_PAY:
        tmpEvent.type    = E_CONTROLLER;
        tmpEvent.rawCode = UE_PAY_FROM_UIW_RECEIPT;
        tmpEvent.data    = this;
        eventManager->Event(tmpEvent, E_CONTROLLER);
        break;
    case UE_CANCEL:
        woAdvancedFlags &= ~WOAF_LOCKED; // disconnect lock !!!
        eventManager->Put(UI_EVENT(S_CLOSE,0));
        break;
    default:
        ccode = UIW_WINDOW::Event(event);
        break;
    }
    return ccode;
}

// --------------------------------------------------------------------------
//								UIW_ACCUM
// --------------------------------------------------------------------------

static BOOL fromDBView = FALSE;
static BOOL fromTurn   = FALSE;

UIW_ACCUM::UIW_ACCUM(BOOL fromDBView, BOOL fromTurn):
        UIW_WINDOW("W_ACCUM", defaultStorage)
{
    ::fromDBView = fromDBView;
    ::fromTurn   = fromTurn;
    if (fromDBView)
    {
        // deactivate some members to avoid confussion
        Get("W_OP")->woFlags |= WOF_NON_SELECTABLE;
        Get("W_ARC")->woFlags |= WOF_NON_SELECTABLE;
    }
    // use userFlags to communicate messages
    userFlags = fromTurn;
    // change Tax prompts
	((UIW_STRING *)Get("W_P_TAX1"))->DataSet(g_cfg->TAX_NAME);
	((UIW_STRING *)Get("W_P_TAX2"))->DataSet(g_cfg->TAX_NAME);
    ((UIW_STRING *)Get("W_P_TAX3"))->DataSet(g_cfg->TAX_NAME);
    windowManager->Center(this);
    helpContext = H_ACCUM;
}

EVENT_TYPE UIW_ACCUM::Event(const UI_EVENT &event)
{
    // Switch on the type of event.
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case S_CREATE:
        {
            UIW_WINDOW::Event(event);
            // send a message into the queue directed to controller
            UI_EVENT tmpEvent;
            tmpEvent.type = E_CONTROLLER;
            // use the raw code to put service
            tmpEvent.rawCode   = UE_VIEW_FROM_UIW_ACCUM;
            tmpEvent.data      = this;
            tmpEvent.modifiers = DB_STATISTICS::TURN;
            eventManager->Event(tmpEvent, E_CONTROLLER);
            break;
        }
    case UE_PRINT:
        {
            UIW_BUTTON  *wButton = (UIW_BUTTON *) this ->Get("W_NO_DEL");
            if (!FlagSet(wButton->woStatus, WOS_SELECTED))
            {
                if (errorSystem->ReportError(windowManager, WOS_INVALID, (char *)CSTR_CRITICAL) != WOS_INVALID)
                    break;
            }
            // send a message into the queue directed to controller
            UI_EVENT tmpEvent;
            tmpEvent.type = E_CONTROLLER;
            // use the raw code to put service
            tmpEvent.rawCode   = UE_PRINT_FROM_UIW_ACCUM;
            tmpEvent.data      = this;
            tmpEvent.modifiers = TRUE; // delete accum ?
            if (FlagSet(wButton->woStatus, WOS_SELECTED))
                tmpEvent.modifiers = FALSE;
            eventManager->Event(tmpEvent, E_CONTROLLER);
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