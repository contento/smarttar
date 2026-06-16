//
// [ DB_VIEW.CPP ]
//

#include "stdst.h"

#include <db_eng.h>

#if !defined(__TEST__)
#include <toolbar.h>
#include <menubar.h>
#endif // !defined(__TEST__)

#include <db_view.h>
#include <events.h>

#ifndef USE_HELP_CONTEXTS
#define USE_HELP_CONTEXTS
#include <help.hpp>
#endif // ifndef USE_HELP_CONTEXTS

extern DB_ENGINE 	*g_dbEngine;
extern CFG       	*g_cfg;

DBView::DBView(BOOL bFromTurn):
	UIW_WINDOW("DBVIEW", defaultStorage),
	m_bFromTurn(bFromTurn),
	m_bArchive(FALSE),
	m_pwTurn(NULL),
	m_pwDate(NULL),
	m_pwNumbers(NULL),
	m_pwNumber(NULL)
{
	windowManager->Center(this);
	//helpContext = H_DBVIEW;
}

DBView::~DBView()
{
	if (m_bArchive)
		g_dbEngine->UnloadArcDB();
}

EVENT_TYPE DBView::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_CREATE:
		{
			UIW_WINDOW::Event(event);

			m_pwTurn = (UIW_INTEGER *) Get("TURN");
			m_pwTurn->userFunction = ProcessTurn;
			if (!m_bFromTurn)
				m_pwTurn->woFlags &= ~WOF_NON_SELECTABLE;

			m_pwDate = (UIW_DATE *) Get("DATE");
			m_pwDate->userFunction = ProcessTurn;
			if (!m_bFromTurn)
			{
				m_pwDate->woFlags &= ~WOF_NON_SELECTABLE;
			}

			int turn = 1;
			if (m_bFromTurn)
			{
				turn = g_cfg->TURN_NUMBER;
			}
			m_pwTurn->DataSet(&turn);

			m_pwNumber = (UIW_BIGNUM *) Get("NUMBER");
			m_pwNumber->userFunction = ProcessNumber;

			m_pwNumbers = (UIW_VT_LIST *) Get("NUMBERS");
			m_pwNumbers->woFlags |= WOF_VIEW_ONLY;

			eventManager->Put(UI_EVENT(UE_VIEW, 0)); // init ...

			break;
		}
#if !defined(__TEST__)
#if !defined(__UTIL__)
    case UE_PRINT:
	{
		UIW_STRING *pwCurrent = (UIW_STRING *)m_pwNumbers->Current();
		if (!pwCurrent)
			break;

		long number = atol(pwCurrent->DataGet());

		BOOL found = FALSE;

		DynamicReceipt dynReceipt;
		dynReceipt.bFromTurn = m_bFromTurn;
		if (m_bFromTurn)
			found = g_dbEngine->Get(dynReceipt.m_r, number);
		else if (m_bArchive)
			found = g_dbEngine->GetArc(dynReceipt.m_r, number);

		if (!found)
			break;

		static UI_EVENT tmpEvent; // 2.22.1 to avoid stack problems

		tmpEvent.type = E_CONTROLLER;
		tmpEvent.rawCode = UE_PRINT_FROM_DB_VIEW;
		tmpEvent.data = &dynReceipt.m_r;
		eventManager->Event(tmpEvent, E_CONTROLLER);

		break;
	}
	case UE_ACCUM:
		if (m_bFromTurn)
			(void *)&(*windowManager + new UIW_ACCUM(TRUE, TRUE));
		else if (m_bArchive)
			(void *)&(*windowManager + new UIW_ACCUM(TRUE, FALSE));
		break;
	case UE_SACCUM:
		if (m_bFromTurn)
			(void *)&(*windowManager + new UIW_SACCUM(TRUE, TRUE));
		else if (m_bArchive)
			(void *)&(*windowManager + new UIW_SACCUM(TRUE, FALSE));
		break;
#endif
#endif
	case UE_ACCEPT:
		eventManager->Put(UI_EVENT(S_CLOSE,0));
		break;

	case UE_VIEW:
		if (m_bFromTurn)
			m_pwNumber->Event(UI_EVENT(L_SELECT, 0));
		else
			m_pwDate->Event(UI_EVENT(L_SELECT, 0));

		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}
	return ccode;
}

