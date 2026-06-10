//
// [ TB_SP.CPP ]
//

#include "stdst.h"

#include <db_eng.h>
#include <ph_eng.h>
#include <toolbar.h>
#include <events.h>
#include <hb_ids.h>

#include <res.hpp>

#ifndef USE_HELP_CONTEXTS
#define USE_HELP_CONTEXTS
#include <help.hpp>
#endif

#if !defined(__TEST__)
#include <stm2.h>
extern STM2 *g_STM2;
#endif

extern CFG        	*g_cfg;
extern DB_ENGINE  	*g_dbEngine;
extern PH_ENGINE 	*g_phEngine;

// --------------------------------------------------------------------------
//								UIW_SP_SERV
// --------------------------------------------------------------------------

UIW_GROUP     *UIW_SP_SERV::WService   = NULL;
UIW_INTEGER   *UIW_SP_SERV::WInt1      = NULL;
UIW_INTEGER   *UIW_SP_SERV::WInt2      = NULL;
UIW_BIGNUM    *UIW_SP_SERV::WBig1      = NULL;
UIW_BIGNUM    *UIW_SP_SERV::WBig2      = NULL;
UIW_BIGNUM    *UIW_SP_SERV::WBig3      = NULL;
UIW_BIGNUM    *UIW_SP_SERV::WBig4      = NULL;
UIW_STRING    *UIW_SP_SERV::WStr1      = NULL;
UIW_STRING    *UIW_SP_SERV::WStr2      = NULL;
UIW_PHONE     *UIW_SP_SERV::WPhone     = NULL;
UIW_BUTTON    *UIW_SP_SERV::WPrint     = NULL;

static DynamicReceipt s_dynReceipt;


UIW_SP_SERV::UIW_SP_SERV(void) : UIW_WINDOW("SPECIAL_SERVICES", defaultStorage)
{
    // get the address of the window used for displaying each service ...
	WService = (UIW_GROUP  *) Get("W_SERVICE");
	WPrint   = (UIW_BUTTON *) Get("W_PRINT");
    //
    windowManager->Center(this);
    //
    eventManager->Put(UI_EVENT(UE_S_N_TEL,0)); // force the first event ...
    // attach help
    this->helpContext = H_SP_SERV;
}

