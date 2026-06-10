//
// [ MB_INF.CPP ]
//

#include "stdst.h"

#include <control.h>
#include <menubar.h>
#include <sid.h>
#include <events.h>

#ifndef USE_HELP_CONTEXTS
#define USE_HELP_CONTEXTS
#include <help.hpp>
#endif

extern CFG	*g_cfg;

// --------------------------------------------------------------------------
//								UIW_SYS_INFO
// --------------------------------------------------------------------------

UIW_SYS_INFO::UIW_SYS_INFO(void) : UIW_WINDOW("W_SYS_INFO", defaultStorage)
{
	UIW_INTEGER *wInteger;
	UIW_STRING  *wString;

	wString = (UIW_STRING *) Get("W_BOOTHS");
	int integer;
	STR32 str, str2;
	itoa(g_cfg->ACTIVE_CLUSTERS*CLUSTER_SIZE, str, 10);
	strcat(str, "/");
	itoa(g_cfg->CLUSTERS*CLUSTER_SIZE, str2, 10);
	strcat(str, str2);
	wString->DataSet(str);
	//
	wString = (UIW_STRING *) Get("W_S_SIGNAL");
	wString->DataSet(g_cfg->SIGNAL);
	//
	wString = (UIW_STRING *) Get("W_FORM");
	wString->DataSet(g_cfg->P_FORM);
	//
	wString = (UIW_STRING *) Get("W_OPERATION");
	wString->DataSet(g_cfg->P_OPERATION);
    //
	wString = (UIW_STRING *) Get("W_PORT");
	if (!strcmp(g_cfg->P_PORT, "lpt"))
	{
        strcpy(str, "LPT");
		strcat(str, (g_cfg->LPT == 1)?"1":"2");
    }
    else
    {
        strcpy(str, "COM");
		strcat(str, g_cfg->COM);
    }
    wString->DataSet(str);
    wString = (UIW_STRING *) Get("W_TIME");

	switch (g_cfg->ASIGNAL)
    {
    case CFG::S_BIAS:
		itoa(g_cfg->T_BIAS, str, 10);
        strcat(str, " milisegundos");
        break;
	case CFG::S_THREAD:
        str[0] = 0;
		break;
	case CFG::S_TONE:
		str[0] = 0;
		break;
	case CFG::S_TIME:
		itoa((g_cfg->T_COM)/1000, str, 10);
		strcat(str, " segundos");
		break;
	}
	wString->DataSet(str);
	wInteger = (UIW_INTEGER *) Get("W_T_TALK");
	integer = g_cfg->T_TALK/1000;
	wInteger->DataSet(&integer);
	wInteger = (UIW_INTEGER *) Get("W_T_DIAL");
	integer = g_cfg->T_DIAL/1000;
	wInteger->DataSet(&integer);
	wInteger = (UIW_INTEGER *) Get("W_T_LOCK");
	integer = g_cfg->T_LOCK;
	wInteger->DataSet(&integer);
	wInteger = (UIW_INTEGER *) Get("W_T_ANSWER");
	integer = g_cfg->T_ANSWER/1000;
	wInteger->DataSet(&integer);

	UIW_BIGNUM *pwBigNum;
	pwBigNum = (UIW_BIGNUM *) Get("W_ROUND");
	pwBigNum->DataSet(&UI_BIGNUM(g_cfg->M_ROUND));
    //
	((UIW_INTEGER*) Get(SID_INTER))->DataSet((int *)&g_cfg->INTER_DIGITS);
	((UIW_INTEGER*) Get(SID_BORDER))->DataSet((int *)&g_cfg->BORDER_DIGITS);
	((UIW_INTEGER*) Get(SID_CELLULAR))->DataSet((int *)&g_cfg->CELLULAR_DIGITS);
	((UIW_INTEGER*) Get(SID_NAL))->DataSet((int *)&g_cfg->NAL_DIGITS);
	((UIW_INTEGER*) Get(SID_LOCAL))->DataSet((int *)&g_cfg->LOCAL_DIGITS);
	((UIW_INTEGER*) Get(SID_SPECIAL))->DataSet((int *)&g_cfg->SPECIAL_DIGITS);
    // wString = (UIW_STRING *) Get(SID_OTHERS);
	// $$$ wString->DataSet(g_cfg->LOCK_LIST);
    wString = (UIW_STRING *) Get("W_CASH");
	wString->DataSet(g_cfg->CASH);
    //
    windowManager->Center(this);
    helpContext = H_SYS_INFO;
}