EVENT_TYPE DBView::ProcessTurn(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode != L_SELECT)// && ccode != S_CURRENT)
		return ccode;

	DBView *pwThis = (DBView *) object->parent;

	/////////////////////////////////////////////////////////////////
	// Unload turn

	if (pwThis->m_bArchive)
	{
		g_dbEngine->UnloadArcDB();
		pwThis->m_bArchive = FALSE;
	}

	/////////////////////////////////////////////////////////////////
	// Load Turn

	int date;
	((UIW_DATE *)pwThis->Get("DATE"))->DataGet()->Export(&date);
	int turn = ((UIW_INTEGER *)pwThis->Get("TURN"))->DataGet();

	long fromNumber = 0;

	if (g_dbEngine->LoadArcDB(date, turn))
	{
		pwThis->m_bArchive = TRUE;
		fromNumber = g_dbEngine->GetArcLowerNumber();
	}

	// delegate processing
	pwThis->m_pwNumber->DataSet(&UI_BIGNUM(fromNumber));
	pwThis->m_pwNumber->Event(UI_EVENT(L_SELECT, 0));

	return ccode;
}

EVENT_TYPE DBView::ProcessNumber(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode != L_SELECT)// && ccode != S_CURRENT)
		return ccode;

	// begin 2.21.8 Build 9

	DBView *pwThis = (DBView *) object->parent;

	/////////////////////////////////////////////////////////////////
	// Clear

	pwThis->m_pwNumbers->Destroy();

	/////////////////////////////////////////////////////////////////
	// Fill Numbers

	long fromNumber;
	pwThis->m_pwNumber->DataGet()->Export(&fromNumber);

	DB_STORAGE::Iterator *pit = NULL;

	if (pwThis->m_bFromTurn)
	{
		pit = new DB_STORAGE::Iterator(*g_dbEngine->GetDBStorage().GetConcreteStorage());
	}
	else
	{
		if (pwThis->m_bArchive)
		{
			pit = new DB_STORAGE::Iterator(*g_dbEngine->GetArcDBStorage().GetConcreteStorage());
		}
	}
	if (!pit)
	{
		// problems
		pwThis->m_pwNumbers->Event(UI_EVENT(S_REDISPLAY, 0));
		pwThis->ShowRecord(0L); // clear

		return ccode;
	}

	fromNumber = pit->Restart(fromNumber); // fix

	pwThis->m_pwNumber->DataSet(&UI_BIGNUM(fromNumber)); // refresh

	int i = 0;
	long number;
	while (*pit && i < 1000)
	{
		number = pit->Current();

		STR16 szNumber;
		ltoa(number, szNumber, 10);

		*pwThis->m_pwNumbers
			+ new UIW_STRING(0, i, 10, szNumber, -1, STF_NO_FLAGS, WOF_VIEW_ONLY, ProcessNumbers);

		(*pit)++;
		i++;
	}

	delete pit;

	// end 2.21.8 Build 9

	// delegate
	pwThis->m_pwNumbers->Event(UI_EVENT(S_REDISPLAY, 0));

	UIW_STRING *pwCurrent = (UIW_STRING *)pwThis->m_pwNumbers->Current();
	if (pwCurrent)
		pwCurrent->Event(UI_EVENT(S_CURRENT, 0));

	return ccode;
}

EVENT_TYPE DBView::ProcessNumbers(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode != L_SELECT && ccode != S_CURRENT)
		return ccode;

	DBView *pwThis = (DBView *) object->parent->parent;

	pwThis->ShowRecord(atol(((UIW_STRING *)object)->DataGet()));

	return ccode;
}

