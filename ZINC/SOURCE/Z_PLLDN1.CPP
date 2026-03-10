//	Zinc Interface Library - Z_PLLDN1.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define INCL_WINMENUS			// OS/2 menu messages and flags.
#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_PULL_DOWN_ITEM -------------------------------------------------

UIW_PULL_DOWN_ITEM::UIW_PULL_DOWN_ITEM(char *_text, WNF_FLAGS _wnFlags,
	USER_FUNCTION _userFunction, EVENT_TYPE _value) :
	UIW_BUTTON(0, 0, ui_strlen(_text), _text,
	BTF_DOWN_CLICK | BTF_NO_TOGGLE | BTF_NO_3D, WOF_JUSTIFY_CENTER,
	_userFunction, _value),
	menu(0, 0, _wnFlags, WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY)
{
	// Initialize the menu item information.
	UIW_PULL_DOWN_ITEM::Information(INITIALIZE_CLASS, NULL);
}

UIW_PULL_DOWN_ITEM::UIW_PULL_DOWN_ITEM(char *_text, WNF_FLAGS _wnFlags,
	UI_ITEM *item) :
	UIW_BUTTON(0, 0, ui_strlen(_text), _text,
	BTF_DOWN_CLICK | BTF_NO_TOGGLE | BTF_NO_3D, WOF_JUSTIFY_CENTER),
	menu(0, 0, _wnFlags, WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY)
{
	// Initialize the menu information.
	UIW_PULL_DOWN_ITEM::Information(INITIALIZE_CLASS, NULL);

	// Add the pop-up menu items.
	for (int i = 0; item[i].text; i++)
		Add(new UIW_POP_UP_ITEM(item[i].text, item[i].flags, BTF_NO_3D,
			WOF_NO_FLAGS, (USER_FUNCTION)item[i].data, item[i].value));
}

UIW_PULL_DOWN_ITEM::~UIW_PULL_DOWN_ITEM(void)
{
	// No item information needs to be destroyed.
}

void *UIW_PULL_DOWN_ITEM::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_PULL_DOWN_ITEM;
	switch (request)
	{
	case INITIALIZE_CLASS:
		// Set the object identification and variables.
#if defined(ZIL_MOTIF)
		UIW_BUTTON::Information(request, data, objectID);
#endif
		searchID = windowID[0] = ID_PULL_DOWN_ITEM;
		windowID[1] = ID_MENU_ITEM;
		windowID[2] = ID_BUTTON;
		font = FNT_SYSTEM_FONT;
		menu.parent = this;

#if defined(ZIL_OS2)
		flStyle = MIS_TEXT;
#endif
		break;

#if defined(ZIL_MSWINDOWS)
	case CHANGED_FLAGS:
		if (parent && parent->screenID)
		{
			WORD wFlags = MF_BYPOSITION;
			if (FlagSet(woFlags, WOF_NON_SELECTABLE))
				wFlags |= MF_GRAYED;
			else
				wFlags |= MF_ENABLED;
			EnableMenuItem(parent->screenID, ListIndex(), wFlags);
		}
		break;
#elif defined(ZIL_MSDOS)
	case CHANGED_FLAGS:							// BUG.1255
		if (screenID && FlagSet(woFlags, WOF_NON_SELECTABLE) &&
		    ((UIW_PULL_DOWN_MENU *)parent)->Current() == this)
		{
			// This sets the right menu item but
			// colors it active.
			parent->Event(UI_EVENT(L_RIGHT));
			// this uncolors it.
			parent->Event(UI_EVENT(S_REDISPLAY));
		}
		break;
#elif defined(ZIL_OS2)
	case CHANGED_FLAGS:														// BUG.2269
		if (parent && parent->screenID)
		{
			WinSendMsg(parent->screenID, MM_SETITEMATTR, MPFROM2SHORT(numberID, FALSE),
				MPFROM2SHORT(MIA_DISABLED, FlagSet(woFlags, WOF_NON_SELECTABLE) ? MIA_DISABLED : 0));
			parent->Event(UI_EVENT(S_REDISPLAY));
		}
		break;
#endif

	case GET_NUMBERID_OBJECT:
	case GET_STRINGID_OBJECT:
		// Get the field that matches the identification.
		{
		void *match = UI_WINDOW_OBJECT::Information(request, data, objectID);
		if (!match)
			match = menu.Information(request, data, objectID);
		data = match;
		}
		break;

	case PRINT_INFORMATION:
	case PRINT_USER_FUNCTION:
		UI_WINDOW_OBJECT::Information(request, data, objectID);
		menu.Information(request, data, objectID);
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
UIW_PULL_DOWN_ITEM::UIW_PULL_DOWN_ITEM(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	UIW_BUTTON(0, 0, 10, NULL, BTF_DOWN_CLICK | BTF_NO_TOGGLE | BTF_NO_3D,
	WOF_JUSTIFY_CENTER),
	menu(0, 0, WNF_NO_FLAGS, WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY)
{
	// Initialize the pull-down item information.
	UIW_PULL_DOWN_ITEM::Load(name, directory, file);
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
	UIW_BUTTON::Information(INITIALIZE_CLASS, NULL);
	UIW_PULL_DOWN_ITEM::Information(INITIALIZE_CLASS, NULL);
	UIW_BUTTON::DataSet(text);
}

void UIW_PULL_DOWN_ITEM::Load(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Read the file information.
	UIW_BUTTON::Load(name, directory, file);
	menu.Load(NULL, directory, file);
}

void UIW_PULL_DOWN_ITEM::Store(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Store the pull-down item information.
	UIW_BUTTON::Store(name, directory, file);
	menu.Store(NULL, directory, file);
}
#endif


