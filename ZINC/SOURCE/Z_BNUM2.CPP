//	Zinc Interface Library - Z_NUM2.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

#define MAX_LENGTH	64

// ----- UIW_BIGNUM ---------------------------------------------------------

UIW_BIGNUM::UIW_BIGNUM(int left, int top, int width, UI_BIGNUM *_number,
	const char *_range, NMF_FLAGS _nmFlags, WOF_FLAGS _woFlags,
	USER_FUNCTION _userFunction) :
	UIW_STRING(left, top, width, NULL, MAX_LENGTH, STF_NO_FLAGS, _woFlags, _userFunction),
	number(NULL), nmFlags(_nmFlags)
{
#if defined(ZIL_LINKBUG)
	extern void z_bignum_dummy(void);	// Bug fix for broken linkers.
	z_bignum_dummy();
#endif

	// Initialize the number information.
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		number = new UI_BIGNUM;
	else
	{
		text = new char[MAX_LENGTH+1];
		text[0] = '\0';
	}
	range = ui_strdup(_range);
	UIW_BIGNUM::Information(INITIALIZE_CLASS, NULL);
	UIW_BIGNUM::DataSet(_number);
}

UIW_BIGNUM::~UIW_BIGNUM(void)
{
	// Restore the number information.
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete number;
	else
		delete text;
	if (range)
		delete range;
}

UI_BIGNUM *UIW_BIGNUM::DataGet(void)
{
	// Return the number.
	UIW_STRING::DataGet();
	number->Import(text);
	return (number);
}

void UIW_BIGNUM::DataSet(UI_BIGNUM *_number)
{
	// Reset the number.
	if (_number)
	{
		if (number == _number || FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
			number = _number;
		else
			number->Import(*_number);
	}
	number->Export(text, nmFlags);
	UIW_STRING::DataSet(text);
}

EVENT_TYPE UIW_BIGNUM::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;

#if defined(ZIL_OS2)
	// Check for OS/2 specific messages.
	if (ccode == E_OS2 && event.message.msg != WM_CHAR)
		return (UIW_STRING::Event(event));
#endif

	// Check for Zinc specific messages.
	ccode = LogicalEvent(event, ID_BIGNUM);
	switch (ccode)
	{
	case L_SELECT:
	case S_NON_CURRENT:
		if (!FlagSet(woStatus, WOS_INTERNAL_ACTION))
		{
			UI_BIGNUM tNumber = *number;		// Keep a temporary number in case of error.
			UIW_STRING::DataGet();
			number->Import(text);
			ccode = UIW_STRING::Event(event);	// Call the user or validate function.
			if (ccode == -1)
				*number = tNumber;				// An error occurred, restore the old number.
			else
				woStatus &= ~WOS_UNANSWERED;
			number->Export(text, nmFlags);
			UIW_STRING::DataSet(text);
		}
		else
			ccode = UIW_STRING::Event(event);
		break;

	case S_CREATE:
		number->Export(text, nmFlags);
		ccode = UIW_STRING::Event(event);
		break;

	default:
		ccode = UIW_STRING::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void *UIW_BIGNUM::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_BIGNUM;
	switch (request)
	{
	case INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = windowID[0] = ID_BIGNUM;
		windowID[1] = ID_NUMBER;
		windowID[2] = ID_STRING;
		// Continue to CHANGED_FLAGS.

	case CHANGED_FLAGS:
		// Check the object and base class flag settings.
		if (request == CHANGED_FLAGS)
			UIW_STRING::Information(CHANGED_FLAGS, data, ID_BIGNUM);

		if (objectID == ID_BIGNUM && FlagSet(woStatus, WOS_REDISPLAY))
		{
			UI_EVENT event(S_INITIALIZE, 0);
			Event(event);
			event.type = S_CREATE;
			Event(event);
		}
		break;

	case GET_VALUE:															// BUG.General
		// Get the bignum value.
		if (!data)
			return (UIW_BIGNUM::DataGet());
		*(UI_BIGNUM *)data = *(UIW_BIGNUM::DataGet());
		break;

	case SET_VALUE:															// BUG.General
		// Set the bignum value.
		UIW_BIGNUM::DataSet((UI_BIGNUM *)data);
		break;

	case GET_FLAGS:
	case SET_FLAGS:
	case CLEAR_FLAGS:
		// Get, set or clear the flag settings.
		if (objectID && objectID != ID_BIGNUM)
			data = UIW_STRING::Information(request, data, objectID);
		else if (request == GET_FLAGS && !data)
			data = &nmFlags;
		else if (request == GET_FLAGS)
			*(NMF_FLAGS *)data = nmFlags;
		else if (request == SET_FLAGS)
			nmFlags |= *(NMF_FLAGS *)data;
		else
			nmFlags &= ~(*(NMF_FLAGS *)data);
		break;

	default:
		data = UIW_STRING::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

int UIW_BIGNUM::Validate(int processError)
{
	// Check for an absolute number error.
	UI_BIGNUM currentNumber;
	char *stringNumber = (char *)UIW_STRING::Information(GET_TEXT, NULL);
	NMI_RESULT errorCode = currentNumber.Import(stringNumber);

	// Check for a range error.
	if (range && *range && errorCode == NMI_OK)
		errorCode = NMI_OUT_OF_RANGE;
	for (char *tRange = range; tRange && errorCode == NMI_OUT_OF_RANGE; )
	{
		char minNumber[MAX_LENGTH], maxNumber[MAX_LENGTH];
		tRange = ParseRange(tRange, minNumber, maxNumber);
		UI_BIGNUM small(minNumber);
		UI_BIGNUM big(maxNumber);
		if (currentNumber >= small && currentNumber <= big)
			errorCode = NMI_OK;
	}

	// Process the error code.
	woStatus &= ~WOS_INVALID;
	if (errorCode == NMI_OK)			// Set up the new number.
	{
		currentNumber.Export(stringNumber, nmFlags);
		UIW_STRING::Information(SET_TEXT, stringNumber);
		return (errorCode);
	}
	else if (!errorSystem)				// Restore the original number.
	{
		number->Export(stringNumber, nmFlags);
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
UIW_BIGNUM::UIW_BIGNUM(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	UIW_STRING(0, 0, 10, NULL, MAX_LENGTH, STF_NO_FLAGS, WOF_NO_FLAGS)
{
	// Initialize the number information.
	UIW_BIGNUM::Load(name, directory, file);
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
	UIW_STRING::Information(INITIALIZE_CLASS, NULL);
	UIW_BIGNUM::Information(INITIALIZE_CLASS, NULL);
}

void UIW_BIGNUM::Load(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Read the file information.
	UIW_STRING::Load(name, directory, file);
	file->Load(&nmFlags);
	file->Load(&range);

	number = new UI_BIGNUM;
	number->Import(text);
}

void UIW_BIGNUM::Store(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Store the number information.
	UIW_STRING::Store(name, directory, file);
	file->Store(nmFlags);
	file->Store(range);
}
#endif

