//
// [ MB_SIMUL.CPP ]
//

#include "stdst.h"

#include <control.h>
#include <rt_eng.h>
#include <menubar.h>
#include <toolbar.h>
#include <events.h>

#ifndef USE_HELP_CONTEXTS
#define USE_HELP_CONTEXTS
#include <help.hpp>
#endif

extern CFG *g_cfg;

const int MAX_SIMULA_CALLS 	= 8;
const int SIMULA_CLUSTER 		= 0; // special case: see MAX_SIMULA_CALLS

UIW_SIMULA::UIW_SIMULA(void) : UIW_WINDOW("W_SIMULA3", defaultStorage)
{
	((UIW_BUTTON *) Get("RECEIPT"))->woStatus |= WOS_SELECTED;
	((UIW_BUTTON *) Get("STATISTICS"))->woStatus |= WOS_SELECTED;
	char strId[20];
	for (int i=0; i < MAX_SIMULA_CALLS; i++)
	{
		// booth
		sprintf(strId, "C%d", i+1);
		((UIW_BUTTON *) Get(strId))->userFunction = ProcessBooth;
		// Prepaid
		sprintf(strId, "P%d", i+1);
		((UIW_BIGNUM *) Get(strId))->DataSet(&UI_BIGNUM(
			CONTROLLER::RTEngineGetPreValue(SIMULA_CLUSTER, i)));
		// call
		sprintf(strId, "L%d", i+1);
		((UIW_BUTTON *) Get(strId))->userFunction = ProcessCall;
	}

	helpContext = H_SIMULA;
}

EVENT_TYPE UIW_SIMULA::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case UE_ACCEPT:
			{
					UI_EVENT tmpEvent;
					// send a message into the queue directed To controller
					tmpEvent.type = E_CONTROLLER;
					// use the raw code To put service
					tmpEvent.rawCode = UE_CANCEL_FROM_UIW_SIMULA;
					eventManager->Event(tmpEvent, E_CONTROLLER);
					//
					this->woAdvancedFlags &= ~WOAF_LOCKED; // disconnect lock !!!
					eventManager->Put(UI_EVENT(S_CLOSE, 0));
					break;
			}
	case UE_SELECT_ALL:
			{
					UI_EVENT tmpEvent;
					char strId[20];
					for (int i=0; i < MAX_SIMULA_CALLS; i++)
					{
							sprintf(strId, "L%d", i+1);
							ProcessCall(Get(strId), tmpEvent, L_SELECT);
					}
					break;
			}
	case UE_VIEW:
			{
					(void *)&(*windowManager + new UIW_DUMP);
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

EVENT_TYPE UIW_SIMULA::ProcessBooth(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
		if (ccode != L_SELECT)
				return ccode;

		if (!g_cfg->MANUAL)
				return ccode;

		int bNum;
		sscanf(object->StringID(), "C%d", &bNum);
		--bNum;
		if (bNum < 0)
		{
			errorSystem->ReportError(windowManager, WOS_NO_STATUS, "Número de cabina incorrecto.");
			return ccode;
		}

		*windowManager + new UIW_MANUAL(bNum);

		return ccode;
}

EVENT_TYPE UIW_SIMULA::ProcessCall (UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode != L_SELECT)
			return ccode;

	int bNum;
	sscanf(object->StringID(), "L%d", &bNum);
	--bNum;
	if (bNum < 0)
	{
		errorSystem->ReportError(windowManager, WOS_NO_STATUS, "Número de cabina incorrecto.");
		return ccode;
	}

	UIW_BUTTON *wObject = (UIW_BUTTON *)object;
	if (!strcmp(wObject->DataGet(), "Llamar"))
	{
		// don't touch a real call v.219
		if (CONTROLLER::RTEngineIsBoothBusy(SIMULA_CLUSTER, bNum))
		{
			errorSystem->ReportError(windowManager,	WOS_NO_STATUS,
				"La cabina tiene llamadas en curso.");
		}
		else
		{
			char strId[20];
			sprintf(strId, "P%d", bNum+1);
			double value;
			((UIW_BIGNUM *) object->parent->Get(strId))->DataGet()->Export(&value);

			if (
				value > 0.0F &&
				CONTROLLER::RTEngineGetNumOfCalls(SIMULA_CLUSTER, bNum) > 0 &&
				CONTROLLER::RTEngineGetPreValue(SIMULA_CLUSTER, bNum) == 0.0F
			)
			{
				errorSystem->ReportError(windowManager,	WOS_NO_STATUS,
					"El prepago para esta cabina ya ha sido usado.");
			}
			else
			{
				SetPrePaid(object, bNum);
				MakeCall(object, bNum);
				SetControls(object, bNum, "Colgar");
			}
		}
	}
	else
	{
		HangCall(object, bNum);
		SetControls(object, bNum, "Llamar");
	}

	return ccode;
}

void UIW_SIMULA::SetControls(
	UI_WINDOW_OBJECT *object,
	int bNum,
	char *newCaption)
{
	((UIW_BUTTON *)object)->DataSet(newCaption);
	BOOL enable = strcmp(newCaption, "Llamar") == 0;

	char strId[20];
	sprintf(strId, "P%d", bNum+1);
	if (enable)
	{
		sprintf(strId, "P%d", bNum+1);
		object->parent->Get(strId)->woFlags &= ~WOF_NON_SELECTABLE;
		sprintf(strId, "T%d", bNum+1);
		object->parent->Get(strId)->woFlags &= ~WOF_NON_SELECTABLE;
	}
	else
	{
		sprintf(strId, "P%d", bNum+1);
		object->parent->Get(strId)->woFlags |= WOF_NON_SELECTABLE;
		sprintf(strId, "T%d", bNum+1);
		object->parent->Get(strId)->woFlags |= WOF_NON_SELECTABLE;
	}
}

