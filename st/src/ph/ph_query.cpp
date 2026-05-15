//
// [ PH_QUERY.CPP ]
//

#include "stdst.h"

#include <strstream.h>
#include <ph_eng.h>
#include <db_eng.h>
#include <ph_query.h>
#include <events.h>

extern CFG 			*g_cfg;
extern PH_ENGINE	*g_phEngine;

PhoneQueryWindow::PhoneQueryWindow():
	UIW_WINDOW("PHONE_QUERY", defaultStorage),
	m_pwPlaces(NULL),
	m_pwPlace(NULL),
	m_pwCurrentPlace(NULL),
	m_pwTime(NULL),
	m_pwCost(NULL),
	m_pwTotal(NULL)
{
	windowManager->Center(this);
	//helpContext = H_PH_QUERY;
}

EVENT_TYPE PhoneQueryWindow::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_CREATE:
		{
			UIW_WINDOW::Event(event);

			Get("LOCAL")->userFunction = ProcessSource;
			Get("NAL")->userFunction   = ProcessSource;
			Get("INTER")->userFunction = ProcessSource;

			m_pwPlaces = (UIW_VT_LIST *) Get("PLACES");
			m_pwPlaces->woFlags |= WOF_VIEW_ONLY;

			m_pwPlace = (UIW_STRING *) Get("PLACE");
			m_pwPlace->userFunction = ProcessPlace;

			m_pwTime = (UIW_BIGNUM *) Get("TIME");
			m_pwTime->userFunction = ProcessTime;

			m_pwCost = (UIW_BIGNUM  *) Get("COST");

			m_pwTotal = (UIW_BIGNUM  *) Get("TOTAL");
			break;
		}
	case UE_ACCEPT:
		eventManager->Put(UI_EVENT(S_CLOSE,0));
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}
	return ccode;
}

EVENT_TYPE PhoneQueryWindow::ProcessSource(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode != L_SELECT && ccode != S_CURRENT)
		return ccode;

	PhoneQueryWindow *pwThis = (PhoneQueryWindow *) object->parent->parent;

	pwThis->m_pwPlaces->Destroy();
	pwThis->m_pwPlaces->Event(UI_EVENT(S_REDISPLAY, 0));

	pwThis->m_pwCost->DataSet(&UI_BIGNUM(0L));
	pwThis->m_pwTotal->DataSet(&UI_BIGNUM(0L));

	return ccode;
}

EVENT_TYPE PhoneQueryWindow::ProcessPlaces(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode != L_SELECT && ccode != S_CURRENT)
		return ccode;

	PhoneQueryWindow *pwThis = (PhoneQueryWindow *) object->parent->parent;

	pwThis->m_pwCurrentPlace = (UIW_STRING *)object;

	// update place
	char *pszCP = pwThis->m_pwCurrentPlace->DataGet();
	int n = 0;
	while (pszCP[n] != ':') // trim until ':'
	{
		++n;
	}
	CITY_NAME place;
	strncpy(place, pszCP, n);
	place[n] = '\0'; // see notes about strncpy
	pwThis->m_pwPlace->DataSet(place);

	pwThis->Recalc();

	return ccode;
}