EVENT_TYPE UIW_SP_SERV::Event(const UI_EVENT &event)
{
	const WOF_FLAGS wofValue = WOF_BORDER | WOF_AUTO_CLEAR | WOF_NON_SELECTABLE;
	const NMF_FLAGS nmfValue = NMF_CURRENCY | NMF_COMMAS | NMF_DECIMAL(2);
	const NMF_FLAGS nmfTime  =  NMF_COMMAS | NMF_DECIMAL(1);
	const WOF_FLAGS wofData  = WOF_BORDER | WOF_AUTO_CLEAR;
    // Switch on the type of event.
    EVENT_TYPE ccode = event.type;
	switch (ccode)
    {
	case UE_S_I_TEL:
    case UE_S_N_TEL:
        {
			WPrint->woFlags |= WOF_NON_SELECTABLE;
            WPrint->Event(UI_EVENT(S_REDISPLAY, 0));
            //
			s_dynReceipt.m_r.Stat.CallAttr = (ccode == UE_S_N_TEL)?~INTERNATIONAL_CALL_MASK:INTERNATIONAL_CALL_MASK;
			s_dynReceipt.m_r.Tag = Receipt::SPECIAL_TEL;
            int  integer = 1;
            char range[0x08], s[0x04];
            strcpy(range, "1..");
			strcat(range, itoa(g_cfg->ACTIVE_CLUSTERS*CLUSTER_SIZE, s, 10));
            s[0] = NULL;

			WService->Destroy();
			WService->Information(SET_TEXT, (ccode == UE_S_N_TEL)?"Telefon�a Nacional":"Telefon�a Internacional");
			*WService
				+ new UIW_PROMPT(1, 1, "   Cabina")
				+ new UIW_PROMPT(1, 2, " Tel�fono")
				+ new UIW_PROMPT(1, 3, " Localidad")
				+ new UIW_PROMPT(1, 4, "   Tiempo")
				+ new UIW_PROMPT(1, 5, "   Tarifa")
				+ new UIW_PROMPT(1, 6, "    Total")
				//
				+ (WInt1  = new UIW_INTEGER(11, 1, 5           , &(integer=1), range, NMF_NO_FLAGS, wofData, processTel))
				+ (WPhone = new UIW_PHONE  (11, 2, sizeof(CITY_NAME)-1+0, s, sizeof(PHONE)-1+1, wofData, processTel))
				+ (WStr1  = new UIW_STRING (11, 3, sizeof(CITY_NAME)-1+6, s, sizeof(CITY_NAME)-1+1, STF_NO_FLAGS, wofData, processTel))
				+ (WBig1  = new UIW_BIGNUM (11, 4, 10          , &UI_BIGNUM(0.0F), "0..999", NMF_DECIMAL(1), wofData, processTel))
			;
			if (ccode == UE_S_I_TEL)
            {
                *WService
                + (WInt2  = new UIW_INTEGER(11, 5, 5           , &(integer=0), "0..19", NMF_NO_FLAGS, wofData, processTel))
                + (WBig2  = new UIW_BIGNUM (11, 6, 14          , &UI_BIGNUM(0.0F), NULL, nmfValue, wofValue))
                ;
            }
            else
            {
				*WService
                + (WInt2  = new UIW_INTEGER(11, 5, 5           , &(integer=0), "0..15", NMF_NO_FLAGS, wofData, processTel))
                + (WBig2  = new UIW_BIGNUM (11, 6, 14          , &UI_BIGNUM(0.0F), NULL, nmfValue, wofValue))
				;
            }
            //
            integer = 3;
            WInt1->Information(SET_TEXT_LENGTH, &integer);
            WInt2->Information(SET_TEXT_LENGTH, &integer);
            integer = 6;
            WBig1->Information(SET_TEXT_LENGTH, &integer);
            integer = 14;
            WBig2->Information(SET_TEXT_LENGTH, &integer);
            //
            WInt1->helpContext  = HB_ENTER_BOOTH;
            WPhone->helpContext = HB_ENTER_TEL;
            WStr1->helpContext  = HB_ENTER_CITY;
            WBig1->helpContext  = HB_ENTER_DAMOUNT;
            WInt2->helpContext  = HB_ENTER_TAR;
            //
			WService->Event(UI_EVENT(S_REDISPLAY, 0));
            break;
        }
	case UE_S_I_FAX:
    case UE_S_N_FAX:
        {
            WPrint->woFlags |= WOF_NON_SELECTABLE;
            WPrint->Event(UI_EVENT(S_REDISPLAY, 0));
            //
			s_dynReceipt.m_r.Tag = Receipt::FAX;
			s_dynReceipt.m_r.Stat.CallAttr = (ccode == UE_S_N_FAX)?~INTERNATIONAL_CALL_MASK:INTERNATIONAL_CALL_MASK;
            int  integer = 1;
            char s[0x04];
            s[0] = NULL;
            WService->Destroy();
            WService->Information(SET_TEXT, (ccode == UE_S_N_FAX)?"Fax Nacional":"Fax Internacional");
            *WService
            + new UIW_PROMPT(1, 1, "Tel�fono")
            + new UIW_PROMPT(1, 2, "Localidad")
            + new UIW_PROMPT(1, 3, " P�ginas")
            + new UIW_PROMPT(1, 4, "  Tarifa")
            + new UIW_PROMPT(1, 5, "   Total")
            //
			+ (WPhone = new UIW_PHONE  (10, 1, sizeof(CITY_NAME)-1+0, s, sizeof(PHONE)-1+1, wofData, processFax))
            + (WStr1  = new UIW_STRING (10, 2, sizeof(CITY_NAME)-1+4, s, sizeof(CITY_NAME)-1+1, STF_NO_FLAGS, wofData, processFax))
            + (WInt1  = new UIW_INTEGER(10, 3, 7           , &(integer=1), "1..99", NMF_NO_FLAGS, wofData, processFax))
            + (WBig1  = new UIW_BIGNUM (10, 4, 14          , &UI_BIGNUM(0.0F), NULL, nmfValue, wofData, processFax))
            + (WBig2  = new UIW_BIGNUM (10, 5, 14          , &UI_BIGNUM(0.0F), NULL, nmfValue, wofValue))
            ;
            //
            integer = 3;
            WInt1->Information(SET_TEXT_LENGTH, &integer);
            integer = 6;
            WBig1->Information(SET_TEXT_LENGTH, &integer);
            WBig2->Information(SET_TEXT_LENGTH, &integer);
            //
            WPhone->helpContext = HB_ENTER_TEL;
            WStr1->helpContext  = HB_ENTER_CITY;
            WInt1->helpContext  = HB_ENTER_IAMOUNT;
            WBig1->helpContext  = HB_ENTER_VALUE;
            //
			WService->Event(UI_EVENT(S_REDISPLAY, 0));
			break;
		}
	case UE_S_N_TELEX:
		{
			WPrint->woFlags |= WOF_NON_SELECTABLE;
			WPrint->Event(UI_EVENT(S_REDISPLAY, 0));
			//
			s_dynReceipt.m_r.Tag = Receipt::TELEX;
			s_dynReceipt.m_r.Stat.CallAttr = ~INTERNATIONAL_CALL_MASK;

			WService->Destroy();
			WService->Information(SET_TEXT, "Internet");

			*WService
				+ new UIW_PROMPT(1, 1, "  Minutos")
				+ new UIW_PROMPT(1, 2, " Cobrados")
				+ new UIW_PROMPT(1, 3, "   Tarifa")
				+ new UIW_PROMPT(1, 4, "    Total")
				//
				+ (WBig1 = new UIW_BIGNUM(11, 1, 14, &UI_BIGNUM(0.0), NULL, nmfTime, wofData, processTelex))
				+ (WBig2 = new UIW_BIGNUM(11, 2, 14, &UI_BIGNUM(g_cfg->INTERNET_ROUND), NULL, nmfTime, wofValue))
				+ (WBig3 = new UIW_BIGNUM(11, 3, 14, &UI_BIGNUM(g_cfg->INTERNET_TARIFF), NULL, nmfValue, wofValue))
				+ (WBig4 = new UIW_BIGNUM(11, 4, 14, &UI_BIGNUM(0.0), NULL, nmfValue, wofValue))
			;
			//
			WBig1->helpContext = HB_ENTER_WORDS;
			//
			WService->Event(UI_EVENT(S_REDISPLAY, 0));
			break;
		}
	case UE_SMCARD:
		{
			WPrint->woFlags |= WOF_NON_SELECTABLE;
			WPrint->Event(UI_EVENT(S_REDISPLAY, 0));
			//
			s_dynReceipt.m_r.Tag = Receipt::CARD;
            int  integer = 1;
            char s[0x10];
            s[0] = NULL;
			WService->Destroy();
            WService->Information(SET_TEXT, "Tarjetas Magn�ticas");
            integer = 3;
            for (int i=0; i<MAX_MAGNETIC_CARDS; i++)
            {
				if (g_cfg->MCARD[i])
                {
					sprintf(s, "%8.2f", g_cfg->MCARD[i]);
                    *WService
                    + new UIW_PROMPT(1, i+1, s)
                    + (WInt1 = new UIW_INTEGER(11, i+1, 5, &(integer=0), "0..20", NMF_NO_FLAGS, wofData, processMCard))
                    ;
                    WInt1->Information(SET_TEXT_LENGTH, &(integer=3));
                    WInt1->helpContext = HB_ENTER_IAMOUNT;
				}
			}
			*WService
			+ new UIW_PROMPT(1, i+1, "    Total")
			+ (WBig1 = new UIW_BIGNUM(11, i+1, 14, &UI_BIGNUM(0.0F), NULL, nmfValue, wofValue))
			;
			//
			WService->Event(UI_EVENT(S_REDISPLAY, 0));
            break;
        }
    case UE_SOTHERS:
        {
            WPrint->woFlags |= WOF_NON_SELECTABLE;
            WPrint->Event(UI_EVENT(S_REDISPLAY, 0));
            //
			s_dynReceipt.m_r.Tag = Receipt::OTHER;
            int  integer = 1;
            char s[0x2];
            s[0] = NULL;
            WService->Destroy();
            WService->Information(SET_TEXT, "Otros");
            *WService
			+ new UIW_PROMPT(1, 1, "   Motivo")
            + new UIW_PROMPT(1, 2, " Cantidad")
            + new UIW_PROMPT(1, 3, " V/Unidad")
            + new UIW_PROMPT(1, 4, "    Total")
			//
            + (WStr1 = new UIW_STRING (11, 1, sizeof(CITY_NAME)-1+6, s, sizeof(CITY_NAME)-1+1, STF_NO_FLAGS, wofData, processOther))
            + (WInt1 = new UIW_INTEGER(11, 2,  5, &(integer=1), "1..999", NMF_NO_FLAGS, wofData, processOther))
			+ (WBig1 = new UIW_BIGNUM (11, 3, 14, &UI_BIGNUM(0.0F), NULL, nmfValue, wofData, processOther))
			+ (WBig2 = new UIW_BIGNUM (11, 4, 14, &UI_BIGNUM(0.0F), NULL, nmfValue, wofValue))
			;
			//
			integer = 4;
			WInt1->Information(SET_TEXT_LENGTH, &integer);
			integer = 8;
			WBig1->Information(SET_TEXT_LENGTH, &integer);
			//
			WStr1->helpContext = HB_ENTER_MOTIF;
			WInt1->helpContext = HB_ENTER_IAMOUNT;
			WBig1->helpContext = HB_ENTER_UV;
			//
			WService->Event(UI_EVENT(S_REDISPLAY, 0));
			break;
		}
		// typical buttons ...
	case UE_PRINT:
		// lookout with the number of receipts !!!
		g_cfg->N_RECEIPT = (g_cfg->N_RECEIPT+1)%BinStorage::MAX_RECEIPTS;
		if (!g_cfg->N_RECEIPT)
			g_cfg->N_RECEIPT++;
		if (!g_cfg->IsDemoMode())
			g_STM2->put(STM2::RECEIPTNUMBER, &g_cfg->N_RECEIPT);
		//
		UI_DATE date;
		UI_TIME time;
		//
		date.Export(&s_dynReceipt.m_r.Date);
		time.Export(&s_dynReceipt.m_r.Time);
		s_dynReceipt.m_r.MagicNumber   = BinStorage::MAGIC_NUMBER;
		s_dynReceipt.m_r.Number        = g_cfg->N_RECEIPT;
		s_dynReceipt.m_r.Stat.Cooked   = TRUE;
		s_dynReceipt.m_r.Stat.Printed  = FALSE;
		s_dynReceipt.m_r.Stat.Archived = FALSE;
		s_dynReceipt.m_r.Stat.Paid     = PAID_CALL;
		s_dynReceipt.m_r.Stat.Manual   = FALSE; // as an auto receipt
		// MetaReceipt members
		s_dynReceipt.Attr_.HeaderOn   = TRUE;
		s_dynReceipt.Attr_.FooterOn   = TRUE;
		s_dynReceipt.Attr_.SummaryOn  = FALSE;
		s_dynReceipt.Attr_.Storable = TRUE;
		s_dynReceipt.Attr_.Countable= TRUE;
		s_dynReceipt.Attr_.Printable= TRUE;
		//
		SetCurrent(Current()->Next());
		Current()->Event(UI_EVENT(S_CURRENT, 0));
		WPrint->woFlags |= WOF_NON_SELECTABLE;
		WPrint->Event(UI_EVENT(S_NON_CURRENT, 0));
		WPrint->Event(UI_EVENT(S_REDISPLAY, 0));
		//
		// send a UE_PRINT message to DControl with the receipt
		// DControl must store and print the receipt. !!!
		// GCC/gcc
		//
		UI_EVENT tmpEvent;
		tmpEvent.type = E_CONTROLLER;
		tmpEvent.rawCode = UE_PRINT_FROM_UIW_SP_SERV;
		tmpEvent.data = &s_dynReceipt;
		eventManager->Event(tmpEvent, E_CONTROLLER);
		break;
	case UE_CANCEL:
		eventManager->Put(UI_EVENT(S_CLOSE,0));
		break;
	default:
		ccode = UIW_WINDOW::Event(event);
		if (Current() == WService)
		{
			UI_EVENT tmpEvent;
			tmpEvent.type = E_CONTROLLER;
			tmpEvent.rawCode = UE_UPDATE_HLP_BAR;
			tmpEvent.data = WService->Current();
			eventManager->Event(tmpEvent, E_CONTROLLER);
		}
		break;
	}

	return ccode;
}

