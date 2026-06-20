//
// [ MB_CONF.CPP ]
//

#include "stdst.h"

#include <control.h>
#include <ph_eng.h>
#include <db_eng.h>
#include <spooler.h>
#include <menubar.h>
#include <events.h>
#include <sid.h>

#ifndef USE_HELP_CONTEXTS
#define USE_HELP_CONTEXTS
#include <help.hpp>
#endif

extern CFG        *g_cfg;
extern DB_ENGINE  *g_dbEngine;
extern PH_ENGINE  *g_phEngine;
extern SPOOLER    *g_spooler;

// --------------------------------------------------------------------------
//								UIW_TIME_DATE
// --------------------------------------------------------------------------

UIW_TIME_DATE::UIW_TIME_DATE(void) : UIW_WINDOW("W_TIME_DATE", defaultStorage)
{
    windowManager->Center(this);
    helpContext = H_TIME_DATE;
}

EVENT_TYPE UIW_TIME_DATE::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_ACCEPT:
        UI_DATE *date = ((UIW_DATE *) Get("W_DATE"))->DataGet();
        UI_TIME *time = ((UIW_TIME *) Get("W_TIME"))->DataGet();
        int packed;
        time->Export(&packed);
        _SetSysTime(packed);
        date->Export(&packed);
        _SetSysDate(packed);
        eventManager->Put(UI_EVENT(S_CLOSE,0));
        //
        UI_EVENT tmpEvent;
        tmpEvent.type = E_CONTROLLER;
        // use the raw code To put service
        tmpEvent.rawCode = UE_REFRESH_STBAR;
        eventManager->Put(tmpEvent);
        //
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
//								UIW_TIME_TIMES
// --------------------------------------------------------------------------

UIW_TIMES::UIW_TIMES(void) : UIW_WINDOW("W_TIMES", defaultStorage)
{
    UIW_INTEGER *wInteger;
    int integer;
    wInteger = (UIW_INTEGER *) Get("W_T_TALK");
	integer = g_cfg->T_TALK/1000;
    wInteger->DataSet(&integer);
    integer = 3;
    wInteger->Information(SET_TEXT_LENGTH, &integer);
    wInteger = (UIW_INTEGER *) Get("W_T_DIAL");
	integer = g_cfg->T_DIAL/1000;
    wInteger->DataSet(&integer);
    integer = 3;
    wInteger->Information(SET_TEXT_LENGTH, &integer);
    wInteger = (UIW_INTEGER *) Get("W_T_LOCK");
	integer = g_cfg->T_LOCK;
    wInteger->DataSet(&integer);
    integer = 5;
    wInteger->Information(SET_TEXT_LENGTH, &integer);
    wInteger = (UIW_INTEGER *) Get("W_T_ANSWER");
	integer = g_cfg->T_ANSWER/1000;
    wInteger->DataSet(&integer);
    integer = 3;
    wInteger->Information(SET_TEXT_LENGTH, &integer);
    wInteger = (UIW_INTEGER *) Get("W_T_COM");
	integer = g_cfg->T_COM/1000;
    wInteger->DataSet(&integer);
    integer = 3;
    wInteger->Information(SET_TEXT_LENGTH, &integer);
    //
    windowManager->Center(this);
    helpContext = H_TIMES;
}

EVENT_TYPE UIW_TIMES::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_ACCEPT:
        UIW_INTEGER *wInteger;
        wInteger = (UIW_INTEGER *) Get("W_T_TALK");
		g_cfg->T_TALK = wInteger->DataGet()*1000;
        wInteger = (UIW_INTEGER *) Get("W_T_DIAL");
		g_cfg->T_DIAL = wInteger->DataGet()*1000;
        wInteger = (UIW_INTEGER *) Get("W_T_LOCK");
		g_cfg->T_LOCK = wInteger->DataGet();
        wInteger = (UIW_INTEGER *) Get("W_T_ANSWER");
		g_cfg->T_ANSWER = wInteger->DataGet()*1000;
        wInteger = (UIW_INTEGER *) Get("W_T_COM");
		g_cfg->T_COM = wInteger->DataGet()*1000;
		g_cfg->Save();
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
//								UIW_SIGNAL
// --------------------------------------------------------------------------

UIW_SIGNAL::UIW_SIGNAL(void) : UIW_WINDOW("W_SIGNAL", defaultStorage)
{
    LastEvent = 0;
    WSignalGroup = (UIW_GROUP *) Get("W_SIGNAL_GROUP");
    UIW_BUTTON *wButton;
	switch (g_cfg->ASIGNAL)
    {
    case CFG::S_BIAS:
        wButton = (UIW_BUTTON *) Get("W_INV");
        eventManager->Put(UI_EVENT(UE_S_INVERT,0));
        break;
    case CFG::S_TIME:
        eventManager->Put(UI_EVENT(UE_S_TIME,0));
        wButton = (UIW_BUTTON *) Get("W_TIME");
        break;
    case CFG::S_THREAD:
        eventManager->Put(UI_EVENT(UE_S_THREAD,0));
        wButton = (UIW_BUTTON *) Get("W_THREAD");
        break;
    case CFG::S_TONE:
        eventManager->Put(UI_EVENT(UE_S_TONE,0));
        wButton = (UIW_BUTTON *) Get("W_TONE");
        break;
    }
    wButton->Event(S_CURRENT);
    helpContext = H_SIGNAL;
    windowManager->Center(this);
}

