//	Zinc Interface Library - Z_TIME1.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

#define MAX_LENGTH	64

// ----- UIW_TIME -----------------------------------------------------------

UIW_TIME::UIW_TIME(int left, int top, int width, UI_TIME *_time,
	const char *_range, DTF_FLAGS _tmFlags, WOF_FLAGS _woFlags,
	USER_FUNCTION _userFunction) :
	UIW_STRING(left, top, width, NULL, MAX_LENGTH, STF_NO_FLAGS, _woFlags, _userFunction),
	time(NULL), tmFlags(_tmFlags)
{
#if defined(ZIL_LINKBUG)
	extern void z_time1_dummy(void);	// Bug fix for broken linkers.
	z_time1_dummy();
#endif

	// Initialize the time information.
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		time = new UI_TIME;
	else
	{
		text = new char[MAX_LENGTH+1];
		text[0] = '\0';
	}
	range = ui_strdup(_range);
	UIW_TIME::Information(INITIALIZE_CLASS, NULL);
	UIW_TIME::DataSet(_time);
}

UIW_TIME::~UIW_TIME(void)
{
	// Restore the time information.
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete time;
	else
		delete text;
	if (range)
		delete range;
}

UI_TIME *UIW_TIME::DataGet(void)
{
	// Return the time.
	UIW_STRING::DataGet();
	time->Import(text, tmFlags);
	return (time);
}

