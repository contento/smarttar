//	Zinc Interface Library - Z_TEXT.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define INCL_WINMLE				// OS/2 multi-line messages and flags.
#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif
#if defined(ZIL_MOTIF)
#	include <Xm/Text.h>
#endif

// ----- UIW_TEXT -----------------------------------------------------------

UIW_TEXT::UIW_TEXT(int left, int top, int width, int height, char *_text,
	int _maxLength, WNF_FLAGS _wnFlags, WOF_FLAGS _woFlags,
	USER_FUNCTION _userFunction) :
#if defined(ZIL_MSDOS)
	UIW_WINDOW(left, top, width, height, _woFlags, WOAF_NO_FLAGS),
	iString(0, 0, 0, NULL, 0, STF_NO_FLAGS, WOF_BORDER | WOF_AUTO_CLEAR | WOF_NO_ALLOCATE_DATA),
	maxLength(_maxLength), text(NULL)
#elif defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(ZIL_MOTIF)
	UIW_WINDOW(left, top, width, height, _woFlags, WOAF_NO_FLAGS),
	maxLength(_maxLength), text(NULL)
#endif
{
	// Initialize the text information.
	wnFlags |= _wnFlags;
	if (maxLength == -1)
		maxLength = _text ? ui_strlen(_text) + 1 : width * height;
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		text = new char[maxLength+1];
	userFunction = _userFunction;
	UIW_TEXT::Information(INITIALIZE_CLASS, NULL);
	UIW_TEXT::DataSet(_text);
}

