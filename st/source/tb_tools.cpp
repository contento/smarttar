//
// [ TB_TOOL.CPP ]
//

#include "stdst.h"

#include <control.h>
#include <db_eng.h>
#include <toolbar.h>
#include <rt_eng.h>
#include <events.h>
#include <hb_ids.h>
#include <res.hpp>

#ifndef USE_HELP_CONTEXTS
#define USE_HELP_CONTEXTS
#include <help.hpp>
#endif

extern CFG *g_cfg;

// --------------------------------------------------------------------------
//								UIW_SPY
// --------------------------------------------------------------------------

static BOOL __letSpyBoth  = TRUE;
static BOOL __wasInternal = TRUE;

UIW_SPY::UIW_SPY(void) : UIW_WINDOW("W_SPY", defaultStorage)
{
    if (!__letSpyBoth)
    {
        UIW_BUTTON *wButton;
        if (__wasInternal)
            wButton = (UIW_BUTTON *) Get("B_EX_LINE");
        else
            wButton = (UIW_BUTTON *) Get("B_IN_LINE");
        wButton->woFlags |= WOF_NON_SELECTABLE;
    }
    UIW_INTEGER *wInteger = (UIW_INTEGER *) Get("W_BOOTH");
    int integer = 3;
    wInteger->Information(SET_TEXT_LENGTH, &integer);
    windowManager->Center(this);
    helpContext = H_SPY;
}

