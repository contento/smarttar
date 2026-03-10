//	Zinc Interface Library - Z_POPUP.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define INCL_WINMENUS			// OS/2 menu messages and flags.
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_POP_UP_MENU ----------------------------------------------------

UIW_POP_UP_MENU::UIW_POP_UP_MENU(int left, int top, WNF_FLAGS _wnFlags,
	WOF_FLAGS _woFlags, WOAF_FLAGS _woAdvancedFlags) :
	UIW_WINDOW(left, top, 0, 0, _woFlags, _woAdvancedFlags)
{
	// Initialize the menu information.
	wnFlags = _wnFlags;
	if (FlagSet(wnFlags, WNF_AUTO_SORT))
		UIW_WINDOW::Information(INITIALIZE_CLASS, NULL);					// BUG.1572
	UIW_POP_UP_MENU::Information(INITIALIZE_CLASS, NULL);
}

UIW_POP_UP_MENU::UIW_POP_UP_MENU(int left, int top, WNF_FLAGS _wnFlags, UI_ITEM *item) :
	UIW_WINDOW(left, top, 0, 0, WOF_BORDER)
{
	// Initialize the menu information.
	wnFlags = _wnFlags;
	UIW_POP_UP_MENU::Information(INITIALIZE_CLASS, NULL);
	if (FlagSet(wnFlags, WNF_AUTO_SORT))
		UIW_WINDOW::Information(INITIALIZE_CLASS, NULL);					// BUG.1572

	// Add the pop-up menu items.
	for (int i = 0; item[i].text; i++)
		Add(new UIW_POP_UP_ITEM(item[i].text, item[i].flags, BTF_NO_3D,
			WOF_NO_FLAGS, (USER_FUNCTION)item[i].data, item[i].value));
}

UIW_POP_UP_MENU::~UIW_POP_UP_MENU(void)
{
	// No menu information needs to be destroyed.
}

void *UIW_POP_UP_MENU::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_POP_UP_MENU;
	switch (request)
	{
	case INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = windowID[0] = ID_POP_UP_MENU;
		windowID[1] = ID_MENU;
		windowID[2] = ID_WINDOW;
		wnFlags |= WNF_CONTINUE_SELECT;
		font = FNT_SYSTEM_FONT;
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
UIW_POP_UP_MENU::UIW_POP_UP_MENU(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) : 
	UIW_WINDOW(0, 0, 20, 6, WOF_NO_FLAGS, WOAF_NO_FLAGS)
{
	// Initialize the menu information.
	UIW_POP_UP_MENU::Load(name, directory, file);
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
	UIW_WINDOW::Information(INITIALIZE_CLASS, NULL);
	UIW_POP_UP_MENU::Information(INITIALIZE_CLASS, NULL);
}

void UIW_POP_UP_MENU::Load(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Load the pop-up menu information.
	UIW_WINDOW::Load(name, directory, file);
}

void UIW_POP_UP_MENU::Store(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Store the pop-up menu information.
	UIW_WINDOW::Store(name, directory, file);
}
#endif