EVENT_TYPE PhoneQueryWindow::ProcessPlace(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode != L_SELECT)
		return ccode;

	PhoneQueryWindow *pwThis = (PhoneQueryWindow *) object->parent;

	pwThis->m_pwPlaces->Destroy();

	if (!strlen(pwThis->m_pwPlace->DataGet())) // don't process an empty place
	{
		pwThis->m_pwPlaces->Event(UI_EVENT(S_REDISPLAY, 0));
		return ccode;
	}

	/////////////////////////////////////////////////////////////////
	// Add new items
	CITY_NAME place;
	strcpy(place, pwThis->m_pwPlace->DataGet());

	int nFrom = PH_ENGINE::LOCAL_SOURCE;
	if (FlagSet(((UIW_BUTTON *)pwThis->Get("NAL"))->woStatus, WOS_SELECTED))
		nFrom = PH_ENGINE::DDN_SOURCE;
	else if (FlagSet(((UIW_BUTTON *)pwThis->Get("INTER"))->woStatus, WOS_SELECTED))
		nFrom = PH_ENGINE::DDI_SOURCE;

	extern PH_ENGINE *g_phEngine;
	PH_ENGINE::PLACE_ENTRY_LIST placeList;

	if (g_phEngine->SearchPlace(place, placeList, nFrom))
	{
		PH_ENGINE::PLACE_ENTRY_LIST_ITERATOR it(placeList);
		int i = 0;

		while (it)
		{
			strstream strPlace;
			g_phEngine->EntryToLine(strPlace, it.current());
			strPlace << ends;
			*pwThis->m_pwPlaces
				+ new UIW_STRING(0, i, 83, strPlace.str(), -1, STF_NO_FLAGS, WOF_VIEW_ONLY, ProcessPlaces);

			++i;
			++it;
		}

		// Recalc values based on first item on list
		pwThis->m_pwCurrentPlace = (UIW_STRING *) pwThis->m_pwPlaces->First();
		pwThis->Recalc();
	}
	else
	{
		// clear contents
		pwThis->m_pwCost->DataSet(&UI_BIGNUM(0L));
		pwThis->m_pwTotal->DataSet(&UI_BIGNUM(0L));

		pwThis->m_pwCurrentPlace = NULL;
	}

	pwThis->m_pwPlaces->Event(UI_EVENT(S_REDISPLAY, 0));

	return ccode;
}

EVENT_TYPE PhoneQueryWindow::ProcessTime(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode != L_SELECT)
		return ccode;

	PhoneQueryWindow *pwThis = (PhoneQueryWindow *) object->parent;

	pwThis->Recalc();

	return ccode;
}

BOOL PhoneQueryWindow::Recalc()
{
	if (!m_pwCurrentPlace)
		return FALSE;

	/////////////////////////////////////////////////////////////////
	//	Parse tokens

	char *pszCP = m_pwCurrentPlace->DataGet();
	char *pszToken;
	int nLen;

	// city
	pszToken = pszCP;
	while (*pszCP != ':')
		++pszCP;
	CITY_NAME szCity;

	nLen = (size_t)(pszCP-pszToken);
	strncpy(szCity, pszToken, nLen);
	szCity[nLen] = '\0';

	// tariff
	pszToken = ++pszCP;
	while (*pszCP != '=')
		++pszCP;
	STR16 szTariff;

	nLen = (size_t)(pszCP-pszToken);
	strncpy(szTariff, pszToken, nLen);
	szTariff[nLen] = '\0';

	// first phone
	pszToken = ++pszCP;
	while (*pszCP && isdigit(*pszCP))
		++pszCP;

	PHONE szPhone;
	int   nAccess = 0;

	if (FlagSet(((UIW_BUTTON *)Get("NAL"))->woStatus, WOS_SELECTED))
	{
		szPhone[0] = g_cfg->ACCESS + '0';
		szPhone[1] = g_cfg->OPERATOR_ACCESS + '0';
		nAccess = 2;
	}
	else if (FlagSet(((UIW_BUTTON *)Get("INTER"))->woStatus, WOS_SELECTED))
	{
		szPhone[0] = g_cfg->ACCESS + '0';
		szPhone[1] = g_cfg->INTER_ACCESS + '0';
		szPhone[2] = g_cfg->OPERATOR_ACCESS + '0';
		nAccess = 3;
	}

	nLen = (size_t)(pszCP-pszToken);
	strncpy(szPhone+nAccess, pszToken, nLen);
	szPhone[nLen+nAccess] = '\0';

	/////////////////////////////////////////////////////////////////
	// calc values

	WORD attr = 0;
	PH_ENGINE::GetCallAttr(szPhone, attr);

	PH_ENGINE::CallInfo callInfo;
	strcpy(callInfo.city, szCity);
	strcpy(callInfo.phone, szPhone);
	callInfo.date 	   		= _GetSysDate();
	callInfo.time 			= _GetSysTime();
	callInfo.nTariff		= atoi(szTariff);
	callInfo.callAttr 		= attr;
	callInfo.isExtension	= FALSE;
	double time;
	m_pwTime->DataGet()->Export(&time);
	time *= 1000L*60;
	callInfo.elapsedTime  	= time;

	g_phEngine->CalcCallValues(callInfo);

	m_pwCost->DataSet(&UI_BIGNUM(callInfo.valuePerMin));
	m_pwTotal->DataSet(&UI_BIGNUM(callInfo.value));

	return TRUE;
}