EVENT_TYPE UIW_SIGNAL::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_ACCEPT:
        switch(LastEvent)
        {
        case UE_S_INVERT:
			strcpy(g_cfg->SIGNAL, "inversion");
            if (FlagSet(WSteadyButton->woStatus, WOS_SELECTED))
				g_cfg->T_BIAS = 500;
            else
				g_cfg->T_BIAS = 150;
			g_cfg->T_COM = 61000U;
			g_cfg->ASIGNAL = CFG::S_BIAS;
            break;
        case UE_S_TIME:
			strcpy(g_cfg->SIGNAL, "tiempo");
			g_cfg->T_COM = WTCom->DataGet()*1000;
			g_cfg->ASIGNAL = CFG::S_TIME;
            break;
        case UE_S_THREAD:
			strcpy(g_cfg->SIGNAL, "hilo c");
            // T_BIAS is used for C Thread signaling too !!!
			g_cfg->T_BIAS = 150;
			g_cfg->ASIGNAL = CFG::S_THREAD;
            break;
        case UE_S_TONE:
			strcpy(g_cfg->SIGNAL, "tone");
			g_cfg->T_BIAS = 60;
			g_cfg->ASIGNAL = CFG::S_TONE;

			break;
        }
		g_cfg->Save();
        eventManager->Put(UI_EVENT(S_CLOSE,0));
        break;
    case UE_CANCEL:
        eventManager->Put(UI_EVENT(S_CLOSE,0));
        break;
    case UE_S_INVERT:
        LastEvent = ccode;
        WSignalGroup->Destroy();
        UIW_BUTTON *tW150;
        WSignalGroup->Information(SET_TEXT, "Por inversión");
        *WSignalGroup
        + (WSteadyButton = new UIW_BUTTON(1, 1, 20, "Permanente", BTF_RADIO_BUTTON))
        + (tW150         = new UIW_BUTTON(1, 2, 20, "Durante 150 ms", BTF_RADIO_BUTTON));
		if (g_cfg->T_BIAS == 150)
            tW150->woStatus |= WOS_SELECTED;
        else
            WSteadyButton->woStatus |= WOS_SELECTED;
        WSignalGroup->Event(UI_EVENT(S_REDISPLAY, 0));
        break;
    case UE_S_TIME:
        LastEvent = ccode;
		int ms = g_cfg->T_COM/1000;
        WSignalGroup->Destroy();
        WSignalGroup->Information(SET_TEXT, "Por tiempo");
        *WSignalGroup
        +          new UIW_PROMPT (1, 1, "Tiempo")
        + (WTCom = new UIW_INTEGER(8, 1, 5, &ms, "15..61", NMF_NO_FLAGS, WOF_BORDER|WOF_AUTO_CLEAR))
        +          new UIW_PROMPT (14, 1, "segundos");
        WSignalGroup->Event(UI_EVENT(S_REDISPLAY, 0));
        break;
	case UE_S_THREAD:
		{
			UIW_BUTTON *wButton;
			LastEvent = ccode;
			WSignalGroup->Destroy();
			WSignalGroup->Information(SET_TEXT, "Por hilo C");
			*WSignalGroup
				+ ( wButton = new UIW_BUTTON(1, 1, 20, "Con Tierra", BTF_RADIO_BUTTON))
				+ new UIW_BUTTON(1, 2, 20, "Con -48V", BTF_RADIO_BUTTON);
			wButton->woStatus |= WOS_SELECTED;
			WSignalGroup->Event(UI_EVENT(S_REDISPLAY, 0));
			break;
		}
	case UE_S_TONE:
		{
			LastEvent = ccode;
			WSignalGroup->Destroy();
			WSignalGroup->Information(SET_TEXT, "Por tonos");
			UIW_BUTTON *wButton;
			*WSignalGroup
				+ ( wButton = new UIW_BUTTON(1, 1, 20, "12", BTF_RADIO_BUTTON))
				+ new UIW_BUTTON(1, 2, 20, "16", BTF_RADIO_BUTTON);
			wButton->woStatus |= WOS_SELECTED;
			WSignalGroup->Event(UI_EVENT(S_REDISPLAY, 0));
			break;
		}
	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}
	return ccode;
}

// --------------------------------------------------------------------------
//								UIW_LOCK_NUM
// --------------------------------------------------------------------------

UIW_LOCK_NUM::UIW_LOCK_NUM(void) : UIW_WINDOW("W_LOCK_NUM", defaultStorage)
{
    UIW_INTEGER *wInteger;
    int integer = 3;
    wInteger = (UIW_INTEGER*) Get(SID_INTER);
	wInteger->DataSet((int *)&g_cfg->INTER_DIGITS);
    wInteger->Information(SET_TEXT_LENGTH, &integer);
    wInteger = (UIW_INTEGER*) Get(SID_BORDER);
	wInteger->DataSet((int *)&g_cfg->BORDER_DIGITS);
    wInteger->Information(SET_TEXT_LENGTH, &integer);
    wInteger = (UIW_INTEGER*) Get(SID_CELLULAR);
	wInteger->DataSet((int *)&g_cfg->CELLULAR_DIGITS);
    wInteger->Information(SET_TEXT_LENGTH, &integer);
    wInteger = (UIW_INTEGER*) Get(SID_NAL);
	wInteger->DataSet((int *)&g_cfg->NAL_DIGITS);
    wInteger->Information(SET_TEXT_LENGTH, &integer);
    wInteger = (UIW_INTEGER*) Get(SID_LOCAL);
	wInteger->DataSet((int *)&g_cfg->LOCAL_DIGITS);
    wInteger->Information(SET_TEXT_LENGTH, &integer);
    wInteger = (UIW_INTEGER*) Get(SID_SPECIAL);
	wInteger->DataSet((int *)&g_cfg->SPECIAL_DIGITS);
    wInteger->Information(SET_TEXT_LENGTH, &integer);
    //
    UIW_GROUP *wGroup;
    UIW_PHONE *wPhone;
    PHONE phone;
    int i;
    // LOCK_LIST (see MAX_LOCKED_NUMBERS)
    wGroup = (UIW_GROUP *) Get("LOCK_LIST");
    i = 0;
    for (wPhone = (UIW_PHONE *) wGroup->First(); wPhone; wPhone=(UIW_PHONE *)wPhone->Next())
    {
        PH_ENGINE::GetLockedNumber(phone, i++);
        wPhone->DataSet(phone);
    }
    // LOCK_RANGES (see MAX_LOCKED_NUMBERS)
    wGroup = (UIW_GROUP *) Get("LOCK_RANGES");
    i = MAX_LOCKED_NUMBERS/2;
    for (wPhone = (UIW_PHONE *) wGroup->First(); wPhone; wPhone=(UIW_PHONE *)wPhone->Next())
    {
        if (wPhone->SearchID() != ID_PROMPT)
        { // jump over prompts
            PH_ENGINE::GetLockedNumber(phone, i++);
            wPhone->DataSet(phone);
        }
    }
    //
    windowManager->Center(this);
    helpContext = H_LOCK_NUM;
}