EVENT_TYPE UIW_SYS_INFO::Event(const UI_EVENT &event)
{
	// Switch on the type of event.
    EVENT_TYPE eventType = event.type;
    switch (eventType)
    {
    case UE_ACCEPT:
        eventManager->Put(UI_EVENT(S_CLOSE,0));
        break;
    default:
        eventType = UIW_WINDOW::Event(event);
        break;
    }
    return eventType;
}

// --------------------------------------------------------------------------
//								UIW_OP_ID
// --------------------------------------------------------------------------

UIW_OP_ID::UIW_OP_ID(void) : UIW_WINDOW("W_OP_ID", defaultStorage)
{
    UIW_BUTTON *tWButton;
	if (!strcmp(g_cfg->OP_TITLE, "Se�or"))
    {
        tWButton = (UIW_BUTTON *) Get("W_MR");
    }
	else if (!strcmp(g_cfg->OP_TITLE, "Se�ora"))
    {
        tWButton = (UIW_BUTTON *) Get("W_MRS");
    }
    else
    {
        tWButton = (UIW_BUTTON *) Get("W_MISS");
    }
    tWButton->woStatus |= (WOS_SELECTED|WOS_CURRENT);
    UIW_STRING *wString = (UIW_STRING *) Get("W_NAME");
	wString->DataSet(g_cfg->OP_NAME);
    windowManager->Center(this);
    helpContext = H_OP_ID;
}

EVENT_TYPE UIW_OP_ID::Event(const UI_EVENT &event)
{
    // Switch on the type of event.
    EVENT_TYPE eventType = event.type;
    switch (eventType)
    {
    case UE_ACCEPT:
        UIW_BUTTON *tWButton;
        tWButton = (UIW_BUTTON *) Get("W_MR");
        if (FlagSet(tWButton->woStatus, WOS_SELECTED))
			strcpy(g_cfg->OP_TITLE, "Se�or");
        else
        {
            tWButton = (UIW_BUTTON *) Get("W_MRS");
            if (FlagSet(tWButton->woStatus, WOS_SELECTED))
				strcpy(g_cfg->OP_TITLE, "Se�ora");
            else
				strcpy(g_cfg->OP_TITLE, "Se�orita");
        }
        UIW_STRING *wString = (UIW_STRING *) Get("W_NAME");
		strcpy(g_cfg->OP_NAME, wString->DataGet());
		g_cfg->Save();
        eventManager->Put(UI_EVENT(S_CLOSE,0));
        break;
    case UE_CANCEL:
        eventManager->Put(UI_EVENT(S_CLOSE,0));
        break;
    default:
        eventType = UIW_WINDOW::Event(event);
        break;
    }
    return eventType;
}

// --------------------------------------------------------------------------
//								UIW_ALARM
// --------------------------------------------------------------------------

UIW_ALARM::UIW_ALARM(void) : UIW_WINDOW("W_ALARM", defaultStorage)
{
    UIW_INTEGER *wInteger;
    wInteger = (UIW_INTEGER *) Get("W_CERR");
    int integer = 3;
    wInteger->Information(SET_TEXT_LENGTH, &integer);
	wInteger->DataSet((int *)&g_cfg->MAX_COM_ERR);
    wInteger = (UIW_INTEGER *) Get("W_DERR");
    wInteger->Information(SET_TEXT_LENGTH, &integer);
	wInteger->DataSet((int *)&g_cfg->MAX_DIAL_ERR);
    windowManager->Center(this);
    helpContext = H_ALARM;
}

