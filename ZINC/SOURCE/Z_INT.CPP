//	Zinc Interface Library - Z_INT.CPP
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

// ----- UIW_INTEGER --------------------------------------------------------

UIW_INTEGER::UIW_INTEGER(int left, int top, int width, int *_number,
	const char *_range, NMF_FLAGS _nmFlags, WOF_FLAGS _woFlags,
	USER_FUNCTION _userFunction) :
	UIW_STRING(left, top, width, NULL, MAX_LENGTH, STF_NO_FLAGS, _woFlags, _userFunction),
	number(NULL), nmFlags(_nmFlags)
{
#if defined(ZIL_LINKBUG)
	extern void z_int_dummy(void);	// Bug fix for broken linkers.
	z_int_dummy();
#endif

	// Initialize the integer information.
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		number = new int;
	else
	{
		text = new char[MAX_LENGTH+1];
		text[0] = '\0';
	}
	range = ui_strdup(_range);
	UIW_INTEGER::Information(INITIALIZE_CLASS, NULL);
	UIW_INTEGER::DataSet(_number);
}

UIW_INTEGER::~UIW_INTEGER(void)
{
	// Restore the integer information.
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete number;
	else
		delete text;
	if (range)
		delete range;
}

int UIW_INTEGER::DataGet(void)
{
	// Return the integer value.
	UIW_STRING::DataGet();
	*number = atoi(text);
	return (*number);
}

void UIW_INTEGER::DataSet(int *_number)
{
	// Reset the integer value.
	if (number == _number || FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		number = _number;
	else if (_number)
		*number = *_number;
	else
		*number = 0;
	::sprintf(text, "%d", *number);
	UIW_STRING::DataSet(text);
}

EVENT_TYPE UIW_INTEGER::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;

#if defined(ZIL_OS2)
	// Check for OS/2 specific messages.
	if (ccode == E_OS2 && event.message.msg != WM_CHAR)
		return (UIW_STRING::Event(event));
#endif

	// Check for Zinc specific messages.
	ccode = LogicalEvent(event, ID_INTEGER);
	switch (ccode)
	{
	case L_SELECT:
	case S_NON_CURRENT:
		if (!FlagSet(woStatus, WOS_INTERNAL_ACTION))
		{
			int tNumber = *number;				// Keep a temporary value in case of error.
			UIW_STRING::DataGet();
			*number = atoi(text);
			ccode = UIW_STRING::Event(event);	// Call the user or valitime function.
			if (ccode == -1)
				*number = tNumber;				// An error occurred, restore the old value.
			else
				woStatus &= ~WOS_UNANSWERED;
			::sprintf(text, "%d", *number);
			UIW_STRING::DataSet(text);
		}
		else
			ccode = UIW_STRING::Event(event);
		break;

	case S_CREATE:
		::sprintf(text, "%d", *number);
		ccode = UIW_STRING::Event(event);
		break;

	default:
		ccode = UIW_STRING::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void *UIW_INTEGER::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_INTEGER;
	switch (request)
	{
	case INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = windowID[0] = ID_INTEGER;
		windowID[1] = ID_NUMBER;
		windowID[2] = ID_STRING;
		break;

	case CHANGED_FLAGS:
		// Check the object and base class flag settings.
		UIW_STRING::Information(CHANGED_FLAGS, data, ID_INTEGER);

		// See if the field needs to be re-computed.
		if (objectID == ID_INTEGER && FlagSet(woStatus, WOS_REDISPLAY))
		{
			UI_EVENT event(S_INITIALIZE, 0);
			Event(event);
			event.type = S_CREATE;
			Event(event);
		}
		break;

	case GET_VALUE:															// BUG.General
		// Get the integer value.
		if (!data)
			return (NULL);
		*(int *)data = UIW_INTEGER::DataGet();
		break;

	case SET_VALUE:															// BUG.General
		// Set the integer value.
		UIW_INTEGER::DataSet((int *)data);
		break;

	default:
		data = UIW_STRING::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

int UIW_INTEGER::Validate(int processError)
{
	// Check for an absolute number error.
	char *stringNumber = (char *)UIW_STRING::Information(GET_TEXT, NULL);
	int currentNumber = atoi(stringNumber);
	NMI_RESULT errorCode = NMI_OK;

	// Check for a range error.
	if (range && *range && errorCode == NMI_OK)
		errorCode = NMI_OUT_OF_RANGE;
	for (char *tRange = range; tRange && errorCode == NMI_OUT_OF_RANGE; )
	{
		char minNumber[MAX_LENGTH], maxNumber[MAX_LENGTH];
		tRange = ParseRange(tRange, minNumber, maxNumber);
		if (currentNumber >= atoi(minNumber) &&
			currentNumber <= atoi(maxNumber))
			errorCode = NMI_OK;
	}

	// Process the error code.
	woStatus &= ~WOS_INVALID;
	if (errorCode == NMI_OK)			// Set up the new number.
	{
		::sprintf(stringNumber, "%d", currentNumber);
		UIW_STRING::Information(SET_TEXT, stringNumber);
		return (errorCode);
	}
	else if (!errorSystem)				// Restore the original number.
	{
		::sprintf(stringNumber, "%d", *number);
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
UIW_INTEGER::UIW_INTEGER(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	UIW_STRING(0, 0, 10, NULL, MAX_LENGTH, STF_NO_FLAGS, WOF_NO_FLAGS)
{
	// Initialize the number information.
	UIW_INTEGER::Load(name, directory, file);
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
	UIW_STRING::Information(INITIALIZE_CLASS, NULL);
	UIW_INTEGER::Information(INITIALIZE_CLASS, NULL);
}

void UIW_INTEGER::Load(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Load the integer information.
	UIW_STRING::Load(name, directory, file);
	file->Load(&nmFlags);
	file->Load(&range);

	number = new int;
	*number = atoi(text);
}

void UIW_INTEGER::Store(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Store the integer information.
	UIW_STRING::Store(name, directory, file);
	file->Store(nmFlags);
	file->Store(range);
}
#endif