EVENT_TYPE UIW_LOCK_NUM::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_ACCEPT:
        UIW_INTEGER *wInteger;
        wInteger = (UIW_INTEGER*) Get(SID_INTER);
		g_cfg->INTER_DIGITS = wInteger->DataGet();
        wInteger = (UIW_INTEGER*) Get(SID_BORDER);
		g_cfg->BORDER_DIGITS = wInteger->DataGet();
        wInteger = (UIW_INTEGER*) Get(SID_NAL);
		g_cfg->NAL_DIGITS = wInteger->DataGet();
        wInteger = (UIW_INTEGER*) Get(SID_LOCAL);
		g_cfg->LOCAL_DIGITS  = wInteger->DataGet();
        wInteger = (UIW_INTEGER*) Get(SID_CELLULAR);
		g_cfg->CELLULAR_DIGITS = wInteger->DataGet();
        wInteger = (UIW_INTEGER*) Get(SID_SPECIAL);
		g_cfg->SPECIAL_DIGITS = wInteger->DataGet();
        //
        UIW_GROUP *wGroup;
        UIW_PHONE *wPhone;
        PHONE phone;
        int i;
        // LOCK_LIST (see MAX_LOCKED_NUMBERS)
        wGroup = (UIW_GROUP *) Get("LOCK_LIST");
        i = 0;
        for (wPhone = (UIW_PHONE *) wGroup->First(); wPhone; wPhone=(UIW_PHONE *)wPhone->Next())
        {
            strcpy((char *)&phone, wPhone->DataGet());
            PH_ENGINE::SetLockedNumber(phone, i++);
        }
        // LOCK_RANGES (see MAX_LOCKED_NUMBERS)
        wGroup = (UIW_GROUP *) Get("LOCK_RANGES");
        i = MAX_LOCKED_NUMBERS/2;
        for (wPhone = (UIW_PHONE *) wGroup->First(); wPhone; wPhone=(UIW_PHONE *)wPhone->Next())
        {
            if (wPhone->SearchID() != ID_PROMPT)
            { // jump over prompts
                strcpy((char *)&phone, wPhone->DataGet());
                PH_ENGINE::SetLockedNumber(phone, i++);
            }
        }
        //
		g_cfg->Save();
		g_phEngine->SaveToInfs();
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
//								UIW_NAL_TAR
// --------------------------------------------------------------------------

UIW_NAL_TAR::UIW_NAL_TAR(BOOL select) : UIW_WINDOW("W_NAL_TAR", defaultStorage)
{
    Select = select;
    WTarGroup = (UIW_GROUP  *) Get("G_FULL");
    WSchGroup = (UIW_GROUP  *) Get("G_REDUCED");
    WSchGroup = (UIW_GROUP  *) Get("G_REDUCED");
    WTax      = (UIW_BIGNUM *) Get("W_TAX");
    WBApply   = (UIW_BUTTON *) Get("W_B_APPLY");
    WEApply   = (UIW_BUTTON *) Get("W_E_APPLY");
    UIW_BUTTON *wButton = (UIW_BUTTON *) Get("W_CANCEL");
    if (!Select)
    {
        WTarGroup->woFlags |= WOF_NON_SELECTABLE;
        WSchGroup->woFlags |= WOF_NON_SELECTABLE;
        WTax->woFlags      |= WOF_NON_SELECTABLE;
        wButton->woFlags   |= WOF_NON_SELECTABLE;
        WBApply->woFlags   |= WOF_NON_SELECTABLE;
        WEApply->woFlags   |= WOF_NON_SELECTABLE;
    }
    windowManager->Center(this);
    helpContext = H_NAL_TAR;
    wnFlags |= WNF_SELECT_MULTIPLE;
}

