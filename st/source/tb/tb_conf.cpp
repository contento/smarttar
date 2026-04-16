//
// [ TB_CONF.CPP ]
//

#include "stdst.h"

#pragma hdrstop

#include <toolbar.h>
#include <events.h>
#include <hb_ids.h>
#include <res.hpp>

#ifndef USE_HELP_CONTEXTS
#define USE_HELP_CONTEXTS
#include <help.hpp>
#endif

extern CFG	*g_cfg;

// --------------------------------------------------------------------------
//								UIW_OPERATION
// --------------------------------------------------------------------------

UIW_OPERATION::UIW_OPERATION(void) : UIW_WINDOW("W_OPERATION", defaultStorage)
{
    UIW_BUTTON *wButton;
	if (!strcmp(g_cfg->P_OPERATION, "automatica"))
        wButton = (UIW_BUTTON *) Get("W_AUTO");
    else
        wButton = (UIW_BUTTON *) Get("W_MANUAL");
    wButton->woStatus |= WOS_SELECTED|WOS_CURRENT;
    windowManager->Center(this);
    helpContext = H_OPERATION;
}

EVENT_TYPE UIW_OPERATION::Event(const UI_EVENT &event)
{
    // Switch on the type of event.
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_ACCEPT:
        UIW_BUTTON *wButton = (UIW_BUTTON *) Get("W_AUTO");
        UI_EVENT tmpEvent;
        tmpEvent.type = E_CONTROLLER;
        tmpEvent.rawCode = UE_AUTO_ON;
        if (FlagSet(wButton->woStatus, WOS_SELECTED))
        {
			strcpy(g_cfg->P_OPERATION, "automatica");
            tmpEvent.region.left = TRUE;  // use the region left
			g_cfg->MANUAL = FALSE;
        }
        else
        {
			strcpy(g_cfg->P_OPERATION, "manual");
			g_cfg->MANUAL = TRUE;
            tmpEvent.region.left = FALSE; // use the region left
        }


		g_cfg->Save();
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
//								UIW_FORMS
// --------------------------------------------------------------------------

UIW_FORMS::UIW_FORMS(void) : UIW_WINDOW("W_FORMS", defaultStorage)
{
    UIW_COMBO_BOX *wCombo;
    wCombo = (UIW_COMBO_BOX *) Get("W_P_FORMS");
    // search for the current printer
    for (UIW_STRING *wStr = (UIW_STRING *) wCombo->First(); wStr; wStr=(UIW_STRING *)wStr->Next())
    {
		if (!strcmp(wStr->DataGet(), g_cfg->P_FORM))
            wCombo->list.SetCurrent(wStr);
    }
    // set help bar connections
    ((UIW_STRING *) Get("W_80DR"))->helpContext = HB_80_FORM;
    ((UIW_STRING *) Get("W_80TP"))->helpContext = HB_80_FORM;
    ((UIW_STRING *) Get("W_80L"))->helpContext  = HB_80_FORM;
    ((UIW_STRING *) Get("W_40DR"))->helpContext = HB_40_FORM;
    ((UIW_STRING *) Get("W_40RS"))->helpContext = HB_40_FORM;
    ((UIW_STRING *) Get("W_18DR"))->helpContext = HB_18_FORM;
    ((UIW_STRING *) Get("W_EME"))->helpContext  = HB_80_FORM;
    ((UIW_STRING *) Get("W_HALF"))->helpContext = HB_80_FORM;
    //
    windowManager->Center(this);
    helpContext = H_FORMS;
}

EVENT_TYPE UIW_FORMS::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_ACCEPT:
        {
            UIW_COMBO_BOX *wCombo;
            wCombo = (UIW_COMBO_BOX *) Get("W_P_FORMS");
            UIW_STRING *wStr = (UIW_STRING *) wCombo->Current();
			strcpy(g_cfg->P_FORM, wStr->DataGet());
			g_cfg->AdjustForm();
			g_cfg->AdjustFooter();
			g_cfg->AdjustHeader();
            g_cfg->Save();
            //
            UI_EVENT tmpEvent;
            tmpEvent.type = E_CONTROLLER;
            tmpEvent.rawCode = UE_LOADPR_DLLS;
            eventManager->Event(tmpEvent, E_CONTROLLER);
            //
            eventManager->Put(UI_EVENT(S_CLOSE,0));
            break;
        }
    case UE_CANCEL:
        eventManager->Put(UI_EVENT(S_CLOSE,0));
        break;
    default:
        {
            ccode = UIW_WINDOW::Event(event);
            UIW_COMBO_BOX *wCombo;
            wCombo = (UIW_COMBO_BOX *) Get("W_P_FORMS");
            if (Current() == wCombo)
            {
                UI_EVENT tmpEvent;
                tmpEvent.type = E_CONTROLLER;
                tmpEvent.rawCode = UE_UPDATE_HLP_BAR;
                tmpEvent.data = wCombo->Current();
                eventManager->Event(tmpEvent, E_CONTROLLER);
            }
            break;
        }
    }
    return ccode;
}
