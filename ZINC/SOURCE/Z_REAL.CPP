//	Zinc Interface Library - Z_REAL.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

#define MAX_LENGTH	64

// ----- UIW_REAL -----------------------------------------------------------

static void ui_sprintf(char *text, double number, int flags)
{
	char buffer[MAX_LENGTH];

	if (FlagSet(flags, NMF_SCIENTIFIC))
		strcpy(buffer, "%le");
	else if (NMF_DIGITS(flags) >= 0)
		sprintf(buffer, "%%.%dlg", NMF_DIGITS(flags) + 1);
	else
		strcpy(buffer, "%lg");
	::sprintf(text, buffer, number);
}

UIW_REAL::UIW_REAL(int left, int top, int width, double *_number,
	const char *_range, NMF_FLAGS _nmFlags, WOF_FLAGS _woFlags,
	USER_FUNCTION _userFunction) :
	UIW_STRING(left, top, width, NULL, MAX_LENGTH, STF_NO_FLAGS, _woFlags, _userFunction),
	number(NULL), nmFlags(_nmFlags)
{
#if defined(ZIL_LINKBUG)
	extern void z_real_dummy(void);	// Bug fix for broken linkers.
	z_real_dummy();
#endif

	// Initialize the real information.
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		number = new double;
	else
	{
		text = new char[MAX_LENGTH+1];
		text[0] = '\0';
	}
	range = ui_strdup(_range);
	UIW_REAL::Information(INITIALIZE_CLASS, NULL);
	UIW_REAL::DataSet(_number);
}

UIW_REAL::~UIW_REAL(void)
{
	// Restore the real information.
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete number;
	else
		delete text;
	if (range)
		delete range;
}

double UIW_REAL::DataGet(void)
{
	// Return the double value.
	UIW_STRING::DataGet();
	*number = atof(text);
	return (*number);
}

void UIW_REAL::DataSet(double *_number)
{
	// Reset the double value.
	if (number == _number || FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		number = _number;
	else if (_number)
		*number = *_number;
	else
		*number = 0.0;
	ui_sprintf(text, *number, nmFlags);
	UIW_STRING::DataSet(text);
}

EVENT_TYPE UIW_REAL::Event(const UI_EVENT &event)
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
	ccode = LogicalEvent(event, ID_REAL);
	switch (ccode)
	{
	case L_SELECT:
	case S_NON_CURRENT:
		if (!FlagSet(woStatus, WOS_INTERNAL_ACTION))
		{
			double tNumber = *number;			// Keep a temporary value in case of error.
			UIW_STRING::DataGet();
			*number = atof(text);
			ccode = UIW_STRING::Event(event);	// Call the user or valitime function.
			if (ccode == -1)
				*number = tNumber;				// An error occurred, restore the old value.
			else
				woStatus &= ~WOS_UNANSWERED;
			ui_sprintf(text, *number, nmFlags);
			UIW_STRING::DataSet(text);
		}
		else
			ccode = UIW_STRING::Event(event);
		break;

	case S_CREATE:
		ui_sprintf(text, *number, nmFlags);
		ccode = UIW_STRING::Event(event);
		break;

	default:
		ccode = UIW_STRING::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void *UIW_REAL::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_REAL;
	switch (request)
	{
	case INITIALIZE_CLASS:
		searchID = windowID[0] = ID_REAL;
		windowID[1] = ID_NUMBER;
		windowID[2] = ID_STRING;
		break;

	case CHANGED_FLAGS:
		// Check the object and base class flag settings.
		UIW_STRING::Information(CHANGED_FLAGS, data, ID_REAL);

		// See if the field needs to be re-computed.
		if (objectID == ID_REAL && FlagSet(woStatus, WOS_REDISPLAY))
		{
			UI_EVENT event(S_INITIALIZE, 0);
			Event(event);
			event.type = S_CREATE;
			Event(event);
		}
		break;

	case GET_VALUE:															// BUG.General
		// Get the double value.
		if (!data)
			return (NULL);
		*(double *)data = UIW_REAL::DataGet();
		break;

	case SET_VALUE:															// BUG.General
		// Set the double value.
		UIW_REAL::DataSet((double *)data);
		break;

	default:
		data = UIW_STRING::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

int UIW_REAL::Validate(int processError)
{
	// Check for an absolute number error.
	char *stringNumber = (char *)UIW_STRING::Information(GET_TEXT, NULL);
	double currentNumber = atof(stringNumber);
	NMI_RESULT errorCode = NMI_OK;

	// Check for a range error.
	if (range && *range && errorCode == NMI_OK)
		errorCode = NMI_OUT_OF_RANGE;
	for (char *tRange = range; tRange && errorCode == NMI_OUT_OF_RANGE; )
	{
		char minNumber[MAX_LENGTH], maxNumber[MAX_LENGTH];
		tRange = ParseRange(tRange, minNumber, maxNumber);
		if (currentNumber >= atof(minNumber) &&
			currentNumber <= atof(maxNumber))
			errorCode = NMI_OK;
	}

	// Process the error code.
	woStatus &= ~WOS_INVALID;
	if (errorCode == NMI_OK)			// Set up the new number.
	{
		ui_sprintf(stringNumber, currentNumber, nmFlags);
		UIW_STRING::Information(SET_TEXT, stringNumber);
		return (errorCode);
	}
	else if (!errorSystem)				// Restore the original number.
	{
		ui_sprintf(stringNumber, *number, nmFlags);
		UIW_STRING::Information(SET_TEXT, stringNumber);
		return (errorCode);
	}
	else if (!processError)
		return (errorCode);

	// Generate the error message and wait for a response.
	if (errorCode == NMI_OUT_OF_RANGE && (!range || !*range))
		errorCode = NMI_INVALID;
	for (int i = 0; errorTable[i].text; i++)
		if (errorTable[i].flags == errorCode)
		{
			WOS_STATUS _woStatus = woStatus;
			woStatus |= WOS_INTERNAL_ACTION;
			UIS_STATUS errorStatus = errorSystem->ReportError(windowManager,
				WOS_INVALID, errorTable[i].text, stringNumber, range);
			if (!FlagSet(_woStatus, WOS_INTERNAL_ACTION))
				woStatus &= ~WOS_INTERNAL_ACTION;
//			if (errorStatus == WOS_INVALID)
//				woStatus |= WOS_INVALID;
//			else
//				return (-1);		// This will cause the number to be restored.
			if (errorStatus == WOS_INVALID)
				return (-1);		// This will cause the number to be restored.
			else
				woStatus |= WOS_INVALID;
			break;
		}
	if (errorCode == NMI_OUT_OF_RANGE)
		return (0);
	return (errorCode);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_PERSISTENCE)
UIW_REAL::UIW_REAL(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	UIW_STRING(0, 0, 10, NULL, MAX_LENGTH, STF_NO_FLAGS, WOF_NO_FLAGS)
{
	// Initialize the number information.
	UIW_REAL::Load(name, directory, file);
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
	UIW_STRING::Information(INITIALIZE_CLASS, NULL);
	UIW_REAL::Information(INITIALIZE_CLASS, NULL);
}

void UIW_REAL::Load(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Load the real information.
	UIW_STRING::Load(name, directory, file);
	file->Load(&nmFlags);
	file->Load(&range);

	number = new double;
	*number = atof(text);
}

void UIW_REAL::Store(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Store the real information.
	UIW_STRING::Store(name, directory, file);
	file->Store(nmFlags);
	file->Store(range);
}
#endif