void UIW_SIMULA::SetPrePaid(UI_WINDOW_OBJECT *object, int bNum)
{
	char strId[20];
	sprintf(strId, "P%d", bNum+1);
	double value;
	((UIW_BIGNUM *) object->parent->Get(strId))->DataGet()->Export(&value);
	if (CONTROLLER::RTEngineGetNumOfCalls(SIMULA_CLUSTER, bNum) == 0)
	{
		CONTROLLER::RTEngineSetFirstPreValue(SIMULA_CLUSTER, bNum, TRUE);
		CONTROLLER::RTEngineSetPreValue(SIMULA_CLUSTER, bNum, value);
		CONTROLLER::RTEngineSetPrePaid(SIMULA_CLUSTER, bNum, value > 0.0F);
	}

	// send a message into the queue directed to controller
	UI_EVENT tmpEvent;
	tmpEvent.type = E_CONTROLLER;
	tmpEvent.rawCode = UE_INTER;  // use the raw code to put service
	tmpEvent.modifiers = bNum; // use modifiers to put boot number
	eventManager->Event(tmpEvent, E_CONTROLLER);
}

void UIW_SIMULA::MakeCall(UI_WINDOW_OBJECT *object, int bNum)
{
	UI_EVENT tmpEvent;
	// send a message into the queue direct To controller
	tmpEvent.type = E_CONTROLLER;
	// use the raw code To put service
	tmpEvent.rawCode = UE_COM_FROM_UIW_SIMULA;
	// use modifiers To put:
	//	 1. booth number = low byte
	//   2. flags (receipt and accumulate) =  high byte
	tmpEvent.modifiers = bNum;
	UIW_BUTTON *wButton;
	wButton = (UIW_BUTTON *) object->parent->parent->Get("RECEIPT");
	tmpEvent.modifiers |= (FlagSet(wButton->woStatus, WOS_SELECTED))?(CONTROLLER::RECEIPT_ON<<8):0;
	wButton = (UIW_BUTTON *) object->parent->parent->Get("STATISTICS");
	tmpEvent.modifiers |= (FlagSet(wButton->woStatus, WOS_SELECTED))?(CONTROLLER::ACCUM_ON<<8):0;
	// use the data To put the phone number
	char strId[20];
	sprintf(strId, "T%d", bNum+1);
	tmpEvent.data = ((UIW_PHONE *) object->parent->parent->Get(strId))->DataGet();
	eventManager->Event(tmpEvent, E_CONTROLLER);
}

void UIW_SIMULA::HangCall(UI_WINDOW_OBJECT *, int boothCount)
{
	UI_EVENT tmpEvent;
	// send a message into the queue direct To controller
	tmpEvent.type = E_CONTROLLER;
	// use the raw code To put service
	tmpEvent.rawCode = UE_HANG_FROM_UIW_SIMULA;
	// use left To put boot number
	tmpEvent.region.left = boothCount;
	eventManager->Event(tmpEvent, E_CONTROLLER);
}

UIW_DUMP::UIW_DUMP(void) : UIW_WINDOW("DUMP2", defaultStorage)
{
	Do();
	//	windowManager->Center(this);
	helpContext = H_SIMULA;
}

EVENT_TYPE UIW_DUMP::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case UE_ACCEPT:
			{
					this->woAdvancedFlags &= ~WOAF_LOCKED; // disconnect lock !!!
					eventManager->Put(UI_EVENT(S_CLOSE, 0));
					break;
			}
	case UE_REFRESH:
			{
					Do();
					break;
			}
	default:
			ccode = UIW_WINDOW::Event(event);
			break;
	}
	return ccode;
}

void UIW_DUMP::Do(void)
{
	UIW_BUTTON *wButton;
	char strId[20];
	for (int i=0; i < MAX_SIMULA_CALLS; i++)
	{
			// flag
			sprintf(strId, "F%d", i+1);
			wButton = (UIW_BUTTON *) Get(strId);
			if (CONTROLLER::RTEngineGetPrePaid(SIMULA_CLUSTER, i))
					wButton->woStatus |= WOS_SELECTED;
			else
					wButton->woStatus &= ~WOS_SELECTED;
			// value
			sprintf(strId, "V%d", i+1);
			((UIW_BIGNUM *) Get(strId))->DataSet(
				&UI_BIGNUM(CONTROLLER::RTEngineGetPreValue(SIMULA_CLUSTER, i)));
			// time
			sprintf(strId, "T%d", i+1);
			((UIW_BIGNUM *) Get(strId))->DataSet(
				&UI_BIGNUM(floor(((double)(
					CONTROLLER::RTEngineGetPreTime(SIMULA_CLUSTER, i)/1000))/60.0F*10)/10));

			UIW_GROUP *wGroup = (UIW_GROUP *) Get("DATA");
			wGroup->Event(UI_EVENT(S_REDISPLAY, 0));
	}
}
