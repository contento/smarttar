//	Zinc Interface Library - Z_STRING.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define INCL_WINENTRYFIELDS		// OS/2 entry field messages and flags.
#define INCL_WINLISTBOXES		// OS/2 list messages and flags.
#include <ctype.h>
#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif
#if defined(ZIL_MOTIF)
#	include <Xm/Text.h>
#endif

// ----- UIW_STRING ---------------------------------------------------------

UIW_STRING::UIW_STRING(int left, int top, int width, char *_text,
	int _maxLength, STF_FLAGS _stFlags, WOF_FLAGS _woFlags,
	USER_FUNCTION _userFunction) :
	UI_WINDOW_OBJECT(left, top, width, 1, _woFlags, WOAF_NO_FLAGS),
	text(NULL), maxLength(_maxLength), stFlags(_stFlags)
{
	// Initialize the string information.
	if (maxLength == -1)
		maxLength = _text ? ui_strlen(_text) + 1 : width;
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA) && maxLength)
		text = new char[maxLength+1];
	userFunction = _userFunction;
	UIW_STRING::Information(INITIALIZE_CLASS, NULL);
	if (!_text)
	   _text = "";
	UIW_STRING::DataSet(_text);
}

UIW_STRING::~UIW_STRING(void)
{
	// Destroy the string information.
	if (text && !FlagSet(woFlags, WOF_NO_ALLOCATE_DATA) && !FlagSet(stFlags, STF_SUBSTRING))
		delete text;
}

char *UIW_STRING::DataGet(void)
{
#if defined(ZIL_MSWINDOWS)
//	if (screenID && !FlagSet(woFlags, WOF_VIEW_ONLY))						// BUG.1389
	if (screenID && screenID != parent->screenID && !FlagSet(woFlags, WOF_VIEW_ONLY))
		SendMessage(screenID, WM_GETTEXT, maxLength, (LONG)text);
#elif defined(ZIL_OS2)
	if (screenID && !parent->Inherited(ID_VT_LIST) && !FlagSet(woFlags, WOF_VIEW_ONLY))
		WinQueryWindowText(screenID, maxLength, (PSZ)text);
#elif defined(ZIL_MOTIF)
	if (screenID && parent->Inherited(ID_COMBO_BOX))
		;
	else if (screenID && !parent->Inherited(ID_VT_LIST) &&
		!FlagSet(stFlags, STF_PASSWORD))
	{
		char *tText = XmTextGetString(screenID);
		strcpy(text, tText);
		XtFree(tText);
	}
#endif

	// Return the text information.
	return (text);
}