EVENT_TYPE UIW_ALARM::Event(const UI_EVENT &event)
{
    // Switch on the type of event.
    EVENT_TYPE eventType = event.type;
    switch (eventType)
    {
    case UE_ACCEPT:
        UIW_INTEGER *wInteger;
        wInteger = (UIW_INTEGER *) Get("W_CERR");
		g_cfg->MAX_COM_ERR = wInteger->DataGet();
        wInteger = (UIW_INTEGER *) Get("W_DERR");
		g_cfg->MAX_DIAL_ERR = wInteger->DataGet();
		g_cfg->Save();
        eventManager->Put(UI_EVENT(S_CLOSE,0));
        break;
    case UE_CANCEL:
        eventManager->Put(UI_EVENT(S_CLOSE,0));
        break;
    default:
        eventType = UIW_WINDOW::Event(event);
        break;
    }
    return eventType;
}

// --------------------------------------------------------------------------
//								UIW_PASSWORD
// --------------------------------------------------------------------------

static BOOL forConfig;

UIW_PASSWORD::UIW_PASSWORD(BOOL forConfig) : UIW_WINDOW("W_PASSWORD", defaultStorage)
{
    ::forConfig = forConfig;
    windowManager->Center(this);
    helpContext = H_PASSWORD;
}

EVENT_TYPE UIW_PASSWORD::Event(const UI_EVENT &event)
{
    // Switch on the type of event.
    EVENT_TYPE eventType = event.type;
    switch (eventType)
    {
    case UE_ACCEPT:
    {
        UIW_STRING *wString = (UIW_STRING *)Get("S_PASSWD");
        char *str = wString->DataGet();
        if (!strlen(str))
        {
        	errorSystem->ReportError(
				windowManager, WOS_NO_STATUS,
				"Por favor entre un c�digo de acceso\n");
        }
        else
		{
            if (forConfig)
            {
				if (g_cfg->SetCurrentPassword(str) && !g_cfg->PasswordIs(str, CFG::UTIL))
                {
                    eventManager->Put(UI_EVENT(S_CLOSE,0));
                    UI_EVENT tmpEvent;
                    tmpEvent.type = E_CONTROLLER;
                    tmpEvent.rawCode = UE_CONFIG_ON;
                    tmpEvent.region.left = TRUE; // use the region left
					if (g_cfg->PasswordIs(str, CFG::BACKDOOR) || g_cfg->PasswordIs(str, CFG::SUPERVISOR))
                        tmpEvent.region.right = TRUE; // use the region right
                    else
                        tmpEvent.region.right = FALSE;// use the region right
                    eventManager->Event(tmpEvent, E_CONTROLLER);
                }
                else
                    errorSystem->ReportError(windowManager, WOS_NO_STATUS, "C�digo de acceso inv�lido\n");
            }
            else
            {
				if (g_cfg->PasswordIs(str, CFG::BACKDOOR)   ||
						g_cfg->PasswordIs(str, CFG::SUPERVISOR) ||
						g_cfg->PasswordIs(str, CFG::OPERATOR)
                   )
                {
                    eventManager->Put(UI_EVENT(S_CLOSE,0));
                    UI_EVENT tmpEvent;
                    tmpEvent.type = E_CONTROLLER;
                    tmpEvent.rawCode = UE_EXTENSION_ON;
                    tmpEvent.region.left = TRUE; // use the region left
                    if (!g_cfg->PasswordIs(str, CFG::OPERATOR))
                        tmpEvent.region.right = TRUE; // use the region right
                    else
                        tmpEvent.region.right = FALSE;// use the region right
                    eventManager->Event(tmpEvent, E_CONTROLLER);
                }
                else
                    errorSystem->ReportError(windowManager, WOS_NO_STATUS, "C�digo de acceso inv�lido\n");
            }
        }

        break;
    }
    case UE_CANCEL:
        eventManager->Put(UI_EVENT(S_CLOSE,0));
        break;
    default:
        eventType = UIW_WINDOW::Event(event);
        break;
    }
    return eventType;
}

