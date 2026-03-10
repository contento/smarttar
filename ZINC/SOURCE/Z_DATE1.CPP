//	Zinc Interface Library - Z_DATE1.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

#define MAX_LENGTH	64

// ----- UIW_DATE -----------------------------------------------------------

UIW_DATE::UIW_DATE(int left, int top, int width, UI_DATE *_date,
	const char *_range, DTF_FLAGS _dtFlags, WOF_FLAGS _woFlags,
	USER_FUNCTION _userFunction) :
	UIW_STRING(left, top, width, NULL, MAX_LENGTH, STF_NO_FLAGS, _woFlags, _userFunction),
	date(NULL), dtFlags(_dtFlags)
{
#if defined(ZIL_LINKBUG)
	extern void z_date1_dummy(void);	// Bug fix for broken linkers.
	z_date1_dummy();
#endif

	// Initialize the date information.
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		date = new UI_DATE;
	else
	{
		text = new char[MAX_LENGTH+1];
		text[0] = '\0';
	}
	range = ui_strdup(_range);
	UIW_DATE::Information(INITIALIZE_CLASS, NULL);
	UIW_DATE::DataSet(_date);
}

UIW_DATE::~UIW_DATE(void)
{
	// Restore the date information.
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete date;
	else
		delete text;
	if (range)
		delete range;
}

UI_DATE *UIW_DATE::DataGet(void)
{
	// Return the date.
	UIW_STRING::DataGet();
	date->Import(text, dtFlags);
	return (date);
}

