//	Zinc Interface Library - Z_GROUP.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define INCL_WINSTATICS			// OS/2 static messages and flags.
#include <ctype.h>
#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_GROUP ----------------------------------------------------------

UIW_GROUP::UIW_GROUP(int left, int top, int width, int height, char *_text,
	WNF_FLAGS _wnFlags, WOF_FLAGS _woFlags) :
	UIW_WINDOW(left, top, width, height, _woFlags), text(NULL)
{
	// Initialize the group information.
	wnFlags |= _wnFlags;
	DataSet(_text);
	UIW_GROUP::Information(INITIALIZE_CLASS, NULL);
}

UIW_GROUP::~UIW_GROUP(void)
{
	if (text && !FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete text;
}

char *UIW_GROUP::DataGet(void)
{
	return (text);
}

void UIW_GROUP::DataSet(char *_text)
{
	// Reset the group string and check for a hot key.
	if (text && text != _text && !FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete text;
	if (FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		text = _text;
	else
		text = ui_strdup(_text);

	// Check for a hotkey.
	hotKey = HotKey(text);
	if (!hotKey)
		hotKey = HOT_KEY_SUB_WINDOW;

	// Redisplay the string.
	if (screenID)
	{
#if defined(ZIL_MSDOS)
		Event(UI_EVENT(S_REDISPLAY));
#elif defined(ZIL_MSWINDOWS)
		InvalidateRect(screenID, NULL, TRUE);
		SendMessage(screenID, WM_SETTEXT, 0, (LONG)text);
#elif defined(ZIL_OS2)
		WinSetWindowText(screenID, (PSZ)text);
#elif defined(ZIL_MOTIF)
		nargs = 0;
		XmString xms;
		XtSetArg(args[nargs], XmNlabelString, 
			xms = XmStringCreate(text ? text : "",
				XmSTRING_DEFAULT_CHARSET)); nargs++;
		XtSetValues(labelWidget, args, nargs);
		XmStringFree(xms);
		Event(UI_EVENT(S_REDISPLAY));
#endif
	}
}

void *UIW_GROUP::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_GROUP;
	switch (request)
	{
	case INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = windowID[0] = ID_GROUP;
		windowID[1] = ID_WINDOW;
		hotKey = HotKey(text);
		if (!hotKey)
			hotKey = HOT_KEY_SUB_WINDOW;
		// Continue to CHANGED_FLAGS.

	case CHANGED_FLAGS:
		// Check the object and base class flag settings.
		if (request == CHANGED_FLAGS)
			UIW_WINDOW::Information(CHANGED_FLAGS, data, ID_GROUP);

#if defined(ZIL_MSWINDOWS)
		dwStyle |= BS_GROUPBOX;
#elif defined(ZIL_OS2)
		if (FlagSet(woFlags, WOF_NON_SELECTABLE))
			flStyle |= WS_DISABLED;											// BUG.1780
#endif

		// See if the field needs to be re-computed.
		if (objectID == ID_GROUP && FlagSet(woStatus, WOS_REDISPLAY))
		{
			UI_EVENT event(S_INITIALIZE, 0);
			Event(event);
			event.type = S_CREATE;
			Event(event);
		}
		break;

	case GET_TEXT:
		// Get the group text.
		if (!data)
			return (DataGet());
		*(char **)data = DataGet();
		break;

	case COPY_TEXT:
		// Copy the group text.
		data = strcpy((char *)data, DataGet());
		break;

	case SET_TEXT:
		// Set the group text.
		DataSet((char *)data);
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
UIW_GROUP::UIW_GROUP(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	UIW_WINDOW(0, 0, 7, 1, WOF_NO_FLAGS, WOAF_NO_FLAGS)
{
	// Initialize the group information.
	Load(name, directory, file);
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
	UIW_WINDOW::Information(INITIALIZE_CLASS, NULL);
	UIW_GROUP::Information(INITIALIZE_CLASS, NULL);
}

void UIW_GROUP::Load(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Load the group information.
	UIW_WINDOW::Load(name, directory, file);
	file->Load(&text);
}

void UIW_GROUP::Store(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Store the group information.
	UIW_WINDOW::Store(name, directory, file);
	file->Store(text);
}
#endif


