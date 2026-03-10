//	Zinc Interface Library - Z_PROMPT.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define INCL_WINSTATICS			// OS/2 static messages and flags.
#include <ctype.h>
#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_PROMPT ---------------------------------------------------------

UIW_PROMPT::UIW_PROMPT(int left, int top, char *_text, WOF_FLAGS _woFlags) :
	UI_WINDOW_OBJECT(left, top, 0, 1, (_woFlags & ~(WOF_JUSTIFY_RIGHT|WOF_JUSTIFY_CENTER)) | WOF_NON_SELECTABLE, WOAF_NO_FLAGS),	// BUG.1487
	text(NULL)
{
	// Initialize the prompt information.
	UIW_PROMPT::Information(INITIALIZE_CLASS, NULL);
	UIW_PROMPT::DataSet(_text);
}

UIW_PROMPT::UIW_PROMPT(int left, int top, int width, char *_text, WOF_FLAGS _woFlags) :
	UI_WINDOW_OBJECT(left, top, width, 1, _woFlags | WOF_NON_SELECTABLE, WOAF_NO_FLAGS),
	text(NULL)
{
	// Initialize the prompt information.
	UIW_PROMPT::Information(INITIALIZE_CLASS, NULL);
	UIW_PROMPT::DataSet(_text);
}

UIW_PROMPT::~UIW_PROMPT(void)
{
	if (text && !FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete text;
}

char *UIW_PROMPT::DataGet(void)
{
	return (text);
}

void UIW_PROMPT::DataSet(char *_text)
{
	// Reset the prompt and check for a hot key.
	if (text && text != _text && !FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete text;
	if (text == _text || FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		text = _text;
	else
		text = ui_strdup(_text);
	hotKey = HotKey(text);

	// Redisplay the prompt.
	if (screenID)
	{
#if defined(ZIL_MSDOS)
		display->Rectangle(screenID, true, lastPalette, 0, TRUE);
#elif defined(ZIL_MSWINDOWS)
		InvalidateRect(screenID, NULL, TRUE);
//		SendMessage(screenID, WM_SETTEXT, 0, (LONG)text);
		SendMessage(screenID, WM_SETTEXT, 0, 0);
#elif defined(ZIL_OS2)
		WinInvalidateRect(screenID, NULL, TRUE);
		WinSetWindowText(screenID, (PSZ)text);
#elif defined(ZIL_MOTIF)
		XmString labelText = XmStringCreateLtoR(text ? text : "", 
			XmSTRING_DEFAULT_CHARSET);
		nargs = 0;
		XtSetArg(args[nargs], XmNlabelString, labelText); nargs++;
		XtSetValues(screenID, args, nargs);
		if (labelText)
			XmStringFree(labelText);
#endif
		Event(UI_EVENT(S_SIZE, 0));
		Event(UI_EVENT(S_REDISPLAY, 0));
	}
}

void *UIW_PROMPT::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_PROMPT;
	switch (request)
	{
	case INITIALIZE_CLASS:
		// Set the object identification and check for a hotkey.
		searchID = windowID[0] = ID_PROMPT;
		hotKey = HotKey(text);
		// Continue to CHANGED_FLAGS.

	case CHANGED_FLAGS:
		// Check the base class flag settings.
		if (request == CHANGED_FLAGS)
			UI_WINDOW_OBJECT::Information(CHANGED_FLAGS, data, ID_PROMPT);

		// Check the environment specific flag settings.
#if defined(ZIL_MSWINDOWS)
		if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
			dwStyle |= SS_RIGHT;
		else
			dwStyle |= SS_LEFT;
#elif defined(ZIL_OS2)
		font = FNT_SYSTEM_FONT;
		flStyle |= SS_TEXT | DT_LEFT | DT_VCENTER | DT_MNEMONIC;
#endif

		// See if the field needs to be re-displayed.
		if (objectID == ID_PROMPT && FlagSet(woStatus, WOS_REDISPLAY))
		{
			UI_EVENT event(S_INITIALIZE, 0);
			Event(event);
			event.type = S_CREATE;
			Event(event);
		}
		break;

	case GET_TEXT:
		// Get the prompt text.
		if (!data)
			return (DataGet());
		*(char **)data = DataGet();
		break;

	case COPY_TEXT:
		// Copy the prompt text.
		data = strcpy((char *)data, DataGet());
		break;

	case SET_TEXT:
		// Set the prompt text.
		DataSet((char *)data);
		break;

	default:
		data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_PERSISTENCE)
UIW_PROMPT::UIW_PROMPT(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	UI_WINDOW_OBJECT(0, 0, 0, 1, WOF_NON_SELECTABLE, WOAF_NO_FLAGS)
{
	// Initialize the prompt information.
	UIW_PROMPT::Load(name, directory, file);
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
	UIW_PROMPT::Information(INITIALIZE_CLASS, NULL);
	UIW_PROMPT::DataSet(text);
}

void UIW_PROMPT::Load(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Read the prompt information.
	UI_WINDOW_OBJECT::Load(name, directory, file);
	file->Load(&text);
}

void UIW_PROMPT::Store(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Write the prompt information.
	UI_WINDOW_OBJECT::Store(name, directory, file);
	file->Store(text);
}
#endif