EVENT_TYPE UIW_SP_SERV::processTel(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
    if (ccode != S_NON_CURRENT && ccode != L_SELECT)
		return ccode;

	int i = 0;
	if (object && object->SearchID() == ID_INTEGER)
	{
		if (((UIW_INTEGER *)object)->Validate(TRUE) != NMI_OK)
		{
			((UIW_INTEGER *)object)->DataSet(&(i=1));
			return -1;
		}
	}
	else if (object && object->SearchID() == ID_BIGNUM)
	{
		if (((UIW_BIGNUM *)object)->Validate(TRUE) != NMI_OK)
		{
			((UIW_BIGNUM *)object)->DataSet(&UI_BIGNUM(0.0F));
			return -1;
		}
	}
	// check phone
	char *phone;
	phone = WPhone->DataGet();
	if (object->SearchID() == ID_UIW_PHONE)
	{
		PH_ENGINE::CALL_PARAMETERS parameters;
		if (g_phEngine->GetCallParms((const PHONE&)*phone, parameters))
		{
			if (s_dynReceipt.m_r.Stat.CallAttr & INTERNATIONAL_CALL_MASK)
			{
				if (parameters.Attr & INTERNATIONAL_CALL_MASK)
					s_dynReceipt.m_r.Stat.CallAttr = parameters.Attr;
				else
				{
					errorSystem->ReportError(windowManager, WOS_NO_STATUS, "N�mero no v�lido");
					return -1;
				}
			}
			else
			{
				if (!(parameters.Attr & INTERNATIONAL_CALL_MASK))
					s_dynReceipt.m_r.Stat.CallAttr = parameters.Attr;
				else
				{
					errorSystem->ReportError(windowManager, WOS_NO_STATUS, "N�mero no v�lido");
					return -1;
				}
			}
		}
		else if (strlen(phone))
		{
			errorSystem->ReportError(windowManager, WOS_NO_STATUS, "N�mero no v�lido");
			return -1;
		}
	}
	//
	WORD date = _GetSysDate();
	WORD time = _GetSysTime();
	double elapsedTime ;
	WBig1->DataGet()->Export(&elapsedTime);
	s_dynReceipt.DecTime_ = elapsedTime;
	s_dynReceipt.m_r.ElapsedTime = (long)(elapsedTime*1000*60);
	s_dynReceipt.m_r.Percent = 100; // 100 %
	double total;
	PH_ENGINE::TARIFF_ENTRY entry;
	if (s_dynReceipt.m_r.Stat.CallAttr & INTERNATIONAL_CALL_MASK)
	{
		entry = g_phEngine->GetDDITariff(WInt2->DataGet());
		int schedule = PH_ENGINE::DDI_REDUCED_BORDER;
		if (s_dynReceipt.m_r.Stat.CallAttr != BORDER_CALL)
		{
			schedule = PH_ENGINE::DDI_REDUCED_OTHER;
			if (!stricmp(WStr1->DataGet(), g_cfg->USA))
			{
				schedule = PH_ENGINE::DDI_REDUCED_USA;
				WStr1->DataSet(g_cfg->USA); // ensure a valid name for USA
			}
		 }
		// check min and ceil for each kind of call
		switch (schedule)
		{
		case PH_ENGINE::DDI_REDUCED_BORDER:
			if (elapsedTime < g_cfg->MIN_BORDER)
				elapsedTime = g_cfg->MIN_BORDER;

			elapsedTime = g_Ceil(elapsedTime, g_cfg->CEIL_BORDER);
			break;
		case PH_ENGINE::DDI_REDUCED_USA:
			if (elapsedTime < g_cfg->MIN_USA)
				elapsedTime = g_cfg->MIN_USA;

			elapsedTime = g_Ceil(elapsedTime, g_cfg->CEIL_USA);
			break;
        default:
			if (elapsedTime < g_cfg->MIN_INTER)
				elapsedTime = g_cfg->MIN_INTER;
			elapsedTime = g_Ceil(elapsedTime, g_cfg->CEIL_INTER);
		}
        // check what kind of schedule
        int dayType = PH_ENGINE::DDI_MONDAY_FRIDAY;
		if (_IsSunday(date) || g_cfg->IsHollyday(date))
            dayType = PH_ENGINE::DDI_SUNDAY_HOLLYDAY;
        else if (_IsSaturday(date))
            dayType = PH_ENGINE::DDI_SATURDAY;
		if (g_cfg->APPLY_DDI_SCHEDULE)
			s_dynReceipt.m_r.Percent = g_phEngine->GetDDIPercent(schedule, dayType, time);
    }
    else
	{
		entry = g_phEngine->GetDDNTariff(WInt2->DataGet());
		// check min and ceil for each kind of call
		if (s_dynReceipt.m_r.Stat.CallAttr == CELLULAR_CALL)
		{
			if (elapsedTime < g_cfg->MIN_CELLULAR)
				elapsedTime = g_cfg->MIN_CELLULAR;
			elapsedTime = g_Ceil(elapsedTime, g_cfg->CEIL_CELLULAR);
			// adjust cellular. 2.33
			entry.TaxPercent = g_cfg->CELLULAR_TAX;
		}
		else
		{
			if (elapsedTime < g_cfg->MIN_NAL)
				elapsedTime= g_cfg->MIN_NAL;

			elapsedTime = g_Ceil(elapsedTime, g_cfg->CEIL_NAL);
		}
		// check what kind of schedule
		int dayType = PH_ENGINE::DDN_MONDAY_FRIDAY;
		if (g_cfg->IsHollyday(date))
			dayType = PH_ENGINE::DDN_HOLLYDAY;
        else if (_IsWeekend(date))
            dayType = PH_ENGINE::DDN_WEEKEND;
		if (g_cfg->APPLY_DDN_SCHEDULE)
			s_dynReceipt.m_r.Percent = g_phEngine->GetDDNPercent(dayType, time);
    }
    // --- value per min (change for ST 2.09, no tax)
	s_dynReceipt.m_r.ValuePerMin = entry.Value*(((double)s_dynReceipt.m_r.Percent)/100.0F);
    // --- call value (+tax)
	total = elapsedTime*s_dynReceipt.m_r.ValuePerMin*(1.0F+entry.TaxPercent/100.0F);
    // apply round Ja, Ja ...
	total = g_Round(total, g_cfg->M_ROUND);
	WBig2->DataSet(&UI_BIGNUM(total));
    // fill data
	strcpy(s_dynReceipt.m_r.Phone, WPhone->DataGet());
	strcpy(s_dynReceipt.m_r.City, WStr1->DataGet());
	s_dynReceipt.m_r.BoothNumber = WInt1->DataGet()-1;
	s_dynReceipt.m_r.CeilMin     = elapsedTime;
	s_dynReceipt.m_r.Tariff      = WInt2->DataGet();
	s_dynReceipt.m_r.Value       = total;
    // warning: Tax corresponding to the total !!!
	s_dynReceipt.m_r.Tax = total*(entry.TaxPercent/(100.0F+entry.TaxPercent));
    //
	WPrint->woFlags &= ~WOF_NON_SELECTABLE;
    WPrint->Event(UI_EVENT(S_REDISPLAY, 0));
    //
    return ccode;
}