void UIW_TIME::DataSet(UI_TIME *_time)
{
	// Reset the time.
	if (time == _time || FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		time = _time;
	else if (_time)
		time->Import(*_time);
	else
		time->Import();
	time->Export(text, tmFlags);
	UIW_STRING::DataSet(text);
}

EVENT_TYPE UIW_TIME::Event(const UI_EVENT &event)
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
	ccode = LogicalEvent(event, ID_TIME);
	switch (ccode)
	{
	case L_SELECT:
	case S_NON_CURRENT:
		if (!FlagSet(woStatus, WOS_INTERNAL_ACTION))
		{
			UI_TIME tTime = *time;				// Keep a temporary time in case of error.
			UIW_STRING::DataGet();
			time->Import(text, tmFlags);
			ccode = UIW_STRING::Event(event);	// Call the user or valitime function.
			if (ccode == -1)
				*time = tTime;					// An error occurred, restore the old time.
			else
				woStatus &= ~WOS_UNANSWERED;
			time->Export(text, tmFlags);
			UIW_STRING::DataSet(text);
		}
		else
			ccode = UIW_STRING::Event(event);
		break;

	case S_CREATE:
		time->Export(text, tmFlags);
		ccode = UIW_STRING::Event(event);
		break;

	default:
		ccode = UIW_STRING::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void *UIW_TIME::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_TIME;
	switch (request)
	{
	case INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = windowID[0] = ID_TIME;
		windowID[1] = ID_STRING;
		break;

	case CHANGED_FLAGS:
		// Check the object and base class flag settings.
		UIW_STRING::Information(CHANGED_FLAGS, data, ID_TIME);

		// See if the field needs to be re-computed.
		if (objectID == ID_TIME && FlagSet(woStatus, WOS_REDISPLAY))
		{
			UI_EVENT event(S_INITIALIZE, 0);
			Event(event);
			event.type = S_CREATE;
			Event(event);
		}
		break;

	case GET_VALUE:															// BUG.General
		// Get the time value.
		if (!data)
			return (UIW_TIME::DataGet());
		*(UI_TIME *)data = *(UIW_TIME::DataGet());
		break;

	case SET_VALUE:															// BUG.General
		// Set the time value.
		UIW_TIME::DataSet((UI_TIME *)data);
		break;

	case GET_FLAGS:
	case SET_FLAGS:
	case CLEAR_FLAGS:
		// Get, set or clear the flag settings.
		if (objectID && objectID != ID_TIME)
			data = UIW_STRING::Information(request, data, objectID);
		else if (request == GET_FLAGS && !data)
			data = &tmFlags;
		else if (request == GET_FLAGS)
			*(TMF_FLAGS *)data = tmFlags;
		else if (request == SET_FLAGS)
			tmFlags |= *(TMF_FLAGS *)data;
		else
			tmFlags &= ~(*(TMF_FLAGS *)data);
		break;

	default:
		data = UIW_STRING::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

int UIW_TIME::Validate(int processError)
{
	// Check for an absolute time error.
	UI_TIME currentTime;
	char *stringTime = (char *)UIW_STRING::Information(GET_TEXT, NULL);
	TMI_RESULT errorCode = currentTime.Import(stringTime, tmFlags);

	// Check for a range error.
	if (range && *range && errorCode == TMI_OK)
		errorCode = TMI_OUT_OF_RANGE;
	for (char *tRange = range; tRange && errorCode == TMI_OUT_OF_RANGE; )
	{
		char minTime[MAX_LENGTH], maxTime[MAX_LENGTH];
		tRange = ParseRange(tRange, minTime, maxTime);
		UI_TIME small(minTime, rangeFlags);
		UI_TIME big(maxTime, rangeFlags);
		if (currentTime >= small && currentTime <= big)
			errorCode = TMI_OK;
	}

	// Process the error code.
	woStatus &= ~WOS_INVALID;
	if (errorCode == TMI_OK)			// Set up the new time.
	{
		currentTime.Export(stringTime, tmFlags);
		UIW_STRING::Information(SET_TEXT, stringTime);
		return (errorCode);
	}
	else if (!errorSystem)				// Restore the original time.
	{
		time->Export(stringTime, tmFlags);
		UIW_STRING::Information(SET_TEXT, stringTime);
		return (errorCode);
	}
	else if (!processError)
		return (errorCode);

	// Generate the error message and wait for a response.
	if (errorCode == TMI_OUT_OF_RANGE && (!range || !*range))
		errorCode = TMI_INVALID;
	for (int i = 0; errorTable[i].text; i++)
		if (errorTable[i].flags == errorCode)
		{
			WOS_STATUS _woStatus = woStatus;
			woStatus |= WOS_INTERNAL_ACTION;
			UIS_STATUS errorStatus = errorSystem->ReportError(windowManager,
				WOS_INVALID, errorTable[i].text, stringTime, range);
			if (!FlagSet(_woStatus, WOS_INTERNAL_ACTION))
				woStatus &= ~WOS_INTERNAL_ACTION;
//			if (errorStatus == WOS_INVALID)
//				woStatus |= WOS_INVALID;
//			else
//				return (-1);		// This will cause the time to be restored.
			if (errorStatus == WOS_INVALID)
				return (-1);		// This will cause the time to be restored.
			else
				woStatus |= WOS_INVALID;
			break;
		}
	if (errorCode == TMI_INVALID || errorCode == TMI_OUT_OF_RANGE)
		return (0);
	return (errorCode);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_PERSISTENCE)
UIW_TIME::UIW_TIME(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	UIW_STRING(0, 0, 10, NULL, MAX_LENGTH, STF_NO_FLAGS, WOF_NO_FLAGS)
{
	// Initialize the time information.
	UIW_TIME::Load(name, directory, file);
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
	UIW_STRING::Information(INITIALIZE_CLASS, NULL);
	UIW_TIME::Information(INITIALIZE_CLASS, NULL);
}

void UIW_TIME::Load(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Read the file information.
	UIW_STRING::Load(name, directory, file);
	file->Load(&tmFlags);
	file->Load(&range);

	time = new UI_TIME;
	if (FlagSet(tmFlags, TMF_SYSTEM))
		time->Export(text, tmFlags);	// Set the text according to the system time.
	else
		time->Import(text, tmFlags);	// Set the time according to the loaded text.
}

void UIW_TIME::Store(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Store the time information.
	UIW_STRING::Store(name, directory, file);
	file->Store(tmFlags);
	file->Store(range);
}
#endif


