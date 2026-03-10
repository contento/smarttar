//	Zinc Interface Library - Z_PLLDN.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define INCL_WINMENUS			// OS/2 menu messages and flags.
#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_PULL_DOWN_MENU -------------------------------------------------

UIW_PULL_DOWN_MENU::UIW_PULL_DOWN_MENU(int _indentation, WOF_FLAGS _woFlags,
	WOAF_FLAGS _woAdvancedFlags) :
	UIW_WINDOW(0, 0, 0, 0, _woFlags | WOF_SUPPORT_OBJECT, _woAdvancedFlags),
	indentation(_indentation)
{
	// Initialize the pull-down menu information.
	UIW_PULL_DOWN_MENU::Information(INITIALIZE_CLASS, NULL);
	if (FlagSet(wnFlags, WNF_AUTO_SORT))
		UIW_WINDOW::Information(INITIALIZE_CLASS, NULL);					// BUG.1572
}

UIW_PULL_DOWN_MENU::UIW_PULL_DOWN_MENU(int _indentation, UI_ITEM *item) :
	UIW_WINDOW(0, 0, 0, 0, WOF_BORDER | WOF_NON_FIELD_REGION | WOF_SUPPORT_OBJECT, WOAF_NO_FLAGS),
	indentation(_indentation)
{
	// Initialize the pull-down menu information.
	UIW_PULL_DOWN_MENU::Information(INITIALIZE_CLASS, NULL);
	if (FlagSet(wnFlags, WNF_AUTO_SORT))
		UIW_WINDOW::Information(INITIALIZE_CLASS, NULL);					// BUG.1572

	// Add the pull-down menu items.
	for (int i = 0; item[i].text; i++)
		Add(new UIW_PULL_DOWN_ITEM(item[i].text, item[i].flags,
			(USER_FUNCTION)item[i].data, item[i].value));
}

UIW_PULL_DOWN_MENU::~UIW_PULL_DOWN_MENU(void)
{
	// No menu information needs to be destroyed.
}

void *UIW_PULL_DOWN_MENU::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_PULL_DOWN_MENU;
	switch (request)
	{
	case INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = windowID[0] = ID_PULL_DOWN_MENU;
		windowID[1] = ID_MENU;
		windowID[2] = ID_WINDOW;
		font = FNT_SYSTEM_FONT;
		hotKey = HOT_KEY_SUB_WINDOW;
		wnFlags |= WNF_CONTINUE_SELECT;
		if (indentation)
		{
			char string[64];
			memset(string, ' ', indentation);
			string[indentation] = '\0';
			UIW_PULL_DOWN_ITEM *item = new UIW_PULL_DOWN_ITEM(string);
			item->woFlags |= WOF_NON_SELECTABLE;
			Add(item);
		}
#if defined(ZIL_OS2)
		flStyle |= MS_ACTIONBAR;
#endif
		break;

#if defined(ZIL_MSDOS)
	case CHANGED_FLAGS:
		if (screenID && objectID == ID_PULL_DOWN_MENU)
			return (parent->Information(request, data));
		break;
#elif defined(ZIL_MSWINDOWS) || defined(ZIL_OS2)
	case CHANGED_FLAGS:
		if (parent)
			parent->Information(SET_MENU, &screenID);
		break;
#endif

	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_PERSISTENCE)
UIW_PULL_DOWN_MENU::UIW_PULL_DOWN_MENU(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	UIW_WINDOW(0, 0, 0, 0, WOF_BORDER | WOF_SUPPORT_OBJECT | WOF_NON_FIELD_REGION,
	WOAF_NO_FLAGS),
	indentation(0)
{
	// Initialize the pull-down menu information.
	Load(name, directory, file);
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
	UIW_WINDOW::Information(INITIALIZE_CLASS, NULL);
	UIW_PULL_DOWN_MENU::Information(INITIALIZE_CLASS, NULL);
}

void UIW_PULL_DOWN_MENU::Load(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Read the file information.
	UIW_WINDOW::Load(name, directory, file);
}

void UIW_PULL_DOWN_MENU::Store(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Store the pull-down menu information.
	UIW_WINDOW::Store(name, directory, file);
}
#endif