EVENT_TYPE UIW_SP_SERV::processFax(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode != S_NON_CURRENT && ccode != L_SELECT)
		return ccode;

	int i = 0;
	if (object && object->SearchID() == ID_INTEGER)
	{
		if (((UIW_INTEGER *)object)->Validate(TRUE) != NMI_OK)
		{
			((UIW_INTEGER *)object)->DataSet(&(i=1));
			return -1;
		}
	}
	else if (object && object->SearchID() == ID_BIGNUM)
	{
		if (((UIW_BIGNUM *)object)->Validate(TRUE) != NMI_OK)
		{
			((UIW_BIGNUM *)object)->DataSet(&UI_BIGNUM(0.0F));
			return -1;
		}
	}
	// check phone
	char *phone;
	phone = WPhone->DataGet();
	if (object->SearchID() == ID_UIW_PHONE)
	{
		PH_ENGINE::CALL_PARAMETERS parameters;
		if (g_phEngine->GetCallParms((const PHONE&)*phone, parameters))
		{
			if (s_dynReceipt.m_r.Stat.CallAttr & INTERNATIONAL_CALL_MASK)
			{
				if (parameters.Attr & INTERNATIONAL_CALL_MASK)
					s_dynReceipt.m_r.Stat.CallAttr = parameters.Attr;
				else
				{
					errorSystem->ReportError(windowManager, WOS_NO_STATUS, "N�mero no v�lido");
					return -1;
				}
			}
			else
			{
				if (!(parameters.Attr & INTERNATIONAL_CALL_MASK))
					s_dynReceipt.m_r.Stat.CallAttr = parameters.Attr;
				else
				{
					errorSystem->ReportError(windowManager, WOS_NO_STATUS, "N�mero no v�lido");
					return -1;
				}
			}
		}
		else if (strlen(phone))
		{
			errorSystem->ReportError(windowManager, WOS_NO_STATUS, "N�mero no v�lido");
			return -1;
		}
	}
	//
	strcpy(s_dynReceipt.m_r.Phone, WPhone->DataGet());
	strcpy(s_dynReceipt.m_r.City, WStr1->DataGet());
	s_dynReceipt.m_r.Amount = WInt1->DataGet();
	WBig1->DataGet()->Export(&s_dynReceipt.m_r.UnitaryValue);
	s_dynReceipt.m_r.Value = s_dynReceipt.m_r.Amount*s_dynReceipt.m_r.UnitaryValue;
	// apply tax (ST 2.09)
	s_dynReceipt.m_r.Value *= (1.0F+(float)g_cfg->TAX_PERCENT/100.0F);
	// apply round Ja, Ja ...
	s_dynReceipt.m_r.Value = g_Round(s_dynReceipt.m_r.Value, g_cfg->M_ROUND);
	s_dynReceipt.m_r.Tax   = s_dynReceipt.m_r.Value*((float)g_cfg->TAX_PERCENT/(100.0F+g_cfg->TAX_PERCENT));
	WBig2->DataSet(&UI_BIGNUM(s_dynReceipt.m_r.Value));
	//
	WPrint->woFlags &= ~WOF_NON_SELECTABLE;
	WPrint->Event(UI_EVENT(S_REDISPLAY, 0));
	//
	return ccode;
}

