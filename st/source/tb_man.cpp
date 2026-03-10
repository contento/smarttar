//
// [ TB_MAN.CPP ]
//

#include "stdst.h"

#include <control.h>
#include <toolbar.h>
#include <events.h>
#include <hb_ids.h>

#ifndef USE_HELP_CONTEXTS
#define USE_HELP_CONTEXTS
#include <help.hpp>
#endif

extern CFG *g_cfg;

// --------------------------------------------------------------------------
//								UIW_MANUAL
// --------------------------------------------------------------------------
//
// Note:
// since the module is equal to the object, then no problem with this static
// variables
//
static int         		s_numOfReceipts  = 0;
static DynamicReceipt 	*s_receipts      = NULL;
static double      		*s_totals        = NULL;
static double 			s_total     = 0.0F;
static double 			s_notPaid   = 0.0F;
static double 			s_tollFree  = 0.0F;
static double 			s_subTotal  = 0.0F;
static double 			s_paid      = 0.0F;
static double 			s_moneyBack = 0.0F;

static UIW_TABLE  *s_wData;
static UIW_STRING **s_wReceipts;
static UIW_BUTTON **s_wNCs    = NULL;
static UIW_BUTTON **s_wPRs    = NULL;

WORD UIW_MANUAL::BoothNum = 0;

//
// Lookout !!!
// At this time the s_receipts can't be saved,
// therefore, we can't use the db system (DBEngine).
//
UIW_MANUAL::UIW_MANUAL(int boothNum) :
        UIW_WINDOW("W_MANUAL", defaultStorage)
{
    BoothNum = boothNum;
	((UIW_STRING *) Get("W_BOOTH"))->DataSet(g_cfg->BoothInfo[boothNum].Name);
	DynamicReceipt dynReceipt;
    UINT numOfObjects;
    //
    // first pass: count items in this booth
    //
	numOfObjects  = CONTROLLER::Receipts->GetCount();
	s_numOfReceipts = 0;
	for (int i=0; i<numOfObjects; i++)
    {
		if (CONTROLLER::Receipts->Get(dynReceipt))
        {
			if
			(
				dynReceipt.m_r.Tag == Receipt::TEL     && // it's a normal rec
				dynReceipt.m_r.BoothNumber == BoothNum &&
				!dynReceipt.Attr_.Printable // avoid reprint
			)
			{
				s_numOfReceipts++;
			}
			CONTROLLER::Receipts->Put(dynReceipt);
        }
        else
            break;
    }
	s_numOfReceipts += 4; // margin to avoid surprises !!!
	s_receipts  = new DynamicReceipt[s_numOfReceipts];
	s_wNCs      = new UIW_BUTTON*[s_numOfReceipts];
	s_wPRs      = new UIW_BUTTON*[s_numOfReceipts];
	s_wReceipts = new UIW_STRING*[s_numOfReceipts];
	s_total     = 0.0F;
	s_notPaid   = 0.0F;
	s_tollFree  = 0.0F;
	s_subTotal  = 0.0F;
	s_paid      = 0.0F;
	s_moneyBack = 0.0F;
	s_totals    = new double[s_numOfReceipts];
	s_numOfReceipts = 0; // recount to get actual value
    //
	// second pass: create list of s_receipts for this booth
    //
	numOfObjects = CONTROLLER::Receipts->GetCount();
    for (i=0; i<numOfObjects; i++)
    {
		if (CONTROLLER::Receipts->Get(dynReceipt))
        {
			if (dynReceipt.m_r.Tag == Receipt::TEL     && // it's a normal rec
				dynReceipt.m_r.BoothNumber == BoothNum &&
				!dynReceipt.Attr_.Printable)          // avoid reprint
			{
				s_receipts[s_numOfReceipts] = dynReceipt;
				s_numOfReceipts++;
				s_paid += dynReceipt.PreValue_; // just processed s_receipts !!!
            }
			CONTROLLER::Receipts->Put(dynReceipt);
		}
		else
			break;
	}
	if (s_paid < 0) // not to allow negative numbers
		s_paid = 0;
	//
	DynamicReceipt::QSort(s_receipts, s_numOfReceipts);
	//
	s_wData = new UIW_TABLE(1, 2, 82, 8, WOF_BORDER);
	s_wData->wnFlags |= WNF_SELECT_MULTIPLE;
	//
	helpContext = H_MANUAL;
	windowManager->Center(this);
}

