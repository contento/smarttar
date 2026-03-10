//	Zinc Interface Library - Z_HLIST.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define INCL_WINLISTBOXES		// OS/2 list messages and flags.
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_HZ_LIST --------------------------------------------------------

UIW_HZ_LIST::UIW_HZ_LIST(int left, int top, int width, int height,
	int _cellWidth, int _cellHeight,
	int (*_compareFunction)(void *element1, void *element2),
	WNF_FLAGS _wnFlags, WOF_FLAGS _woFlags, WOAF_FLAGS _woAdvancedFlags) :
	UIW_WINDOW(left, top, width, height, _woFlags, _woAdvancedFlags),
	cellWidth(_cellWidth), cellHeight(_cellHeight)
{
	// Initialize the menu information.
	wnFlags = _wnFlags;
	compareFunction = _compareFunction;
	UIW_HZ_LIST::Information(INITIALIZE_CLASS, NULL);
}

UIW_HZ_LIST::UIW_HZ_LIST(int left, int top, int width, int height,
	int (*_compareFunction)(void *element1, void *element2), 
	WOF_FLAGS flagSetting, UI_ITEM *item) :
	UIW_WINDOW(left, top, width, height, WOF_BORDER, WOAF_NO_FLAGS)
{
	// Initialize the list information.
	wnFlags |= WNF_NO_WRAP;
	compareFunction = _compareFunction;
	UIW_HZ_LIST::Information(INITIALIZE_CLASS, NULL);

	// Add the string items.
	for (int i = 0; item[i].text; i++)
	{
		UIW_STRING *string = new UIW_STRING(0, 0, width, item[i].text, -1,
			item[i].flags, WOF_BORDER | WOF_AUTO_CLEAR, (USER_FUNCTION)item[i].data);
		if (FlagSet(flagSetting, item[i].value))
			string->woStatus |= WOS_SELECTED;
		UIW_WINDOW::Add(string);
	}
}

UIW_HZ_LIST::~UIW_HZ_LIST(void)
{
	// No list information needs to be destroyed.
}

void UIW_HZ_LIST::Destroy(void)
{
	// Destroy the list elements (high level).
	if (screenID)
	{
#if defined(ZIL_MSWINDOWS)
		SendMessage(screenID, WM_SETREDRAW, FALSE, 0);
		SendMessage(screenID, LB_RESETCONTENT, 0, 0);
#elif defined(ZIL_OS2) || defined(ZIL_MOTIF)
		UIW_WINDOW::Destroy();
#endif
	}

	// Destroy the list elements (low level).
	UI_LIST::Destroy();
}

void *UIW_HZ_LIST::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_HZ_LIST;
	switch (request)
	{
	case INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = windowID[0] = ID_HZ_LIST;
		windowID[1] = ID_LIST;
		windowID[2] = ID_WINDOW;
		font = FNT_DIALOG_FONT;
		// Continue to CHANGED_FLAGS.

	case CHANGED_FLAGS:
		// Check the flag settings.
		if (request == CHANGED_FLAGS)
			UIW_WINDOW::Information(CHANGED_FLAGS, data, ID_HZ_LIST);
		wnFlags |= WNF_CONTINUE_SELECT;
		if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
			woStatus |= WOS_OWNERDRAW;
		else
			woStatus &= ~WOS_OWNERDRAW;

		// Check the environment specific flag settings.
#if defined(ZIL_MSWINDOWS)
		dwStyle |= LBS_NOINTEGRALHEIGHT | LBS_MULTICOLUMN | LBS_NOTIFY;
		if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
			dwStyle |= LBS_OWNERDRAWFIXED;
//		if (FlagSet(wnFlags, WNF_SELECT_MULTIPLE))							// BUG.1346
		if (FlagSet(wnFlags, WNF_SELECT_MULTIPLE) || !FlagSet(wnFlags, WNF_AUTO_SELECT))	
			dwStyle |= LBS_MULTIPLESEL;
		if (compareFunction)
			dwStyle |= LBS_SORT;
#endif

		// See if the field needs to be re-computed.
		if (objectID == ID_HZ_LIST && FlagSet(woStatus, WOS_REDISPLAY))
		{
			UI_EVENT event(S_INITIALIZE, 0);
			Event(event);
			event.type = S_CREATE;
			Event(event);
		}
		break;

	case GET_TEXT:
	case GET_BITMAP_ARRAY:
	case SET_BITMAP_ARRAY:
		return (current ? Current()->Information(request, data, objectID) : NULL);

	case DESTROY_LIST:
		Destroy();
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
UIW_HZ_LIST::UIW_HZ_LIST(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	UIW_WINDOW(0, 0, 20, 6, WOF_NO_FLAGS)
{
	// Initialize the text information.
	UIW_HZ_LIST::Load(name, directory, file);
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
	UIW_WINDOW::Information(INITIALIZE_CLASS, NULL);
	UIW_HZ_LIST::Information(INITIALIZE_CLASS, NULL);
}

void UIW_HZ_LIST::Load(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Load the horizontal list information.
	UIW_WINDOW::Load(name, directory, file);
	short _value; file->Load(&_value); cellWidth = _value;
	file->Load(&_value); cellHeight = _value;
}

void UIW_HZ_LIST::Store(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Store the horizontal list information.
	UIW_WINDOW::Store(name, directory, file);
	short _value = cellWidth; file->Store(_value);
	_value = cellHeight; file->Store(_value);
}
#endif


