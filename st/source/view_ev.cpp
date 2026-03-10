//
// [ VIEW_EV.CPP ]
//

#include "stdst.h"

#include <view.h>
#include <menubar.h>
#include <toolbar.h>
#include <db_view.h>
#include <calendar.h>
#include <hb_ids.h>
#include <events.h>
#include <ph_query.h>

#ifndef USE_HELP_CONTEXTS
#define USE_HELP_CONTEXTS
#include <help.hpp>
#endif

extern CFG 	*g_cfg;

EVENT_TYPE UIW_VIEW::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;

	// 2.21.1 build 6
	if (ccode == E_KEY)
	{
		static struct ACCELERATOR_PAIR
		{
			RAW_CODE 		rawCode;
			LOGICAL_EVENT 	logicalType;
		} acceleratorTable[] =
		{
			{ SHIFT_F1, UE_ABOUT },
			{ CTRL_F1 , UE_MD },
			{ F2, UE_SIMULA },
			{ F6, UE_ADM_REC },
			{ F7, UE_SPY },
			{ F8, UE_INTER },
			{ F9, UE_VIEW_TURN},
			{ F10, UE_PHQUERY }, // 2.50
			//
			{ 0, 0 }
		};
		for (int i = 0; acceleratorTable[i].rawCode; i++)
		{
			if (event.rawCode == acceleratorTable[i].rawCode)
			{
				UI_EVENT tmpEvent(acceleratorTable[i].logicalType);
				eventManager->Put(tmpEvent);

				return ccode;
			}
		}
	}

    switch (ccode)
    {
        // ---- Toolbar ...
    case UE_SP_SERV:
        (void *)&(*windowManager + new UIW_SP_SERV);
        break;
    case UE_CALC:
        (void *)&(*windowManager + new UIW_CALC);
        break;
    case UE_SPY:
        (void *)&(*windowManager + new UIW_SPY);
        break;
	case UE_INTER:
		(void *)&(*windowManager + new UIW_INTER);
		break;
	case UE_LOCK:
		(void *)&(*windowManager + new UIW_LOCK);
        break;
    case UE_RECEIPT:
        (void *)&(*windowManager + new UIW_RECEIPT);
        break;
    case UE_ACCUM:
        (void *)&(*windowManager + new UIW_ACCUM);
        break;
    case UE_OPERATION:
        (void *)&(*windowManager + new UIW_OPERATION);
        break;
    case UE_FORMS:
        (void *)&(*windowManager + new UIW_FORMS);
        break;
    case UE_MD:
        STR16 str;
		sprintf(str, "W_MD_%02d", g_cfg->DEALER);
        UIW_WINDOW *WMD = new UIW_WINDOW(str, defaultStorage);
        WMD->helpContext = H_MD;
        windowManager->Center(WMD);
		*windowManager + WMD;
        break;
        // ---- Menu ...
    case UE_FLUSH_ALL:
    case UE_REBUILD_ALL:
        {
            // to the DControl
            UI_EVENT tmpEvent;
            tmpEvent.type = E_CONTROLLER;
            // use the raw code to put service
            tmpEvent.rawCode = ccode;
            eventManager->Event(tmpEvent, E_CONTROLLER);
            break;
        }
    case UE_TIME_DATE:
        (void *)&(*windowManager + new UIW_TIME_DATE);
        break;
    case UE_SDAYS:
        (void *)&(*windowManager + new CALENDAR(1, 1, 71, 10));
        break;
	case UE_SIGNAL:
        (void *)&(*windowManager + new UIW_SIGNAL);
        break;
    case UE_TIMES:
        (void *)&(*windowManager + new UIW_TIMES);
        break;
    case UE_LOCK_NUM:
        (void *)&(*windowManager + new UIW_LOCK_NUM);
        break;
    case UE_PPORT:
        (void *)&(*windowManager + new UIW_PPORT);
        break;
    case UE_NAL_TAR:
        (void *)&(*windowManager + new UIW_NAL_TAR);
        break;
    case UE_INTER_TAR:
        (void *)&(*windowManager + new UIW_INTER_TAR);
        break;
	case UE_PHQUERY:
		(void *)&(*windowManager + new PhoneQueryWindow);
		break;
	case UE_NEW_CITY:
        (void *)&(*windowManager + new UIW_NEW_CITY);
        break;
    case UE_NEW_COUNTRY:
        (void *)&(*windowManager + new UIW_NEW_COUNTRY);
        break;
    case UE_CASH:
        (void *)&(*windowManager + new UIW_CASH);
        break;
    case UE_ALIAS:
        (void *)&(*windowManager + new UIW_ALIAS(TRUE));
        break;
    case UE_ROUND:
        (void *)&(*windowManager + new UIW_ROUND);
        break;
    case UE_DISPLAY:
        (void *)&(*windowManager + new UIW_DISPLAY);
        break;
    case UE_CHANGE_PASSWD:
        (void *)&(*windowManager + new UIW_CHANGE_PASSWD);
        break;
	case UE_SIMULA:
		if (!FlagSet(WConfigMenu->woFlags, WOF_NON_SELECTABLE))
			(void *)&(*windowManager + new UIW_SIMULA);
        break;
    case UE_DEACTIVATE_CONFIG:
        WMenu->Event(UI_EVENT(L_RIGHT, 0));
        WConfigMenu->woFlags |= WOF_NON_SELECTABLE;
        WActivateConfig->woFlags  &= ~WOF_NON_SELECTABLE;
        WConfigMenu->Event(UI_EVENT(S_REDISPLAY, 0));
        break;
    case UE_SYS_INFO:
        (void *)&(*windowManager + new UIW_SYS_INFO);
        break;
    case UE_OP_ID:
        (void *)&(*windowManager + new UIW_OP_ID);
        break;
    case UE_ALARM:
        (void *)&(*windowManager + new UIW_ALARM);
        break;
    case UE_I_NAL_TAR:
        (void *)&(*windowManager + new UIW_NAL_TAR(FALSE));
        break;
    case UE_I_INTER_TAR:
        (void *)&(*windowManager + new UIW_INTER_TAR(FALSE));
        break;
    case UE_I_ALIAS:
        (void *)&(*windowManager + new UIW_ALIAS(FALSE));
        break;
    case UE_ACTIVATE_CONFIG:
        (void *)&(*windowManager + new UIW_PASSWORD(TRUE));
        break;
    case UE_ZPRINT:
        (void *)&(*windowManager + new UIW_ZPRINT);
        break;
    case UE_IPRINT:
        (void *)&(*windowManager + new UIW_IPRINT);
        break;
    case UE_SACCUM:
        (void *)&(*windowManager + new UIW_SACCUM);
        break;
    case UE_FOOTER:
        (void *)&(*windowManager + new UIW_FOOTER);
        break;
    case UE_ADM_REC:
        (void *)&(*windowManager + new UIW_ADM_REC);
        break;
    case UE_PRG_MNG:
        windowManager->helpSystem->DisplayHelp(windowManager, H_GENERAL);
        break;
    case UE_ABOUT:
        (void *)&(*windowManager + new UIW_ABOUT);
        break;
    case UE_VIEW_TURN:
		(void *)&(*windowManager + new DBView(TRUE));
		break;
	case UE_VIEW_OTHER_TURN:
		(void *)&(*windowManager + new DBView(FALSE));
        break;
        // Extensiones
    case UE_E_ACCOUNT:
        (void *)&(*windowManager + new E_ACCOUNT);
        break;
    case UE_E_ACCUM:
        (void *)&(*windowManager + new E_ACCUM);
        break;
    case UE_E_PARAMETERS:
        (void *)&(*windowManager + new E_PARAMETERS);
        break;
    case UE_E_ACTIVATE:
        (void *)&(*windowManager + new UIW_PASSWORD(FALSE));
        break;
    case UE_E_DEACTIVATE:
        WMenu->Event(UI_EVENT(L_RIGHT, 0));
        WExtMenu->woFlags |= WOF_NON_SELECTABLE;
        WActivateExt->woFlags  &= ~WOF_NON_SELECTABLE;
        WExtMenu->Event(UI_EVENT(S_REDISPLAY, 0));
        break;
        //
        //
        //
    default:
        ccode = UIW_WINDOW::Event(event);
        if (Current() == WToolBar)
            WStatBar->Update(WToolBar->Current());
        else if (Current() == WMenu)
            WStatBar->Update(WMenu->Current());
        break;
    }
    return ccode;
}

EVENT_TYPE UIW_VIEW::processBooth(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
    if (ccode != L_SELECT)
        return ccode;
    if (!g_cfg->MANUAL)
        return ccode;
    if (((UIW_INTEGER *)(object))->Validate(FALSE) != NMI_OK)
        return ccode;
    // ---
    int boothNum = 1;
    if (object && object->SearchID() == ID_BUTTON)
        boothNum = atoi(((UIW_BUTTON*) object)->DataGet(TRUE));
    else
        boothNum = ((UIW_INTEGER *) object)->DataGet();
    *windowManager + new UIW_MANUAL(boothNum-1);
    return ccode;
}