void UIW_DATE::DataSet(UI_DATE *_date)
{
	// Reset the date.
	if (date == _date || FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		date = _date;
	else if (_date)
		date->Import(*_date);
	else
		date->Import();
	date->Export(text, dtFlags);
	UIW_STRING::DataSet(text);
}

EVENT_TYPE UIW_DATE::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;

	// Check for environment specific messages.
#if defined(ZIL_MSWINDOWS)
	if (ccode == E_MSWINDOWS && (event.message.message < WM_KEYFIRST ||
		event.message.message > WM_KEYLAST))
		return (UIW_STRING::Event(event));
#elif defined(ZIL_OS2)
	if (ccode == E_OS2 && event.message.msg != WM_CHAR)
		return (UIW_STRING::Event(event));
#endif

	// Check for Zinc specific messages.
	ccode = LogicalEvent(event, ID_DATE);
	switch (ccode)
	{
	case L_SELECT:
	case S_NON_CURRENT:
		if (!FlagSet(woStatus, WOS_INTERNAL_ACTION))
		{
			UI_DATE tDate = *date;				// Keep a temporary date in case of error.
			UIW_STRING::DataGet();
			date->Import(text, dtFlags);
			ccode = UIW_STRING::Event(event);	// Call the user or validate function.
			if (ccode == -1)
				*date = tDate;					// An error occurred, restore the old date.
			else
				woStatus &= ~WOS_UNANSWERED;
			date->Export(text, dtFlags);
			UIW_STRING::DataSet(text);
		}
		else
			ccode = UIW_STRING::Event(event);
		break;

	case S_CREATE:
		date->Export(text, dtFlags);
		ccode = UIW_STRING::Event(event);
		break;

	default:
		ccode = UIW_STRING::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void *UIW_DATE::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_DATE;
	switch (request)
	{
	case INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = windowID[0] = ID_DATE;
		windowID[1] = ID_STRING;
		break;

	case CHANGED_FLAGS:
		// Check the object and base class flag settings.
		UIW_STRING::Information(CHANGED_FLAGS, data, ID_DATE);

		// See if the field needs to be re-computed.
		if (objectID == ID_DATE && FlagSet(woStatus, WOS_REDISPLAY))
		{
			UI_EVENT event(S_INITIALIZE, 0);
			Event(event);
			event.type = S_CREATE;
			Event(event);
		}
		break;

	case GET_VALUE:															// BUG.General
		// Get the date value.
		if (!data)
			return (UIW_DATE::DataGet());
		*(UI_DATE *)data = *(UIW_DATE::DataGet());
		break;

	case SET_VALUE:															// BUG.General
		// Set the date value.
		UIW_DATE::DataSet((UI_DATE *)data);
		break;

	case GET_FLAGS:
	case SET_FLAGS:
	case CLEAR_FLAGS:
		// Get, set or clear the flag settings.
		if (objectID && objectID != ID_DATE)
			data = UIW_STRING::Information(request, data, objectID);
		else if (request == GET_FLAGS && !data)
			data = &dtFlags;
		else if (request == GET_FLAGS)
			*(DTF_FLAGS *)data = dtFlags;
		else if (request == SET_FLAGS)
			dtFlags |= *(DTF_FLAGS *)data;
		else
			dtFlags &= ~(*(DTF_FLAGS *)data);
		break;

	default:
		data = UIW_STRING::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

int UIW_DATE::Validate(int processError)
{
	// Check for an absolute date error.
	UI_DATE currentDate;
	char *stringDate = (char *)UIW_STRING::Information(GET_TEXT, NULL);
	DTI_RESULT errorCode = currentDate.Import(stringDate, dtFlags);

	// Check for a range error.
	if (range && *range && errorCode == DTI_OK)
		errorCode = DTI_OUT_OF_RANGE;
	for (char *tRange = range; tRange && errorCode == DTI_OUT_OF_RANGE; )
	{
		char minDate[MAX_LENGTH], maxDate[MAX_LENGTH];
		tRange = ParseRange(tRange, minDate, maxDate);
		UI_DATE small(minDate, rangeFlags);
		UI_DATE big(maxDate, rangeFlags);
		if (currentDate >= small && currentDate <= big)
			errorCode = DTI_OK;
	}

	// Process the error code.
	woStatus &= ~WOS_INVALID;
	if (errorCode == DTI_OK)			// Set up the new date.
	{
		currentDate.Export(stringDate, dtFlags);
		UIW_STRING::Information(SET_TEXT, stringDate);
		return (errorCode);
	}
	else if (!errorSystem)				// Restore the original date.
	{
		date->Export(stringDate, dtFlags);
		UIW_STRING::Information(SET_TEXT, stringDate);
		return (errorCode);
	}
	else if (!processError)
		return (errorCode);

	// Generate the error message and wait for a response.
	if (errorCode == DTI_OUT_OF_RANGE && (!range || !*range))
		errorCode = DTI_INVALID;
	for (int i = 0; errorTable[i].text; i++)
		if (errorTable[i].flags == errorCode)
		{
			WOS_STATUS _woStatus = woStatus;
			woStatus |= WOS_INTERNAL_ACTION;
			UIS_STATUS errorStatus = errorSystem->ReportError(windowManager,
				WOS_INVALID, errorTable[i].text, stringDate, range);
			if (!FlagSet(_woStatus, WOS_INTERNAL_ACTION))
				woStatus &= ~WOS_INTERNAL_ACTION;
//			if (errorStatus == WOS_INVALID)
//				woStatus |= WOS_INVALID;
//			else
//				return (-1);		// This will cause the date to be restored.
			if (errorStatus == WOS_INVALID)
				return (-1);		// This will cause the date to be restored.
			else
				woStatus |= WOS_INVALID;
			break;
		}
	if (errorCode == DTI_INVALID || errorCode == DTI_OUT_OF_RANGE)
		return (0);
	return (errorCode);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_PERSISTENCE)
UIW_DATE::UIW_DATE(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	UIW_STRING(0, 0, 15, NULL, MAX_LENGTH, STF_NO_FLAGS, WOF_NO_FLAGS)
{
	// Initialize the date information.
	UIW_DATE::Load(name, directory, file);
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
	UIW_STRING::Information(INITIALIZE_CLASS, NULL);
	UIW_DATE::Information(INITIALIZE_CLASS, NULL);
}

void UIW_DATE::Load(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Load the date information.
	UIW_STRING::Load(name, directory, file);
	file->Load(&dtFlags);
	file->Load(&range);

	date = new UI_DATE;
	if (FlagSet(dtFlags, DTF_SYSTEM))
		date->Export(text, dtFlags);	// Set the text according to the system date.
	else
		date->Import(text, dtFlags);	// Set the date according to the loaded text.
}

void UIW_DATE::Store(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Store the date information.
	UIW_STRING::Store(name, directory, file);
	file->Store(dtFlags);
	file->Store(range);
}
#endif