void UIW_STRING::DataSet(char *_text, int _maxLength)
{
	if (_text)
	{
		// Reset the text length.
		if (_maxLength != -1)
		{
			if (_maxLength > maxLength && !FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
			{
				char *tText = new char[_maxLength+1];
				strncpy(tText, text ? text : "", maxLength);
				tText[maxLength-1] = '\0';
				if (_text == text)
					_text = tText;
				if (text)
					delete text;
				text = tText;
			}
			maxLength = _maxLength;
		}

		// Reset the text and compute the length.
		if (text == _text || FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
			text = _text;
		else
		{
			strncpy(text, _text, maxLength);
			text[maxLength-1] = '\0';
		}
		if (FlagSet(woStatus, WOS_UNANSWERED))
			strcpy(text, "");
#if defined(ZIL_MSDOS)
		if (!FlagSet(stFlags, STF_SUBSTRING))
			length = ui_strlen(text);
		cursor = (FlagSet(woFlags, WOF_AUTO_CLEAR) && !FlagSet(woFlags, WOF_VIEW_ONLY)) ? length : 0;
		beginMark = endMark = -1;
		leftClip = 0;
#endif
	}

	// Redisplay the string field.
	if (screenID)
	{
#if defined(ZIL_MSDOS)
		if (parent->Inherited(ID_COMBO_BOX) && parent->parent->Inherited(ID_COMBO_BOX) &&	// BUG.1267
			FlagSet(woStatus, WOS_CURRENT) && parent->parent->screenID)
				parent->parent->Information(SET_TEXT, text);
		else
			Event(UI_EVENT(S_REDISPLAY));
#elif defined(ZIL_MSWINDOWS)
		if (parent->Inherited(ID_COMBO_BOX))
		{
//			SendMessage(screenID, CB_SETITEMDATA, ListIndex(),				// BUG.1267
//				FlagSet(parent->woStatus, WOS_OWNERDRAW) ? (LONG)this : (LONG)text);
			SCREENID tScreenID = screenID;
			SendMessage(screenID, CB_DELETESTRING, ListIndex(), 0);
			SendMessage(screenID, CB_INSERTSTRING, ListIndex(),
				FlagSet(parent->woStatus, WOS_OWNERDRAW) ? (LONG)this : (LONG)text);
			if (FlagSet(woStatus, WOS_CURRENT))
				SendMessage(screenID, CB_SETCURSEL, ListIndex(), 0);
			screenID = tScreenID;
		}
		else if (parent->Inherited(ID_LIST))
		{
//			SendMessage(screenID, LB_SETITEMDATA, ListIndex(),
//				FlagSet(parent->woStatus, WOS_OWNERDRAW) ? (LONG)this : (LONG)text);
			SCREENID tScreenID = screenID;
			SendMessage(screenID, LB_DELETESTRING, ListIndex(), 0);
			SendMessage(screenID, LB_INSERTSTRING, ListIndex(),
				FlagSet(parent->woStatus, WOS_OWNERDRAW) ? (LONG)this : (LONG)text);
			if (FlagSet(woStatus, WOS_CURRENT))
				SendMessage(screenID, LB_SETCURSEL, ListIndex(), 0);
			screenID = tScreenID;
		}
		else
		{
			if (_maxLength != -1)
				SendMessage(screenID, EM_LIMITTEXT, maxLength - 1, FALSE);
			SendMessage(screenID, WM_SETTEXT, 0, (LONG)text);
			if (FlagSet(woFlags, WOF_AUTO_CLEAR))
				SendMessage(screenID, EM_SETSEL, 0, 0x7FFF0000L);
		}
#elif defined(ZIL_OS2)
		if (parent->Inherited(ID_COMBO_BOX) && !FlagSet(parent->woStatus, WOS_OWNERDRAW))
			WinSendMsg(parent->screenID, LM_SETITEMTEXT, (MPARAM)ListIndex(), (MPARAM)text);
		else if (parent->Inherited(ID_VT_LIST))
			WinSendMsg(parent->screenID, LM_SETITEMTEXT, (MPARAM)ListIndex(), (MPARAM)text);
		else
		{
			if (_maxLength != -1)
				WinSendMsg(screenID, EM_SETTEXTLIMIT, (MPARAM)maxLength, (MPARAM)0);
			WinSetWindowText(screenID, (PSZ)text);
			Event(UI_EVENT(S_REDISPLAY));
		}
#elif defined(ZIL_MOTIF)
		if (screenID && parent->Inherited(ID_COMBO_BOX))
		{
			XmString labelText = XmStringCreateLtoR(text ? text : "", 
				XmSTRING_DEFAULT_CHARSET);
			nargs = 0;
			XtSetArg(args[nargs], XmNlabelString, labelText); nargs++;
			XtSetValues(screenID, args, nargs);
			if (labelText)
				XmStringFree(labelText);
		}
		else if (screenID)
		{
			if (!text)														// BUG.INTERNAL
				text = "";
			char *tText = text;
			if (FlagSet(stFlags, STF_PASSWORD))
			{
				tText = strdup(text);
				*text = 0;	// DataGet() will be called by StringCallback().
			}

			// Use XtSetValues because XmTextSetString doesn't flush.
			int currentMax;
			char *value;
			XtVaGetValues(screenID, XmNmaxLength, &currentMax,
				XmNvalue, &value, NULL);
			nargs = 0;
			if (_maxLength != -1 && _maxLength != currentMax)
				XtSetArg(args[nargs], XmNmaxLength, maxLength), nargs++;
			if (strcmp(tText, value))
				XtSetArg(args[nargs], XmNvalue, tText), nargs++;
			if (tText != text)
				delete (tText);
			XtSetValues(screenID, args, nargs);
		}
#endif
	}
}

void *UIW_STRING::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_STRING;
	switch (request)
	{
	case INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = windowID[0] = ID_STRING;
		insertMode = TRUE;
#if defined(ZIL_MSDOS)
		font |= IGNORE_UNDERSCORE;
		scrollCount = 5;
		leftClip = cursor = lineWidth = 0;
		beginMark = endMark = -1;
#endif
		// Continue to CHANGED_FLAGS.

	case CHANGED_FLAGS:
		// Check the object and base class flag settings.
		if (request == CHANGED_FLAGS)
			UI_WINDOW_OBJECT::Information(CHANGED_FLAGS, data, ID_STRING);

#if defined(ZIL_MSWINDOWS)
		// Check the environment specific flag settings.
		dwStyle |= ES_AUTOHSCROLL;
		if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
			dwStyle |= ES_CENTER;
		else if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
			dwStyle |= ES_RIGHT;
		if (FlagSet(stFlags, STF_LOWER_CASE))
			dwStyle |= ES_LOWERCASE;
		else if (FlagSet(stFlags, STF_UPPER_CASE))
			dwStyle |= ES_UPPERCASE;
		if (FlagSet(stFlags, STF_PASSWORD))
			dwStyle |= ES_PASSWORD;
#elif defined(ZIL_OS2)
		// Check the environment specific flag settings.
		flStyle |= ES_AUTOSCROLL;											// BUG.2260
		if (FlagSet(woFlags, WOF_BORDER))
			flStyle |= ES_MARGIN;
		if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
			flStyle |= ES_CENTER;
		else if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
			flStyle |= ES_RIGHT;
		if (FlagSet(woFlags, WOF_VIEW_ONLY | WOF_NON_SELECTABLE))			// BUG.????
			flStyle |= ES_READONLY; 
		if (FlagSet(stFlags, STF_PASSWORD))
			flStyle |= ES_UNREADABLE;
#endif

		// See if the field needs to be re-computed.
		if (objectID == ID_STRING && FlagSet(woStatus, WOS_REDISPLAY))
		{
			UI_EVENT event(S_INITIALIZE, 0);
			Event(event);
			event.type = S_CREATE;
			Event(event);
		}
		break;

	case GET_TEXT:
		// Get the string text.
		if (!data)
			return (UIW_STRING::DataGet());
		*(char **)data = UIW_STRING::DataGet();
		break;

	case COPY_TEXT:
		// Copy the string text.
		data = strcpy((char *)data, UIW_STRING::DataGet());
		break;

	case SET_TEXT:
		// Set the string text.
		UIW_STRING::DataSet((char *)data);
		break;

	case GET_TEXT_LENGTH:
		// Get the string text length.
		if (!data)
			return (&maxLength);
		*(int *)data = maxLength;
		break;

	case SET_TEXT_LENGTH:
		// Set the string text length.
		UIW_STRING::DataSet(text, *(int *)data);
		break;

	case GET_FLAGS:
	case SET_FLAGS:
	case CLEAR_FLAGS:
		// Get, set or clear the button status settings.
		if (objectID && objectID != ID_STRING)
			data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		else if (request == GET_FLAGS && !data)
			data = &stFlags;
		else if (request == GET_FLAGS)
			*(STF_FLAGS *)data = stFlags;
		else if (request == SET_FLAGS)
			stFlags |= *(STF_FLAGS *)data;
		else
			stFlags &= ~(*(STF_FLAGS *)data);
		break;

	default:
		data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

char *UIW_STRING::ParseRange(char *buffer, char *minValue, char *maxValue)
{
	// Parse the minimum value.
	int position = 0, offset = 0;
	while (buffer[offset] != '\0' &&
		(buffer[offset] != '.' || buffer[offset+1] != '.') &&
		buffer[offset] != '/')
		minValue[position++] = buffer[offset++];
	minValue[position] = '\0';

	// See if it is a standalone value.
	if (buffer[offset] == '/' || buffer[offset] == '\0')
	{
		strcpy(maxValue, minValue);
		return (buffer[offset] ? &buffer[++offset] : NULL);
	}

	// Increment the offset.
	while (buffer[offset] == '.')
		offset++;

	// Parse the maximum value.
	position = 0;
	while (buffer[offset] != '\0' && buffer[offset] != '/')
		maxValue[position++] = buffer[offset++];
	maxValue[position] = '\0';

	// Return the offset position.
	return (buffer[offset] ? &buffer[++offset] : NULL);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_PERSISTENCE)
UIW_STRING::UIW_STRING(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	UI_WINDOW_OBJECT(0, 0, 15, 1, WOF_NO_FLAGS, WOAF_NO_FLAGS), text(NULL)
{
	// Initialize the string information.
	UIW_STRING::Load(name, directory, file);
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
	UIW_STRING::Information(INITIALIZE_CLASS, NULL);
	UIW_STRING::DataSet(text);
}

void UIW_STRING::Load(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Load the string information.
	UI_WINDOW_OBJECT::Load(name, directory, file);
	file->Load(&stFlags);
	short _value; file->Load(&_value); maxLength = _value;
	if (text)
		delete text;
	text = new char[maxLength+1]; file->Load(text, maxLength);
#if defined(ZIL_MSDOS)
	if (!FlagSet(stFlags, STF_SUBSTRING))
		length = ui_strlen(text);
#endif
}

void UIW_STRING::Store(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Store the string information.
	UI_WINDOW_OBJECT::Store(name, directory, file);
	file->Store(stFlags);
	short _value = maxLength; file->Store(_value);
	file->Store(text);
}
#endif