UIW_TEXT::~UIW_TEXT(void)
{
#if defined(ZIL_MSDOS)
	iString.text = NULL;
#endif
	if (text && !FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete text;
}

char *UIW_TEXT::DataGet(void)
{
#if defined(ZIL_MSWINDOWS)
	if (screenID && !FlagSet(woFlags, WOF_VIEW_ONLY))
		SendMessage(screenID, WM_GETTEXT, maxLength, (LONG)text);
#elif defined(ZIL_OS2)
	if (screenID && !FlagSet(woFlags, WOF_VIEW_ONLY))
		WinQueryWindowText(screenID, maxLength, (PSZ)text);
#elif defined(ZIL_MOTIF)
	if (screenID && !FlagSet(woFlags, WOF_VIEW_ONLY))
	{
		char *tText = XmTextGetString(screenID);
		strcpy(text, tText);
		XtFree(tText);
	}
#endif

	// Return the text information.
	return (text);
}

void UIW_TEXT::DataSet(char *_text, int _maxLength)
{
	// Reset the length.
	if (FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		maxLength = (_maxLength != -1) ? _maxLength : maxLength;
	else if (_maxLength != -1)
	{
		if (_maxLength > maxLength)
		{
			char *tText = new char[_maxLength+1];
			strncpy(tText, text, maxLength);
			tText[maxLength] = '\0';
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
		strncpy(text, _text ? _text : "", maxLength);
		text[maxLength] = '\0';
	}
#if defined(ZIL_MSDOS)
	iString.maxLength = maxLength;
	topLine = iString.text = text;
	iString.length = ui_strlen(text);
//	iString.cursor = 0;														// Bug.1443
	iString.cursor = FlagSet(woFlags, WOF_AUTO_CLEAR) ? iString.length : 0;
	iString.beginMark = iString.endMark = -1;
	if (first)
		RecomputeFields(FALSE, TRUE);
#endif

#if defined(ZIL_MOTIF)
	// kill CR/LF characters.
	ui_strstrip(text, '\r');
#endif

	// Redisplay the text field.
	if (screenID)
	{
#if defined(ZIL_MSDOS)
		Event(S_VSCROLL_CHECK);
		Event(UI_EVENT(S_REDISPLAY));
#elif defined(ZIL_MSWINDOWS)
		InvalidateRect(screenID, NULL, TRUE);
		if (_maxLength != -1)
			SendMessage(screenID, EM_LIMITTEXT, maxLength - 1, FALSE);
		SendMessage(screenID, WM_SETTEXT, 0, (LONG)text);
#elif defined(ZIL_OS2)
		WinInvalidateRect(screenID, NULL, TRUE);
		if (_maxLength != -1)
			WinSendMsg(screenID, MLM_SETTEXTLIMIT, (MPARAM)maxLength, (MPARAM)0);
		WinSetWindowText(screenID, (PSZ)text);
#elif defined(ZIL_MOTIF)
		nargs = 0;
		if (_maxLength != -1)
			XtSetArg(args[nargs], XmNmaxLength, _maxLength), nargs++;
		XtSetArg(args[nargs], XmNvalue, text); nargs++;
		XtSetValues(screenID, args, nargs);
#endif
	}
}

void *UIW_TEXT::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_TEXT;
	switch (request)
	{
	case INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = windowID[0] = ID_TEXT;
		windowID[1] = ID_STRING;
		windowID[2] = ID_WINDOW;
		font = FNT_DIALOG_FONT;
		insertMode = TRUE;

#if defined(ZIL_MSDOS)
		iString.maxLength = maxLength;
		topLine = iString.text = text;
		iString.length = ui_strlen(text);
		iString.cursor = 0;
		iString.beginMark = iString.endMark = -1;
		iString.insertMode = insertMode;
		iString.woAdvancedFlags = WOAF_NO_DESTROY;
#endif
		// Continue to CHANGED_FLAGS.

	case CHANGED_FLAGS:
		if (request == CHANGED_FLAGS)
			UIW_WINDOW::Information(CHANGED_FLAGS, data, ID_TEXT);

		// Check the environment specific flag settings.
#if defined(ZIL_MSWINDOWS)
		dwStyle |= ES_AUTOVSCROLL | ES_MULTILINE;
		if (FlagSet(wnFlags, WNF_NO_WRAP))
			dwStyle |= ES_AUTOHSCROLL;
		if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
			dwStyle |= ES_CENTER;
		else if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
			dwStyle |= ES_RIGHT;
#elif defined(ZIL_OS2)
		if (!FlagSet(woFlags, WOF_NON_FIELD_REGION))
			flStyle |= MLS_IGNORETAB;
		if (!FlagSet(wnFlags, WNF_NO_WRAP))
			flStyle |= MLS_WORDWRAP;
		if (FlagSet(woFlags, WOF_BORDER))
			flStyle |= MLS_BORDER;
		if (FlagSet(woFlags, WOF_VIEW_ONLY))								// BUG.1530
			flStyle |= MLS_READONLY;
#endif

		// See if the field needs to be re-computed.
		if (objectID == ID_TEXT && FlagSet(woStatus, WOS_REDISPLAY))
		{
			UI_EVENT event(S_INITIALIZE, 0);
			Event(event);
			event.type = S_CREATE;
			Event(event);
		}
		wnFlags |= WNF_CONTINUE_SELECT;
		break;

	case GET_TEXT:
		// Get the text.
		if (!data)
			return (DataGet());
		*(char **)data = DataGet();
		break;

	case COPY_TEXT:
		data = strcpy((char *)data, DataGet());
		break;

	case SET_TEXT:
		// Set the text.
		DataSet((char *)data);
		break;

	case GET_TEXT_LENGTH:
		// Get the text length.
		if (!data)
			return (&maxLength);
		*(int *)data = maxLength;
		break;

	case SET_TEXT_LENGTH:
		// Set the text length.
		DataSet(text, *(int *)data);
		break;

	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_PERSISTENCE)
UIW_TEXT::UIW_TEXT(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
#if defined(ZIL_MSDOS)
	UIW_WINDOW(0, 0, 20, 6, WOF_NO_FLAGS), text(NULL),
	iString(0, 0, 0, NULL, 0, STF_NO_FLAGS, WOF_BORDER | WOF_AUTO_CLEAR | WOF_NO_ALLOCATE_DATA)
#elif defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(ZIL_MOTIF)
	UIW_WINDOW(0, 0, 20, 6, WOF_NO_FLAGS), text(NULL)
#endif
{
	// Initialize the text information.
	UIW_TEXT::Load(name, directory, file);
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
	UIW_WINDOW::Information(INITIALIZE_CLASS, NULL);
	UIW_TEXT::Information(INITIALIZE_CLASS, NULL);
	UIW_TEXT::DataSet(text);
}

void UIW_TEXT::Load(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Load the text information.
	UI_WINDOW_OBJECT::Load(name, directory, file);
	short _value; file->Load(&_value); maxLength = _value;
	text = new char[maxLength+1]; file->Load(text, maxLength);
	short noOfObjects;
	file->Load(&noOfObjects);
	for (int i = 0; i < noOfObjects; i++)
		Add(UI_WINDOW_OBJECT::New(NULL, directory, file));
	file->Load(&wnFlags);
}

void UIW_TEXT::Store(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Store the text information.
	UI_WINDOW_OBJECT::Store(name, directory, file);
	short _value = maxLength; file->Store(_value);
	file->Store(text);
	short noOfObjects = support.Count();
	file->Store(noOfObjects);
	for (UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
	{
		_value = object->SearchID();
		file->Store(_value);
		object->Store(NULL, directory, file);
	}
	file->Store(wnFlags);
}
#endif