void DBView::ShowRecord(long nNumber)
{
	BOOL bFound = FALSE;

	Receipt receipt;

	if (nNumber)
	{
		if (m_bFromTurn)
			bFound = g_dbEngine->Get(receipt, nNumber);
		else if (m_bArchive)
			bFound = g_dbEngine->GetArc(receipt, nNumber);
	}

	/////////////////////////////////////////////////////////////////
	// Fill data
	char *servPrompt, *timePrompt, *boothPrompt, *phonePrompt, *amountPrompt, *totalPrompt;
	servPrompt = boothPrompt = timePrompt = phonePrompt = amountPrompt = totalPrompt = "";

	char *serv, *booth, *phone;
	serv = booth = phone = "";

	UI_TIME time;
	UI_BIGNUM total(0.0F), amount(0.0F);

	BOOL notPaid = FALSE, tollFree = FALSE;

	if (bFound)
	{
		servPrompt = "Servicio";
		timePrompt = "   Hora";
		time.Import(receipt.Time);
		amountPrompt = "Cantidad";
		amount.Import((long)receipt.Amount);
		switch (receipt.Tag)
		{
		case Receipt::TEL:
			{
				serv = "Telefon�a";
				boothPrompt = "  Cabina";
				booth = g_cfg->BoothInfo[receipt.BoothNumber].Name;
				phonePrompt = "Tel�fono";
				phone = receipt.Phone;
				amountPrompt = "Duraci�n";
				amount.Import(g_Milisec2Time(receipt.ElapsedTime, g_cfg->CORRECTION_TIME));
				break;
			}
		case Receipt::SPECIAL_TEL:
			{
				serv = "Telefon�a Esp.";
				boothPrompt = "  Cabina";
				booth = g_cfg->BoothInfo[receipt.BoothNumber].Name;
				phonePrompt = "Tel�fono";
				phone = receipt.Phone;
				amountPrompt = "Duraci�n";
				amount.Import(g_Milisec2Time(receipt.ElapsedTime, g_cfg->CORRECTION_TIME));
				break;
			}
		case Receipt::FAX:
			{
				serv = "Fax";
				phonePrompt = "Tel�fono";
				phone = receipt.Phone;
				break;
			}
		case Receipt::TELEX:
			{
				serv = "Internet";
				amount.Import((long)receipt.CeilMin);
				break;
			}
		case Receipt::CARD:
			{
				serv = "Tarjeta Magn.";
				int cards = 0;
				for (int i=0; i < MAX_MAGNETIC_CARDS; i++)
					cards += receipt.Cards[i];
				amount.Import((long)cards);
				break;
			}
		case Receipt::OTHER:
			{
				serv = "Otros";
				phonePrompt = "  Motivo";
				phone = receipt.Motif;
				break;
			}
		}
		totalPrompt = "   Total";
		total.Import(receipt.Value);
		if (receipt.Stat.Paid & NOT_PAID_CALL)
			notPaid = TRUE;
		else if (receipt.Stat.Paid & TOLL_FREE_CALL)
			tollFree = TRUE;
	}
	//
	((UIW_STRING *)Get("PSERV"))->DataSet(servPrompt);
	((UIW_STRING *)Get("SERV"))->DataSet(serv);
	((UIW_STRING *)Get("PTIME"))->DataSet(timePrompt);
	((UIW_TIME   *)Get("TIME"))->DataSet(&time);
	((UIW_STRING *)Get("PBOOTH"))->DataSet(boothPrompt);
	((UIW_STRING *)Get("BOOTH"))->DataSet(booth);
	((UIW_STRING *)Get("PPHONE"))->DataSet(phonePrompt);
	((UIW_STRING *)Get("PHONE"))->DataSet(phone);
	((UIW_STRING *)Get("PAMOUNT"))->DataSet(amountPrompt);
	((UIW_BIGNUM *)Get("AMOUNT"))->DataSet(&amount);
	((UIW_STRING *)Get("PTOTAL"))->DataSet(totalPrompt);
	((UIW_BIGNUM *)Get("TOTAL"))->DataSet(&total);

	UIW_BUTTON *wButton;
	wButton = (UIW_BUTTON *)Get("NC");
	if (notPaid)
		wButton->woStatus |= WOS_SELECTED;
	else
		wButton->woStatus &= ~WOS_SELECTED;

	wButton->Event(UI_EVENT(S_REDISPLAY, 0));
	wButton = (UIW_BUTTON *)Get("PR");
	if (tollFree)
		wButton->woStatus |= WOS_SELECTED;
	else
		wButton->woStatus &= ~WOS_SELECTED;
	wButton->Event(UI_EVENT(S_REDISPLAY, 0));
}