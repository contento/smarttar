//	Zinc Interface Library - Z_TITLE.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_TITLE ----------------------------------------------------------

UIW_TITLE::UIW_TITLE(char *text, WOF_FLAGS _woFlags) :
	UIW_BUTTON(0, 0, 0, text, BTF_DOWN_CLICK | BTF_NO_TOGGLE | BTF_NO_3D,
	_woFlags | WOF_BORDER | WOF_SUPPORT_OBJECT | WOF_NON_FIELD_REGION)
{
	// Initialize the title information.
	UIW_TITLE::Information(INITIALIZE_CLASS, NULL);
}

UIW_TITLE::~UIW_TITLE(void)
{
	// No title information needs to be destroyed.
}

char *UIW_TITLE::DataGet(void)
{
	// Return the text information.
 	return (text);
}

void UIW_TITLE::DataSet(char *_text)
{
	// Reset the title.
	if (text && text != _text && !FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete text;
	if (text == _text || FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		text = _text;
	else
		text = ui_strdup(_text);

	// Redisplay the string.
	if (parent->screenID)
	{
#if defined(ZIL_MSDOS) || defined(ZIL_MOTIF)
		Event(UI_EVENT(S_REDISPLAY));
#elif defined(ZIL_MSWINDOWS)
		SendMessage(parent->screenID, WM_SETTEXT, 0, (LONG)text);
#elif defined(ZIL_OS2)
		WinSetWindowText(parent->screenID, (PSZ)text);
#endif
	}
}

void *UIW_TITLE::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_TITLE;
	switch (request)
	{
	case INITIALIZE_CLASS:
		// Set the object identification and initial title settings.
		searchID = windowID[0] = ID_TITLE;
		windowID[1] = ID_BUTTON;
		numberID = NUMID_TITLE;
		strcpy(stringID, "NUMID_TITLE");
		woAdvancedFlags |= WOAF_NON_CURRENT;
		font = FNT_SYSTEM_FONT | IGNORE_UNDERSCORE;
		depth = 0;
		break;

#if defined(ZIL_MSWINDOWS)
	case CHANGED_FLAGS:
		parent->Information(SET_TITLE, this);
		parent->Information(SET_DWSTYLE, &dwStyle);
		break;
#elif defined(ZIL_OS2)
	case CHANGED_FLAGS:
		parent->Information(SET_TITLE, this);
		parent->Information(SET_FLFLAG, &flFlag);
		break;
#endif

	case GET_TEXT:
		// Get the title text.
		if (!data)
			return (DataGet());
		*(char **)data = DataGet();
		break;

	case COPY_TEXT:
		// Copy the title text.
		data = strcpy((char *)data, DataGet() ? DataGet() : "");			// Bug.INTERNAL
		break;

	case SET_TEXT:
		// Set the title text.
		DataSet((char *)data);
		break;

	default:
		data = UIW_BUTTON::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_PERSISTENCE)
UIW_TITLE::UIW_TITLE(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	UIW_BUTTON(0, 0, 0, NULL, BTF_DOWN_CLICK | BTF_NO_TOGGLE | BTF_NO_3D,
	WOF_BORDER | WOF_JUSTIFY_CENTER | WOF_SUPPORT_OBJECT | WOF_NON_FIELD_REGION)
{
	// Initialize the title information.
	UIW_TITLE::Load(name, directory, file);
	UIW_TITLE::Information(INITIALIZE_CLASS, NULL);
}

#if defined(__HIGHC__)
void UIW_TITLE::Load(const char *dummy1, UI_STORAGE *dummy2, UI_STORAGE_OBJECT *file)
#else
void UIW_TITLE::Load(const char *, UI_STORAGE *, UI_STORAGE_OBJECT *file)
#endif
{
	// Load the title information.
	file->Load(&text);
}

#if defined(__HIGHC__)
void UIW_TITLE::Store(const char *dummy1, UI_STORAGE *dummy2, UI_STORAGE_OBJECT *file)
#else
void UIW_TITLE::Store(const char *, UI_STORAGE *, UI_STORAGE_OBJECT *file)
#endif
{
	// Store the title information.
	file->Store(text);

	// Flag the object as being used.
	if (objectTable)
	{
		for (int i = 0; objectTable[i].data; i++)
			if (objectTable[i].value == searchID)
			{
				objectTable[i].flags = TRUE;
				break;
			}
	}
}
#endif