EVENT_TYPE UIW_SP_SERV::processTelex(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode != S_NON_CURRENT && ccode != L_SELECT)
		return ccode;

	// verify data
	if (object && object->SearchID() == ID_BIGNUM)
	{
		if (((UIW_BIGNUM *)object)->Validate(TRUE) != NMI_OK)
		{
			((UIW_BIGNUM *)object)->DataSet(&UI_BIGNUM(0.0F));
			return -1;
		}
	}

	WBig1->DataGet()->Export(&s_dynReceipt.m_r.Minutes);

	double units = ceil(s_dynReceipt.m_r.Minutes/g_cfg->INTERNET_ROUND);

	s_dynReceipt.m_r.CeilMin = units*g_cfg->INTERNET_ROUND;
	WBig2->DataSet(&UI_BIGNUM(s_dynReceipt.m_r.CeilMin));

	s_dynReceipt.m_r.UnitaryValue = g_cfg->INTERNET_TARIFF;

	s_dynReceipt.m_r.Value = units * s_dynReceipt.m_r.UnitaryValue;

	// apply tax
	s_dynReceipt.m_r.Value *= (1.0F+g_cfg->INTERNET_TAX/100.0F);

	// apply round Ja, Ja ...
	s_dynReceipt.m_r.Value = g_Round(s_dynReceipt.m_r.Value, g_cfg->M_ROUND);

	s_dynReceipt.m_r.Tax   = s_dynReceipt.m_r.Value*(g_cfg->INTERNET_TAX/(100.0F+g_cfg->INTERNET_TAX));

	WBig4->DataSet(&UI_BIGNUM(s_dynReceipt.m_r.Value));

	WPrint->woFlags &= ~WOF_NON_SELECTABLE;
	WPrint->Event(UI_EVENT(S_REDISPLAY, 0));

	return ccode;
}