UIW_MANUAL::~UIW_MANUAL(void)
{
	delete [] s_receipts;
	delete [] s_totals;
}

static BoothDisplay::Info s_boothDisplayInfo;

EVENT_TYPE UIW_MANUAL::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_CREATE:
		{
			UIW_WINDOW::Event(event);
			// create table
			STR256 s;
			DynamicReceipt *dynReceipt;
			for (int i=0; i<s_numOfReceipts; i++)
			{
				dynReceipt = &s_receipts[i];
				sprintf(s,
						"%-8ld %-18s %4.1f     %4.1f     %2d     %12.2f",
						dynReceipt->m_r.Number, dynReceipt->m_r.Phone, g_Milisec2Time(dynReceipt->m_r.ElapsedTime, g_cfg->CORRECTION_TIME),
						dynReceipt->m_r.CeilMin, dynReceipt->m_r.Tariff, dynReceipt->m_r.Value
					   );
				s_totals[i] = dynReceipt->m_r.Value;
				s_wNCs[i] = new UIW_BUTTON(1, i,  3, "", BTF_NO_TOGGLE|BTF_CHECK_BOX|BTF_AUTO_SIZE, WOF_JUSTIFY_CENTER, UIW_MANUAL::processCheck);
				s_wPRs[i] = new UIW_BUTTON(5, i,  3, "", BTF_NO_TOGGLE|BTF_CHECK_BOX|BTF_AUTO_SIZE, WOF_JUSTIFY_CENTER, processCheck);
				s_wReceipts[i] = new UIW_STRING(9, i,  70, s, -1, STF_NO_FLAGS, WOF_NON_SELECTABLE);
				switch (dynReceipt->m_r.Stat.Paid)
				{
				case PAID_CALL:
					s_total += dynReceipt->m_r.Value;
					break;
				case NOT_PAID_CALL:
					s_wNCs[i]->woStatus |= WOS_SELECTED;
					s_notPaid += dynReceipt->m_r.Value;
					break;
				case TOLL_FREE_CALL:
					s_wPRs[i]->woStatus |= WOS_SELECTED;
					s_tollFree += dynReceipt->m_r.Value;
					break;
				}
				*s_wData + s_wNCs[i];
				*s_wData + s_wPRs[i];
				*s_wData + s_wReceipts[i];
			}
			*this + s_wData;
			//
			s_subTotal  = s_total;
			s_moneyBack = s_paid - s_subTotal;
			// put s_boothDisplayInfo
			((UIW_INTEGER *)Get("W_CALLS"))->DataSet(&s_numOfReceipts);
			((UIW_BIGNUM *)Get("W_NO_PAY"))->DataSet(&UI_BIGNUM(s_notPaid));
			((UIW_BIGNUM *)Get("W_TOLL_FREE"))->DataSet(&UI_BIGNUM(s_tollFree));
			((UIW_BIGNUM *)Get("W_SUB_TOTAL"))->DataSet(&UI_BIGNUM(s_subTotal));
			((UIW_BIGNUM *)Get("W_TOTAL"))->DataSet(&UI_BIGNUM(s_total));
			((UIW_BIGNUM *)Get("W_PAID"))->DataSet(&UI_BIGNUM(s_paid));
			((UIW_BIGNUM *)Get("W_PAID"))->userFunction = processPayment;
			((UIW_BIGNUM *)Get("W_BACK"))->DataSet(&UI_BIGNUM(s_moneyBack));
			// update booth display
			UI_EVENT tmpEvent;
			tmpEvent.type = E_CONTROLLER;
			// use the raw code to put service
			tmpEvent.rawCode   = UE_UPDATE_TOTAL_DISPLAY;
			//
			s_boothDisplayInfo.displayNum  = 0;  // always booth 0
			s_boothDisplayInfo.boothNum    = BoothNum + 1;
			s_boothDisplayInfo.elapsedTime = 0;
			s_boothDisplayInfo.cost        = 0.0;
			s_boothDisplayInfo.numOfCalls  = s_numOfReceipts;
			s_boothDisplayInfo.totalCost   = s_total;
			//
			tmpEvent.data = &s_boothDisplayInfo;
			eventManager->Event(tmpEvent, E_CONTROLLER);
			break;
		}
		//
	case UE_PRINT:
		{
			UI_EVENT tmpEvent;
			if (!s_numOfReceipts)
			{ // nothing to do ?
				woAdvancedFlags &= ~WOAF_LOCKED; // disconnect lock !!!
				eventManager->Put(UI_EVENT(S_CLOSE,0));
			}
			else
			{
				// --- change attributtes to let print and accumulate
				int numOfPaidReceipts = 0;
				// --- first pass: qualify s_receipts (PAID etc)
				for (int i=0; i<s_numOfReceipts; i++)
				{
					if (FlagSet(s_wNCs[i]->woStatus, WOS_SELECTED))
						s_receipts[i].m_r.Stat.Paid = NOT_PAID_CALL;
					else if (FlagSet(s_wPRs[i]->woStatus, WOS_SELECTED))
						s_receipts[i].m_r.Stat.Paid = TOLL_FREE_CALL;
					else
						numOfPaidReceipts++;
				}
				//
				DynamicReceipt dynReceipt;
				DynamicReceipt *match;
				// --- second pass: extract from s_receipts to solve problem of sorting JEAM/GCC.
				UINT numOfObjects = CONTROLLER::Receipts->GetCount();
				for (i=0; i<numOfObjects; i++)
				{
					CONTROLLER::Receipts->Get(dynReceipt);
					match = DynamicReceipt::BSearch(dynReceipt.m_r.Number, s_receipts, s_numOfReceipts);
					if (match)
						match->Attr_.Storable = dynReceipt.Attr_.Storable; // save the status
					else
						CONTROLLER::Receipts->Put(dynReceipt);
				}
				// --- third pass: process and re-insert s_paid recs into Engine
				int receiptNum = 0;
				for (i=0; i<s_numOfReceipts; i++)
				{
					if (s_receipts[i].m_r.Stat.Paid == PAID_CALL)
					{
						s_receipts[i].Attr_.HeaderOn  = FALSE;
						s_receipts[i].Attr_.FooterOn  = FALSE;
						s_receipts[i].Attr_.SummaryOn = FALSE;
						s_receipts[i].Attr_.Countable = TRUE; // now count ...
						s_receipts[i].Attr_.Printable = TRUE; // & print
						receiptNum++;
						if (numOfPaidReceipts > 1)
						{
							if (receiptNum == 1)
								s_receipts[i].Attr_.HeaderOn = TRUE;
							else if (receiptNum == numOfPaidReceipts)
							{
								s_receipts[i].Attr_.FooterOn   = TRUE;
								s_receipts[i].Attr_.SummaryOn  = TRUE;
								s_receipts[i].NumOfCalls_ = numOfPaidReceipts;
								s_receipts[i].Total_      = s_subTotal;
								s_receipts[i].MoneyBack_  = (s_moneyBack < 0)?0:s_moneyBack;
							}
						}
						else
						{
							s_receipts[i].Attr_.HeaderOn = TRUE;
							s_receipts[i].Attr_.FooterOn = TRUE;
							s_receipts[i].MoneyBack_     = (s_moneyBack < 0)?0:s_moneyBack;
						}
						CONTROLLER::Receipts->Put(s_receipts[i]);
					}
				}
				// --- fourth pass: re-insert not s_paid and toll free recs into Engine
				for (i=0; i<s_numOfReceipts; i++)
				{
					if (s_receipts[i].m_r.Stat.Paid != PAID_CALL)
					{
						s_receipts[i].Attr_.Countable = TRUE; // now count ...
						s_receipts[i].Attr_.Printable = TRUE; // & print
						s_receipts[i].Attr_.HeaderOn  = TRUE;
						s_receipts[i].Attr_.FooterOn  = TRUE;
						s_receipts[i].Attr_.SummaryOn = FALSE;
						s_receipts[i].PreValue_ = 0;
						CONTROLLER::Receipts->Put(s_receipts[i]);
					}
				}
				//
				woAdvancedFlags &= ~WOAF_LOCKED; // disconnect lock !!!
				eventManager->Put(UI_EVENT(S_CLOSE,0));
				// send a UE_PRINT message to DControl with the booth number
				// DControl must generate the receipt and set the print flag. !!! GCC/gcc
				tmpEvent.type = E_CONTROLLER;
				// use the raw code to put service
				tmpEvent.rawCode = UE_PRINT_FROM_UIW_MANUAL;
				tmpEvent.region.left = BoothNum;
				eventManager->Event(tmpEvent, E_CONTROLLER);
				// reset manual s_boothDisplayInfo, we assume was printed
				memset(&CONTROLLER::manualInfo[BoothNum], 0, sizeof(CONTROLLER::ManualInfoItem));
			}
			// update booth display
			tmpEvent.type       = E_CONTROLLER;
			tmpEvent.rawCode    = UE_UPDATE_TOTAL_DISPLAY;
			//
			s_boothDisplayInfo.displayNum     = 0;  // always booth 0
			s_boothDisplayInfo.boothNum       = 0;
			s_boothDisplayInfo.elapsedTime    = 0;
			s_boothDisplayInfo.cost           = 0;
			s_boothDisplayInfo.numOfCalls     = 0;
			s_boothDisplayInfo.totalCost      = 0.0;
			//
			tmpEvent.data       = &s_boothDisplayInfo;
			eventManager->Event(tmpEvent, E_CONTROLLER);
			//
			break;
		}
	case UE_CANCEL:
		woAdvancedFlags &= ~WOAF_LOCKED; // disconnect lock !!!
		eventManager->Put(UI_EVENT(S_CLOSE,0));
		// update booth display
		UI_EVENT tmpEvent;
		tmpEvent.type = E_CONTROLLER;
		// use the raw code to put service
		tmpEvent.rawCode   = UE_UPDATE_TOTAL_DISPLAY;
		//
		s_boothDisplayInfo.displayNum   = 0;  // always booth 0
		s_boothDisplayInfo.boothNum     = 0;
		s_boothDisplayInfo.elapsedTime  = 0;
		s_boothDisplayInfo.cost         = 0;
		s_boothDisplayInfo.numOfCalls   = 0;
		s_boothDisplayInfo.totalCost    = 0;
		tmpEvent.data     = &s_boothDisplayInfo;
		//
		eventManager->Event(tmpEvent, E_CONTROLLER);
		break;
	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}
	return ccode;
}