EVENT_TYPE UIW_SPY::Event(const UI_EVENT &event)
{
	// Switch on the type of event.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case UE_ACCEPT:
		// load number and spy the corresponding booth
		UIW_INTEGER *wBoothNumber = (UIW_INTEGER *) Get("W_BOOTH");
		int boothNum = wBoothNumber->DataGet();

		if (boothNum < 0 || boothNum > (g_cfg->ACTIVE_CLUSTERS*CLUSTER_SIZE))
		{
			errorSystem->ReportError
			(
				windowManager,	WOS_NO_STATUS, "Número de cabina incorrecto."
			);
			break;
		}

		--boothNum;
		// send a message into the queue directed to controller
		UI_EVENT tmpEvent;
		tmpEvent.type = E_CONTROLLER;
		tmpEvent.rawCode = UE_SPY;
		// use the region left to put booth number
		tmpEvent.region.left = boothNum;
		UIW_BUTTON *wButton = (UIW_BUTTON *) Get("B_IN_LINE");
		// use the region right to put kind of spy (TRUE:internal, FALSE:external)
		tmpEvent.region.right  = (FlagSet(wButton->woStatus, WOS_SELECTED))?TRUE:FALSE;
		// because internal and external are exclusive
		__wasInternal = tmpEvent.region.right;
		__letSpyBoth = (tmpEvent.region.left == -1)?TRUE:FALSE;
		eventManager->Put(UI_EVENT(S_CLOSE,0));
		eventManager->Event(tmpEvent, E_CONTROLLER);
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
//								UIW_INTER
// --------------------------------------------------------------------------

UIW_INTER::UIW_INTER(void) : UIW_WINDOW("W_INTER3", defaultStorage)
{
    UIW_INTEGER *wInteger = (UIW_INTEGER *) Get("W_BOOTH");
    int integer = 3;
    wInteger->Information(SET_TEXT_LENGTH, &integer);
    UIW_BIGNUM  *wBignum = (UIW_BIGNUM *)  Get("W_VALUE");
    integer = 9;
    wBignum->Information(SET_TEXT_LENGTH, &integer);
    // fill table
    char strId[10];
    int cNum, bNum;
	for (int i = 0; i < g_cfg->ACTIVE_CLUSTERS*CLUSTER_SIZE; i++)
    {
		wBignum = (UIW_BIGNUM *)  Get(itoa(i+1, strId, 10));
        cNum = i/CLUSTER_SIZE;
        bNum = i%CLUSTER_SIZE;
        wBignum->DataSet(&UI_BIGNUM(CONTROLLER::RTEngineGetPreValue(cNum, bNum)));
    }
	helpContext = H_INTER;
    windowManager->Center(this);
}

EVENT_TYPE UIW_INTER::Event(const UI_EVENT &event)
{
	// Switch on the type of event.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case UE_ACCEPT:
		{
			UIW_INTEGER *wInteger = (UIW_INTEGER *) Get("W_BOOTH");
			UIW_BIGNUM  *wBignum = (UIW_BIGNUM *)  Get("W_VALUE");
			int boothCount = wInteger->DataGet();
			if (boothCount <= 0 || boothCount > (g_cfg->ACTIVE_CLUSTERS*CLUSTER_SIZE))
			{
				errorSystem->ReportError
				(
					windowManager,	WOS_NO_STATUS, "Número de cabina incorrecto."
				);
				break;
			}

			--boothCount;
			int cNum = boothCount/CLUSTER_SIZE;
			int bNum = boothCount%CLUSTER_SIZE;
			// don't let change PreValue if there are calls. v.219
			if
			(
				CONTROLLER::RTEngineGetNumOfCalls(cNum, bNum) ||
				CONTROLLER::RTEngineIsBoothBusy  (cNum, bNum)
			)
			{
				errorSystem->ReportError
				(
					windowManager,	WOS_NO_STATUS, "La cabina tiene llamadas en curso."
				);
				break;
			}

			double value;
			wBignum->DataGet()->Export(&value);
			CONTROLLER::RTEngineSetPreValue     (cNum, bNum, value);
			CONTROLLER::RTEngineSetPrePaid      (cNum, bNum, (value == 0.0F)?FALSE:TRUE);
			CONTROLLER::RTEngineSetFirstPreValue(cNum, bNum, TRUE);
			// send a message into the queue directed to controller
			UI_EVENT tmpEvent;
			tmpEvent.type = E_CONTROLLER;
			// use the raw code to put service
			tmpEvent.rawCode = UE_INTER;
			// use modifiers to put booth number
			tmpEvent.modifiers = boothCount;
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

// --------------------------------------------------------------------------
//								UIW_LOCK
// --------------------------------------------------------------------------

UIW_LOCK::UIW_LOCK(void) : UIW_WINDOW("W_LOCK", defaultStorage)
{
    UIW_INTEGER *wInteger = (UIW_INTEGER *) Get("W_BOOTH");
	int integer = 3;
	wInteger->Information(SET_TEXT_LENGTH, &integer);
    windowManager->Center(this);
    helpContext = H_LOCK;
}

EVENT_TYPE UIW_LOCK::Event(const UI_EVENT &event)
{
    // Switch on the type of event.
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_ACCEPT:
        // load number and lock the corresponding booth
        UIW_INTEGER *wBoothNumber = (UIW_INTEGER *) Get("W_BOOTH");
		int boothNum = wBoothNumber->DataGet();

		if (boothNum < 0 || boothNum > (g_cfg->ACTIVE_CLUSTERS*CLUSTER_SIZE))
		{
			errorSystem->ReportError
			(
				windowManager,	WOS_NO_STATUS, "Número de cabina incorrecto."
			);
			break;
		}

		--boothNum;
		// send a message into the queue directed to controller
		UI_EVENT tmpEvent;
		tmpEvent.type = E_CONTROLLER;
		// check to see if it is lock or unlock
		UIW_BUTTON *wButton = (UIW_BUTTON *) Get("B_LOCK");
		// use the region right to put kind of service
		tmpEvent.rawCode = (FlagSet(wButton->woStatus, WOS_SELECTED))?UE_LOCK:UE_UNLOCK;
		// use the region left to put booth number
		tmpEvent.region.left = boothNum;
		eventManager->Put(UI_EVENT(S_CLOSE,0));
		eventManager->Event(tmpEvent, E_CONTROLLER);
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