EVENT_TYPE UIW_SP_SERV::processMCard(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode != S_NON_CURRENT && ccode != L_SELECT)
		return ccode;
	// read values
	int i=0;
	if (object && object->SearchID() == ID_INTEGER)
	{
		if (((UIW_INTEGER *)object)->Validate(TRUE) != NMI_OK)
		{
			((UIW_INTEGER *)object)->DataSet(&(i=1));
            return -1;
        }
    }
	s_dynReceipt.m_r.Value = 0;
    UI_WINDOW_OBJECT *tmpObject = WService->First();
    while (i < MAX_MAGNETIC_CARDS)
    {
        if (tmpObject && tmpObject->SearchID() == ID_INTEGER)
        {
			s_dynReceipt.m_r.Cards[i] = ((UIW_INTEGER *)(tmpObject))->DataGet();
			s_dynReceipt.m_r.Value += g_cfg->MCARD[i]*s_dynReceipt.m_r.Cards[i];
            i++;
        }
        tmpObject = tmpObject->Next();
    }
	s_dynReceipt.m_r.Tax = s_dynReceipt.m_r.Value*((float)g_cfg->TAX_PERCENT/(100.0F+g_cfg->TAX_PERCENT));
	WBig1->DataSet(&UI_BIGNUM(s_dynReceipt.m_r.Value));
    //
    WPrint->woFlags &= ~WOF_NON_SELECTABLE;
    WPrint->Event(UI_EVENT(S_REDISPLAY, 0));
    //
    return ccode;
}