EVENT_TYPE UIW_MANUAL::processCheck(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode != S_NON_CURRENT && ccode != L_SELECT)
		return ccode;
	// --- Localize the object to see if it belongs to NCs or PRs
	int i;
	for (i=0; i<s_numOfReceipts; i++)
	{
		// try to toggle the other button (exclusive)
		if (s_wNCs[i] == (UIW_BUTTON *)object)
		{
			if (FlagSet(s_wNCs[i]->woStatus, WOS_SELECTED) && FlagSet(s_wPRs[i]->woStatus, WOS_SELECTED))
			{
				s_wPRs[i]->woStatus &= ~WOS_SELECTED;
				s_wPRs[i]->Event(UI_EVENT(S_REDISPLAY, 0));
			}
        }
		else if (s_wPRs[i] == (UIW_BUTTON *)object)
        {
			if (FlagSet(s_wNCs[i]->woStatus, WOS_SELECTED) && FlagSet(s_wPRs[i]->woStatus, WOS_SELECTED))
            {
				s_wNCs[i]->woStatus &= ~WOS_SELECTED;
				s_wNCs[i]->Event(UI_EVENT(S_REDISPLAY, 0));
            }
        }
    }
    // --- recalc fields
	s_notPaid    = 0;
	s_tollFree = 0;
    WORD numOfTollFree = 0,	numOfNotPaid = 0;
	for (i=0; i<s_numOfReceipts; i++)
    {
        if (FlagSet(s_wNCs[i]->woStatus, WOS_SELECTED))
        {
			s_notPaid += s_totals[i];
			numOfNotPaid++;
        }
        if (FlagSet(s_wPRs[i]->woStatus, WOS_SELECTED))
        {
			s_tollFree += s_totals[i];
            numOfTollFree++;
        }
    }
	s_subTotal = s_total - s_notPaid - s_tollFree;
    UIW_BIGNUM *wBignum;
    UI_WINDOW_OBJECT *parent = object->parent->parent;
    wBignum = (UIW_BIGNUM *) parent->Get("W_PAID");
	wBignum->DataGet()->Export(&s_paid);
	s_moneyBack = s_paid - s_subTotal;
    wBignum = (UIW_BIGNUM *) parent->Get("W_NO_PAY");
    wBignum->DataSet(&UI_BIGNUM(s_notPaid));
    wBignum->Event(UI_EVENT(S_REDISPLAY, 0));
    wBignum = (UIW_BIGNUM *) parent->Get("W_TOLL_FREE");
    wBignum->DataSet(&UI_BIGNUM(s_tollFree));
    wBignum->Event(UI_EVENT(S_REDISPLAY, 0));
	wBignum = (UIW_BIGNUM *) parent->Get("W_SUB_TOTAL");
	wBignum->DataSet(&UI_BIGNUM(s_subTotal));
    wBignum->Event(UI_EVENT(S_REDISPLAY, 0));
    wBignum = (UIW_BIGNUM *) parent->Get("W_BACK");
	wBignum->DataSet(&UI_BIGNUM(s_moneyBack));
    wBignum->Event(UI_EVENT(S_REDISPLAY, 0));
    // update booth display
    UI_EVENT tmpEvent;
    tmpEvent.type = E_CONTROLLER;
    // use the raw code to put service
    tmpEvent.rawCode   = UE_UPDATE_TOTAL_DISPLAY;
    //
	s_boothDisplayInfo.displayNum  = 0;  // always booth 0
	s_boothDisplayInfo.boothNum    = BoothNum + 1;
	s_boothDisplayInfo.elapsedTime = 0;
	s_boothDisplayInfo.cost        = 0.0;
	s_boothDisplayInfo.numOfCalls  = s_numOfReceipts - numOfNotPaid - numOfTollFree;
	s_boothDisplayInfo.totalCost   = s_subTotal;
    //
	tmpEvent.data       = &s_boothDisplayInfo;
	eventManager->Event(tmpEvent, E_CONTROLLER);
    //
    return ccode;
}

EVENT_TYPE UIW_MANUAL::processPayment(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
    if (ccode != S_NON_CURRENT && ccode != L_SELECT)
        return ccode;
    //
    UIW_BIGNUM *wBignum;
    UI_WINDOW_OBJECT *parent = object->parent;
    wBignum = (UIW_BIGNUM *) parent->Get("W_PAID");
	wBignum->DataGet()->Export(&s_paid);
	s_moneyBack = s_paid - s_subTotal;
    wBignum = (UIW_BIGNUM *) parent->Get("W_BACK");
    wBignum->DataSet(&UI_BIGNUM(s_moneyBack));
    wBignum->Event(UI_EVENT(S_REDISPLAY, 0));
    //
    return ccode;
}