EVENT_TYPE UIW_NAL_TAR::Event(const UI_EVENT &event)
{
    int i, schedule, dayType;
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case S_CREATE:
        UIW_WINDOW::Event(event);
        //
        int middle = MAX_DDN_TARIFF/2, integer = 9;
        char s[3];
        // full tariffs
        for (i = 0; i < MAX_DDN_TARIFF; i++)
        {
            WTarPrompts[i] = new UIW_PROMPT(24*(i/middle)+2, i%middle+1, 5, itoa(i, s, 10));
			WTars[i] = new UIW_REAL(24*(i/middle)+6, i%middle+1, 10, &g_phEngine->GetDDNTariff(i).Value);
            WTars[i]->woFlags |= WOF_NO_ALLOCATE_DATA;
            WTars[i]->Information(SET_TEXT_LENGTH, &integer);
			WIVAButtons[i] = new UIW_BUTTON(24*(i/middle)+16, i%middle+1, 7, g_cfg->TAX_NAME, BTF_CHECK_BOX);
			if (g_phEngine->GetDDNTariff(i).TaxPercent)
                WIVAButtons[i]->woStatus |= WOS_SELECTED;
            *WTarGroup
            + WTarPrompts[i]
            + WTars[i]
            + WIVAButtons[i]
            ;
        }
        // schedule
        int value;
        for (dayType = 0; dayType < MAX_DDN_DAY_TYPE; dayType++)
            for (schedule = 0; schedule < MAX_DDN_SCHEDULE; schedule++)
            {
				value = g_phEngine->GetDDNSchedule(dayType, schedule).From;
                WfromTimes[dayType][schedule] =
                    new UIW_TIME(dayType*19+7*0+3, schedule+2, 7, &UI_TIME(value), NULL, TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL);
				value = g_phEngine->GetDDNSchedule(dayType, schedule).To;
                WtoTimes[dayType][schedule] =
                    new UIW_TIME(dayType*19+7*1+3, schedule+2, 7, &UI_TIME(value), NULL, TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL);
				value = g_phEngine->GetDDNSchedule(dayType, schedule).Percent;
                WPercents[dayType][schedule]  =
                    new UIW_INTEGER(dayType*19+7*2+3, schedule+2, 4, &value, "0..100");
                *WSchGroup
                + WfromTimes[dayType][schedule]
                + WtoTimes[dayType][schedule]
                + WPercents[dayType][schedule]
                ;
            }
        // TaxPercent
		((UIW_STRING *)Get("W_P_TAX"))->DataSet(g_cfg->TAX_NAME);
		WTax->DataSet(&UI_BIGNUM(g_cfg->DDN_TAX));
        // apply reduced schedule
		if (g_cfg->APPLY_DDN_SCHEDULE)
            WBApply->woStatus |= WOS_SELECTED;
        else
            WBApply->woStatus &= ~WOS_SELECTED;
		if (g_cfg->E_APPLY_DDN_SCHEDULE)
            WEApply->woStatus |= WOS_SELECTED;
        else
            WEApply->woStatus &= ~WOS_SELECTED;
        break;
    case UE_ACCEPT:
        if (!Select)
        {
			eventManager->Put(UI_EVENT(S_CLOSE,0));
			break;
		}
		// --- Tax Percent
		WTax->DataGet()->Export(&g_cfg->DDN_TAX);
		// --- tars
		for (i = 0; i < MAX_DDN_TARIFF; i++)
		{
			if (FlagSet(WIVAButtons[i]->woStatus, WOS_SELECTED))
				g_phEngine->GetDDNTariff(i).TaxPercent = g_cfg->DDN_TAX;
			else
				g_phEngine->GetDDNTariff(i).TaxPercent = 0;
			g_phEngine->GetDDNTariff(i).Value = WTars[i]->DataGet();
		}
		// --- schedule
		for (dayType = 0; dayType < MAX_DDN_DAY_TYPE; dayType++)
            for (schedule = 0; schedule < MAX_DDN_SCHEDULE; schedule++)
            {
				(WfromTimes[dayType][schedule]->DataGet())->Export((int *)&g_phEngine->GetDDNSchedule(dayType, schedule).From);
				(WtoTimes[dayType][schedule]->DataGet())->Export((int *)&g_phEngine->GetDDNSchedule(dayType, schedule).To);
				g_phEngine->GetDDNSchedule(dayType, schedule).Percent = WPercents[dayType][schedule]->DataGet();
            }
        // apply reduced schedule
		g_cfg->APPLY_DDN_SCHEDULE = FlagSet(WBApply->woStatus, WOS_SELECTED)?1:0;
		g_cfg->E_APPLY_DDN_SCHEDULE = FlagSet(WEApply->woStatus, WOS_SELECTED)?1:0;
		g_phEngine->SaveToInfs();
		//
		g_cfg->Save(); // 2.21.8
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
//								UIW_INTER_TAR
// --------------------------------------------------------------------------

UIW_INTER_TAR::UIW_INTER_TAR(BOOL select) : UIW_WINDOW("W_INTER_TAR", defaultStorage)
{
    Select = select;
    WFullGroup   = (UIW_GROUP *)  Get("G_FULL");
    WUSAGroup    = (UIW_GROUP *)  Get("G_USA");
    WOthersGroup = (UIW_GROUP *)  Get("G_OTHERS");
    WBorderGroup = (UIW_GROUP *)  Get("G_BORDER");
    WTax         = (UIW_BIGNUM *) Get("W_TAX");
    WBApply      = (UIW_BUTTON *) Get("W_B_APPLY");
    WEApply      = (UIW_BUTTON *) Get("W_E_APPLY");
    UIW_BUTTON *wButton = (UIW_BUTTON *) Get("W_CANCEL");
    if (!select)
    {
        WFullGroup->woFlags   |= WOF_NON_SELECTABLE;
        WUSAGroup->woFlags    |= WOF_NON_SELECTABLE;
        WOthersGroup->woFlags |= WOF_NON_SELECTABLE;
        WBorderGroup->woFlags |= WOF_NON_SELECTABLE;
        WTax->woFlags         |= WOF_NON_SELECTABLE;
        wButton->woFlags      |= WOF_NON_SELECTABLE;
        WBApply->woFlags   |= WOF_NON_SELECTABLE;
        WEApply->woFlags   |= WOF_NON_SELECTABLE;
    }
    //
    windowManager->Center(this);
    helpContext = H_INTER_TAR;
    wnFlags |= WNF_SELECT_MULTIPLE;
}

EVENT_TYPE UIW_INTER_TAR::Event(const UI_EVENT &event)
{
    int i, dayType, schedule;
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case S_CREATE:
        UIW_WINDOW::Event(event);
        int n4 = MAX_DDI_TARIFF/4, integer = 9;
        char s[3];
        // full tariffs
        for (i = 0; i < MAX_DDI_TARIFF; i++)
        {
            WTarPrompts[i] = new UIW_PROMPT(16*(i/n4)+2, i%n4+1, 5, itoa(i, s, 10));
			WTars[i] = new UIW_REAL(16*(i/n4)+5, i%n4+1, 10, &g_phEngine->GetDDITariff(i).Value);
            WTars[i]->woFlags |= WOF_NO_ALLOCATE_DATA;
            WTars[i]->Information(SET_TEXT_LENGTH, &integer);
            *WFullGroup
            + WTarPrompts[i]
            + WTars[i]
            ;
        }
        // schedules
        int value;
        int y;
        for (dayType = 0; dayType < MAX_DDI_DAY_TYPE; dayType++)
            for (schedule = 0; schedule < MAX_DDI_SCHEDULE; schedule++)
            {
                UIW_GROUP *wGroup;
                // please see TInterTars !!! GCC/gcc
                switch (schedule)
                {
                case 0:
                case 1:
                    wGroup = WUSAGroup   ;
                    y = (schedule-0)+1;
                    break;
                case 2:
                case 3:
                    wGroup = WOthersGroup;
                    y = (schedule-2)+1;
                    break;
                case 4:
                    wGroup = WBorderGroup;
                    y = (schedule-4)+1;
                    break;
                }
				value = g_phEngine->GetDDISchedule(dayType, schedule).From;
                WfromTimes[dayType][schedule] =
                    new UIW_TIME(dayType*19+7*0+1, y, 7, &UI_TIME(value), NULL, TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL);
				value = g_phEngine->GetDDISchedule(dayType, schedule).To;
                WtoTimes[dayType][schedule] =
                    new UIW_TIME(dayType*19+7*1+1, y, 7, &UI_TIME(value), NULL, TMF_TWENTY_FOUR_HOUR|TMF_ZERO_FILL);
				value = g_phEngine->GetDDISchedule(dayType, schedule).Percent;
                WPercents[dayType][schedule]  =
                    new UIW_INTEGER(dayType*19+7*2+1, y, 4, &value, "0..100");
                //
                *wGroup
                + WfromTimes[dayType][schedule]
                + WtoTimes  [dayType][schedule]
                + WPercents [dayType][schedule]
                ;
            }
        // TaxPercent
		((UIW_STRING *)Get("W_P_TAX"))->DataSet(g_cfg->TAX_NAME);
		WTax->DataSet(&UI_BIGNUM(g_cfg->DDI_TAX));
        // apply reduced schedule
		if (g_cfg->APPLY_DDI_SCHEDULE)
            WBApply->woStatus |= WOS_SELECTED;
        else
            WBApply->woStatus &= ~WOS_SELECTED;
		if (g_cfg->E_APPLY_DDI_SCHEDULE)
            WEApply->woStatus |= WOS_SELECTED;
        else
            WEApply->woStatus &= ~WOS_SELECTED;
        break;
    case UE_ACCEPT:
        if (!Select)
        {
            eventManager->Put(UI_EVENT(S_CLOSE,0));
            break;
        }
        // --- TaxPercent
		WTax->DataGet()->Export(&g_cfg->DDI_TAX);
        // --- tars
        for (i = 0; i < MAX_DDI_TARIFF; i++)
        {
			g_phEngine->GetDDITariff(i).Value = WTars[i]->DataGet();
			g_phEngine->GetDDITariff(i).TaxPercent = g_cfg->DDI_TAX;
        }
        // --- schedules
        for (dayType = 0; dayType < MAX_DDI_DAY_TYPE; dayType++)
            for (schedule = 0; schedule < MAX_DDI_SCHEDULE; schedule++)
            {
				(WfromTimes[dayType][schedule]->DataGet())->Export((int *)&g_phEngine->GetDDISchedule(dayType, schedule).From);
				(WtoTimes[dayType][schedule]->DataGet())->Export((int *)&g_phEngine->GetDDISchedule(dayType, schedule).To);
				g_phEngine->GetDDISchedule(dayType, schedule).Percent = WPercents[dayType][schedule]->DataGet();
            }
        // apply reduced schedule
		g_cfg->APPLY_DDI_SCHEDULE = FlagSet(WBApply->woStatus, WOS_SELECTED)?1:0;
		g_cfg->E_APPLY_DDI_SCHEDULE = FlagSet(WEApply->woStatus, WOS_SELECTED)?1:0;
        //
		g_phEngine->SaveToInfs();
        //
		g_cfg->Save(); // 2.21.8
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
//								UIW_NEW_CITY
// --------------------------------------------------------------------------

UIW_NEW_CITY::UIW_NEW_CITY(void) : UIW_WINDOW("W_NEW_CITY", defaultStorage)
{
    int integer = 3;
    UIW_INTEGER *wInteger  = (UIW_INTEGER *) Get("W_TAR");
    wInteger->Information(SET_TEXT_LENGTH, &integer);
    windowManager->Center(this);
    helpContext = H_NEW_CITY;
}

EVENT_TYPE UIW_NEW_CITY::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    static BOOL save = FALSE;
    switch (ccode)
    {
    case UE_ACCEPT:
        {
            STR512 line;
            char str[0x04];
            UIW_STRING  *wCStr = (UIW_STRING  *) Get("W_CITY");
            UIW_INTEGER *wInteger  = (UIW_INTEGER *) Get("W_TAR");
            UIW_STRING  *wNStr = (UIW_STRING  *) Get("W_NUM");
            strcpy(line, wCStr->DataGet());
            strcat(line, ":");
            strcat(line, itoa(wInteger->DataGet(), str, 10));
            strcat(line, "=");
            strcat(line, wNStr->DataGet());
            UIW_BUTTON *wButton = (UIW_BUTTON *) Get("LOCAL");
            if (FlagSet(wButton->woStatus, WOS_SELECTED))
				g_phEngine->Add(PH_ENGINE::LOCAL_SOURCE, line);
            else
				g_phEngine->Add(PH_ENGINE::DDN_SOURCE, line);
            // clear
            int i=0;
            wCStr->DataSet("");
            wInteger->DataSet(&i);
            wNStr->DataSet("");
            wButton->woStatus &= ~WOS_SELECTED; // $$$
            save = TRUE;
            break;
        }
    case UE_CANCEL:
        {
            if (save)
            {
				g_phEngine->SaveToInfs();
            }
            save = FALSE;
            this->woAdvancedFlags &= ~WOAF_LOCKED; // disconnect lock !!!
            eventManager->Put(UI_EVENT(S_CLOSE,0));
            break;
        }
    default:
        ccode = UIW_WINDOW::Event(event);
        break;
    }
    return ccode;
}

// --------------------------------------------------------------------------
//								UIW_NEW_COUNTRY
// --------------------------------------------------------------------------

UIW_NEW_COUNTRY::UIW_NEW_COUNTRY(void) : UIW_WINDOW("W_NEW_COUNTRY", defaultStorage)
{
    int integer = 3;
    UIW_INTEGER *wInteger  = (UIW_INTEGER *) Get("W_TAR");
    wInteger->Information(SET_TEXT_LENGTH, &integer);
    windowManager->Center(this);
    helpContext = H_NEW_COUNTRY;
}

EVENT_TYPE UIW_NEW_COUNTRY::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    static BOOL save = FALSE;
    switch (ccode)
    {
    case UE_ACCEPT:
        {
            STR512 line;
            char str[0x04];
            UIW_STRING  *wCStr = (UIW_STRING  *) Get("W_COUNTRY");
            UIW_INTEGER *wInteger  = (UIW_INTEGER *) Get("W_TAR");
            UIW_STRING  *wNStr = (UIW_STRING  *) Get("W_NUM");
            strcpy(line, wCStr->DataGet());
            strcat(line, ":");
            strcat(line, itoa(wInteger->DataGet(), str, 10));
            strcat(line, "=");
            strcat(line, wNStr->DataGet());
			g_phEngine->Add(PH_ENGINE::DDI_SOURCE, line);
            // clear
            int ti=0;
            wCStr->DataSet("");
            wInteger->DataSet(&ti);
            wNStr->DataSet("");
            save = TRUE;
            break;
        }
    case UE_CANCEL:
        if (save)
            g_phEngine->SaveToInfs();
        save = FALSE;
        this->woAdvancedFlags &= ~WOAF_LOCKED; // disconnect lock !!!
        eventManager->Put(UI_EVENT(S_CLOSE,0));
        break;
    default:
        ccode = UIW_WINDOW::Event(event);
        break;
    }
    return ccode;
}