EVENT_TYPE UIW_SP_SERV::processOther(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
    if (ccode != S_NON_CURRENT && ccode != L_SELECT)
        return ccode;
    if (object && object->SearchID() == ID_BIGNUM)
    {
        if (((UIW_BIGNUM *)object)->Validate(TRUE) != NMI_OK)
        {
            ((UIW_BIGNUM *)object)->DataSet(&UI_BIGNUM(0.0F));
            return -1;
        }
    }
    if (object && object->SearchID() == ID_INTEGER)
    {
        if (((UIW_INTEGER *)object)->Validate(TRUE) != NMI_OK)
        {
            int integer = 0;
            ((UIW_INTEGER *)object)->DataSet(&integer);
            return -1;
        }
    }
	strcpy(s_dynReceipt.m_r.Motif, WStr1->DataGet());
	s_dynReceipt.m_r.Amount = WInt1->DataGet();
	WBig1->DataGet()->Export(&s_dynReceipt.m_r.UnitaryValue);
	s_dynReceipt.m_r.Value = s_dynReceipt.m_r.Amount*s_dynReceipt.m_r.UnitaryValue;
    // apply tax (ST 2.09)
	s_dynReceipt.m_r.Value *= (1.0F+(float)g_cfg->TAX_PERCENT/100.0F);
    // apply round Ja, Ja ...
	s_dynReceipt.m_r.Value = g_Round(s_dynReceipt.m_r.Value, g_cfg->M_ROUND);
	s_dynReceipt.m_r.Tax = s_dynReceipt.m_r.Value*((float)g_cfg->TAX_PERCENT/(100.0F+g_cfg->TAX_PERCENT));
	WBig2->DataSet(&UI_BIGNUM(s_dynReceipt.m_r.Value));
    //
    WPrint->woFlags &= ~WOF_NON_SELECTABLE;
    WPrint->Event(UI_EVENT(S_REDISPLAY, 0));
    //
    return ccode;
}