// --------------------------------------------------------------------------
//								UIW_ALIAS
// --------------------------------------------------------------------------

UIW_ALIAS::UIW_ALIAS(BOOL select) : UIW_WINDOW("W_ALIAS", defaultStorage)
{
    WBoothsGroup = (UIW_GROUP *) Get("G_BOOTHS");
    UIW_BUTTON * wButton = (UIW_BUTTON *) Get("W_CANCEL");
    if (!select)
    {
        WBoothsGroup->woFlags |= WOF_NON_SELECTABLE;
        wButton->woFlags |= WOF_NON_SELECTABLE;
    }
    windowManager->Center(this);
    helpContext = H_ALIAS;
}

EVENT_TYPE UIW_ALIAS::Event(const UI_EVENT &event)
{
    int i;
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case S_CREATE:
        {
            char s[3];
            int integer = 13;
            UIW_WINDOW::Event(event);
            for (i = 0; i < MAX_BOOTH; i++)
            {
                WPrompts[i] =
                    new UIW_PROMPT(20*(i/CLUSTER_SIZE)+1, i%CLUSTER_SIZE+1, 5 , itoa(i+1, s, 10));
                WNames[i] =
					new UIW_STRING(20*(i/CLUSTER_SIZE)+4, i%CLUSTER_SIZE+1, 16, g_cfg->BoothInfo[i].Name);
                WNames[i]->Information(SET_TEXT_LENGTH, &integer);
                *WBoothsGroup
                + WPrompts[i]
                + WNames[i]
                ;
            }
            break;
        }
    case UE_ACCEPT:
        for (i = 0; i < MAX_BOOTH; i++)
			strcpy(g_cfg->BoothInfo[i].Name, WNames[i]->DataGet());
		g_cfg->Save(NULL, FALSE);
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
//								UIW_CASH
// --------------------------------------------------------------------------

UIW_CASH::UIW_CASH(void) : UIW_WINDOW("W_CASH", defaultStorage)
{
    UIW_BUTTON *wButton;
	if      (!strcmp(g_cfg->CASH, "prn"))
        wButton = (UIW_BUTTON *) Get("W_PRN");
	else if (!strcmp(g_cfg->CASH, "com1"))
        wButton = (UIW_BUTTON *) Get("W_COM1");
	else if (!strcmp(g_cfg->CASH, "com2"))
        wButton = (UIW_BUTTON *) Get("W_COM2");
    else
        wButton = (UIW_BUTTON *) Get("W_ST");
    wButton->Event(S_CURRENT);
    windowManager->Center(this);
    helpContext = H_CASH;
}

EVENT_TYPE UIW_CASH::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_ACCEPT:
        UIW_BUTTON *wButton;
        wButton = (UIW_BUTTON *)  Get("W_PRN");
        if (FlagSet(wButton->woStatus, WOS_SELECTED))
			strcpy(g_cfg->CASH, "prn");
        wButton = (UIW_BUTTON *) Get("W_COM1");
        if (FlagSet(wButton->woStatus, WOS_SELECTED))
			strcpy(g_cfg->CASH, "com1");
        wButton = (UIW_BUTTON *) Get("W_COM2");
        if (FlagSet(wButton->woStatus, WOS_SELECTED))
			strcpy(g_cfg->CASH, "com2");
        wButton = (UIW_BUTTON *) Get("W_ST");
        if (FlagSet(wButton->woStatus, WOS_SELECTED))
			strcpy(g_cfg->CASH, "st");
        eventManager->Put(UI_EVENT(S_CLOSE,0));
		g_cfg->Save();
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
//								UIW_DISPLAY
// --------------------------------------------------------------------------

UIW_DISPLAY::UIW_DISPLAY(void) : UIW_WINDOW("DISPLAY", defaultStorage)
{
    UIW_BUTTON *wBtn;
    char *btnStr;
    // default message
	((UIW_STRING *)Get(SID_MESSAGE))->DataSet(g_cfg->DISPLAY_DEFAULT_MESSAGE);
    // COM
    btnStr = "COM2";
	switch (g_cfg->DISPLAY_COM)
    {
    case SERIAL::COM1:
        btnStr = "COM1";
        break;
    case SERIAL::COM2:
        btnStr = "COM2";
        break;
    }
    wBtn = (UIW_BUTTON *)Get(btnStr);
    wBtn->woStatus |= WOS_SELECTED;
    wBtn->Event(S_CURRENT);
    // BAUDS
    btnStr = "9600";
	switch (g_cfg->DISPLAY_BAUDS)
    {
    case  9600L:
        btnStr =  "9600";
        break;
    case  19200:
        btnStr = "19200";
        break;
    }
    wBtn = (UIW_BUTTON *)Get(btnStr);
    wBtn->woStatus |= WOS_SELECTED;
    wBtn->Event(S_CURRENT);
    // enable flag
    wBtn = (UIW_BUTTON *)Get(SID_ENABLE);
	if (g_cfg->DISPLAY_ENABLE)
        wBtn->woStatus |= WOS_SELECTED;
    else
        wBtn->woStatus &= ~WOS_SELECTED;
    //
    windowManager->Center(this);
    // helpContext = H_DISPLAY;
}



EVENT_TYPE UIW_DISPLAY::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_ACCEPT:
        {
            // default message
			strcpy(g_cfg->DISPLAY_DEFAULT_MESSAGE, ((UIW_STRING *)Get(SID_MESSAGE))->DataGet());
            // COM
            if (FlagSet(((UIW_BUTTON *)Get("COM1"))->woStatus, WOS_SELECTED))
				g_cfg->DISPLAY_COM = SERIAL::COM1;
            else if (FlagSet(((UIW_BUTTON *)Get("COM2"))->woStatus, WOS_SELECTED))
				g_cfg->DISPLAY_COM = SERIAL::COM2;
            // BAUDS
            if (FlagSet(((UIW_BUTTON *)Get("9600"))->woStatus, WOS_SELECTED))
				g_cfg->DISPLAY_BAUDS = 9600;
            else if (FlagSet(((UIW_BUTTON *)Get("19200"))->woStatus, WOS_SELECTED))
				g_cfg->DISPLAY_BAUDS = 19200;
            //
            eventManager->Put(UI_EVENT(S_CLOSE,0));
			g_cfg->Save();
            // notyfy to controller
			if (g_cfg->DISPLAY_ENABLE)
            {
                UI_EVENT tmpEvent;
                tmpEvent.type = E_CONTROLLER;
                tmpEvent.rawCode = UE_DISPLAY;
                eventManager->Event(tmpEvent, E_CONTROLLER);
            }
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
//								UIW_ROUND
// --------------------------------------------------------------------------

UIW_ROUND::UIW_ROUND(void) : UIW_WINDOW("W_ROUND", defaultStorage)
{
    UIW_BUTTON *wButton;
    windowManager->Center(this);
	helpContext = H_ROUND;

	// set default
	char strRound[0x10];
	strcpy(strRound, "W_");
  if (g_cfg->M_ROUND == 0.01)
    strcat(strRound, "001");
  else if (g_cfg->M_ROUND == 0.05)
    strcat(strRound, "005");
  else if (g_cfg->M_ROUND == 0.10)
		strcat(strRound, "010");
	else if (g_cfg->M_ROUND == 0.20)
		strcat(strRound, "020");
	else if (g_cfg->M_ROUND == 0.25)
		strcat(strRound, "025");
	else if (g_cfg->M_ROUND == 0.50)
		strcat(strRound, "050");
	else if (g_cfg->M_ROUND == 1.0)
		strcat(strRound, "1");
	else if (g_cfg->M_ROUND == 2.0)
		strcat(strRound, "2");
	else if (g_cfg->M_ROUND == 5.0)
		strcat(strRound, "5");
	else if (g_cfg->M_ROUND == 10.0)
		strcat(strRound, "10");
	else if (g_cfg->M_ROUND == 20.0)
		strcat(strRound, "20");
	else if (g_cfg->M_ROUND == 50.0)
		strcat(strRound, "50");
	else if (g_cfg->M_ROUND == 100.0)
		strcat(strRound, "100");
	else if (g_cfg->M_ROUND == 200.0)
		strcat(strRound, "200");
	else if (g_cfg->M_ROUND == 500.0)
		strcat(strRound, "500");
	else if (g_cfg->M_ROUND == 1000.0)
		strcat(strRound, "1000");
	else
		strcat(strRound, "5");

	wButton = (UIW_BUTTON*) this->Information(GET_STRINGID_OBJECT, strRound);
	wButton->Event(S_CURRENT);
}

EVENT_TYPE UIW_ROUND::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case UE_ACCEPT:
		UIW_BUTTON *wButton;
    wButton = (UIW_BUTTON*) Get("W_001");
		if (FlagSet(wButton->woStatus, WOS_SELECTED))
	  g_cfg->M_ROUND = 0.01;
    wButton = (UIW_BUTTON*) Get("W_005");
		if (FlagSet(wButton->woStatus, WOS_SELECTED))
	  g_cfg->M_ROUND = 0.05;
    wButton = (UIW_BUTTON*) Get("W_010");
		if (FlagSet(wButton->woStatus, WOS_SELECTED))
			g_cfg->M_ROUND = 0.10;
		wButton = (UIW_BUTTON*) Get("W_020");
		if (FlagSet(wButton->woStatus, WOS_SELECTED))
			g_cfg->M_ROUND = 0.20;
		wButton = (UIW_BUTTON*) Get("W_025");
		if (FlagSet(wButton->woStatus, WOS_SELECTED))
			g_cfg->M_ROUND = 0.25;
		wButton = (UIW_BUTTON*) Get("W_050");
		if (FlagSet(wButton->woStatus, WOS_SELECTED))
			g_cfg->M_ROUND = 0.50;
		wButton = (UIW_BUTTON*) Get("W_1");
		if (FlagSet(wButton->woStatus, WOS_SELECTED))
			g_cfg->M_ROUND = 1.0;
		wButton = (UIW_BUTTON*) Get("W_2");
		if (FlagSet(wButton->woStatus, WOS_SELECTED))
			g_cfg->M_ROUND = 2.0;
		wButton = (UIW_BUTTON*) Get("W_5");
		if (FlagSet(wButton->woStatus, WOS_SELECTED))
			g_cfg->M_ROUND = 5.0;
		wButton = (UIW_BUTTON*) Get("W_10");
		if (FlagSet(wButton->woStatus, WOS_SELECTED))
			g_cfg->M_ROUND = 10.0;
		wButton = (UIW_BUTTON*) Get("W_20");
		if (FlagSet(wButton->woStatus, WOS_SELECTED))
			g_cfg->M_ROUND = 20.0;
		wButton = (UIW_BUTTON*) Get("W_50");
		if (FlagSet(wButton->woStatus, WOS_SELECTED))
			g_cfg->M_ROUND = 50.0;
		wButton = (UIW_BUTTON*) Get("W_100");
		if (FlagSet(wButton->woStatus, WOS_SELECTED))
			g_cfg->M_ROUND = 100.0;
		wButton = (UIW_BUTTON*) Get("W_200");
		if (FlagSet(wButton->woStatus, WOS_SELECTED))
			g_cfg->M_ROUND = 200.0;
		wButton = (UIW_BUTTON*) Get("W_500");
		if (FlagSet(wButton->woStatus, WOS_SELECTED))
			g_cfg->M_ROUND = 500.0;
		wButton = (UIW_BUTTON*) Get("W_1000");
		if (FlagSet(wButton->woStatus, WOS_SELECTED))
			g_cfg->M_ROUND = 1000.0;
		//
		g_cfg->Save();
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
//								UIW_S_PORT
// --------------------------------------------------------------------------

UIW_S_PORT::UIW_S_PORT(void) : UIW_WINDOW("W_S_PORT", defaultStorage)
{
	// parse line
	UINT port=1, speed=2400, bits=8, stopBits=1;
	STR16 parity = {"none"};
	extern char *_COM_FMT;
	sscanf(g_cfg->COM, _COM_FMT, &port, &speed, &bits, parity, &stopBits);
	UIW_BUTTON *wButton;
	if (port == 1)
		wButton = (UIW_BUTTON *) Get("W_COM1");
    else
        wButton = (UIW_BUTTON *) Get("W_COM2");
    wButton->woStatus |= WOS_SELECTED|WOS_CURRENT;
    switch (speed)
    {
    case 1200 :
        wButton = (UIW_BUTTON *) Get("W_1200");
        break;
    case 2400 :
        wButton = (UIW_BUTTON *) Get("W_2400");
        break;
    case 4800 :
        wButton = (UIW_BUTTON *) Get("W_4800");
        break;
    case 9600 :
        wButton = (UIW_BUTTON *) Get("W_9600");
        break;
    case 19200:
        wButton = (UIW_BUTTON *) Get("W_19200");
        break;
    default   :
        wButton = (UIW_BUTTON *) Get("W_2400");
    }
    wButton->woStatus |= (WOS_SELECTED|WOS_CURRENT);
    if (!strcmp(parity, "even"))
        wButton = (UIW_BUTTON *) Get("W_EVEN");
    else if (!strcmp(parity, "odd"))
        wButton = (UIW_BUTTON *) Get("W_ODD");
    else
        wButton = (UIW_BUTTON *) Get("W_NONE");
    wButton->woStatus |= (WOS_SELECTED|WOS_CURRENT);
    if (bits == 7)
        wButton = (UIW_BUTTON *) Get("W_7");
    else
        wButton = (UIW_BUTTON *) Get("W_8");
    wButton->woStatus |= (WOS_SELECTED|WOS_CURRENT);
    if (stopBits == 1)
        wButton = (UIW_BUTTON *) Get("W_1");
    else
        wButton = (UIW_BUTTON *) Get("W_2");
    wButton->woStatus |= (WOS_SELECTED|WOS_CURRENT);
    //
    windowManager->Center(this);
    helpContext = H_S_PORT;
}

EVENT_TYPE UIW_S_PORT::Event(const UI_EVENT &event)
{
    UIW_BUTTON *wButton;
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_ACCEPT:
        wButton = (UIW_BUTTON *) Get("W_COM1");
        if (FlagSet(wButton->woStatus, WOS_SELECTED))
			strcpy(g_cfg->COM, "1 ");
        else
			strcpy(g_cfg->COM, "2 ");
        //
        wButton = (UIW_BUTTON *) Get("W_1200");
        if (FlagSet(wButton->woStatus, WOS_SELECTED))
        {
			strcat(g_cfg->COM, "1200 ");
        }
        else
        {
            wButton = (UIW_BUTTON *) Get("W_2400");
            if (FlagSet(wButton->woStatus, WOS_SELECTED))
				strcat(g_cfg->COM, "2400 ");
            else
            {
                wButton = (UIW_BUTTON *) Get("W_4800");
                if (FlagSet(wButton->woStatus, WOS_SELECTED))
					strcat(g_cfg->COM, "4800 ");
                else
                {
                    wButton = (UIW_BUTTON *) Get("W_9600");
                    if (FlagSet(wButton->woStatus, WOS_SELECTED))
						strcat(g_cfg->COM, "9600 ");
                    else
						strcat(g_cfg->COM, "19200 ");
                }
            }
        }
        //
        wButton = (UIW_BUTTON *) Get("W_8");
        if (FlagSet(wButton->woStatus, WOS_SELECTED))
			strcat(g_cfg->COM, "8 ");
        else
			strcat(g_cfg->COM, "7 ");
        //
        wButton = (UIW_BUTTON *) Get("W_NONE");
        if (FlagSet(wButton->woStatus, WOS_SELECTED))
			strcat(g_cfg->COM, "none ");
        else
        {
            wButton = (UIW_BUTTON *) Get("W_EVEN");
            if (FlagSet(wButton->woStatus, WOS_SELECTED))
				strcat(g_cfg->COM, "even ");
            else
				strcat(g_cfg->COM, "odd ");
        }
        //
        wButton = (UIW_BUTTON *) Get("W_1");
        if (FlagSet(wButton->woStatus, WOS_SELECTED))
			strcat(g_cfg->COM, "1");
        else
			strcat(g_cfg->COM, "2");
        //
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
//								UIW_P_PORT
// --------------------------------------------------------------------------

UIW_P_PORT::UIW_P_PORT(void) : UIW_WINDOW("W_P_PORT", defaultStorage)
{
    UIW_BUTTON *wButton;
	if (g_cfg->LPT == 1)
        wButton = (UIW_BUTTON *) Get("W_LPT1");
    else
        wButton = (UIW_BUTTON *) Get("W_LPT2");
    wButton->Event(S_CURRENT);
    windowManager->Center(this);
    helpContext = H_P_PORT;
}

EVENT_TYPE UIW_P_PORT::Event(const UI_EVENT &event)
{
    UIW_BUTTON *wButton;
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_ACCEPT:
        wButton = (UIW_BUTTON *) Get("W_LPT1");
        if (FlagSet(wButton->woStatus, WOS_SELECTED))
			g_cfg->LPT = 1;
        else
			g_cfg->LPT = 2;
		g_cfg->Save();
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
//								UIW_PPORT
// --------------------------------------------------------------------------

UIW_PPORT::UIW_PPORT(void) : UIW_WINDOW("W_PPORT", defaultStorage)
{
    UIW_BUTTON *wButton;
	if (!strcmp(g_cfg->P_PORT, "lpt"))
        wButton = (UIW_BUTTON *) Get("W_P_PORT");
    else
        wButton = (UIW_BUTTON *) Get("W_S_PORT");
    wButton->Event(S_CURRENT);
    windowManager->Center(this);
    helpContext = H_PPORT;
}

EVENT_TYPE UIW_PPORT::Event(const UI_EVENT &event)
{
    UIW_BUTTON *wButton;
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_OPTIONS:
        wButton = (UIW_BUTTON *) Get("W_P_PORT");
        if (FlagSet(wButton->woStatus, WOS_SELECTED))
            (void *)&(*windowManager + new UIW_P_PORT);
        else
            (void *)&(*windowManager + new UIW_S_PORT);
        break;
    case UE_ACCEPT:
        wButton = (UIW_BUTTON *) Get("W_P_PORT");
		g_spooler->UninstallSerial();
        if (FlagSet(wButton->woStatus, WOS_SELECTED))
			strcpy(g_cfg->P_PORT, "lpt");
        else
        {
			strcpy(g_cfg->P_PORT, "com");
            if (!g_spooler->InstallSerial())
                errorSystem->ReportError(windowManager, WOS_NO_STATUS, "No se pudo programar el puerto serial.");
        }
		g_cfg->Save();
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
//								UIW_CHANGE_PASSWD
// --------------------------------------------------------------------------

UIW_CHANGE_PASSWD::UIW_CHANGE_PASSWD(void) : UIW_WINDOW("W_CHANGE_PASSWD", defaultStorage)
{
    windowManager->Center(this);
    helpContext = H_CHANGE_PASSWD;
}

EVENT_TYPE UIW_CHANGE_PASSWD::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_ACCEPT:
        UIW_STRING *wString;
        wString = (UIW_STRING *)Get("S_PASSWD");
        char *oldString = wString->DataGet();
        wString = (UIW_STRING *)Get("S_NEW_PASSWD");
        char *newString = wString->DataGet();
		if (!g_cfg->ChangePassword(oldString, newString))
            errorSystem->ReportError(windowManager, WOS_NO_STATUS, "Código de acceso inválido\n");
        else
        {
            g_cfg->Save(NULL, FALSE);
            eventManager->Put(UI_EVENT(S_CLOSE,0));
        }
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

