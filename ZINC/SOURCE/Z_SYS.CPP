//	Zinc Interface Library - Z_SYS.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

/* START BLOCK COMMENT														// BUG.268.2
**	#if defined(ZIL_MSDOS)
**	UIW_POP_UP_MENU *_genericSystemMenu = NULL;
**	int _genericSystemMenuCount = 0;
**	#endif
END BLOCK COMMENT */

// ----- UIW_SYSTEM_BUTTON --------------------------------------------------

UIW_SYSTEM_BUTTON::UIW_SYSTEM_BUTTON(UI_ITEM *item) :
	UIW_BUTTON(0, 0, 0, NULL, BTF_NO_FLAGS | BTF_STATIC_BITMAPARRAY,
	WOF_BORDER | WOF_JUSTIFY_CENTER | WOF_SUPPORT_OBJECT | WOF_NON_FIELD_REGION),
	menu(0, 0, WNF_NO_FLAGS, WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY),
	syFlags(SYF_NO_FLAGS)
{
#if defined(ZIL_LINKBUG)
	extern void z_sys_dummy(void);		// Bug fix for broken linkers.
	z_sys_dummy();
#endif

	// Initialize the system button information.
	UIW_SYSTEM_BUTTON::Information(INITIALIZE_CLASS, NULL);

	// Add the pop-up menu items.
	for (int i = 0; item[i].text; i++)
		Add(new UIW_POP_UP_ITEM(item[i].text, item[i].flags, BTF_NO_3D,
			WOF_NO_FLAGS, (USER_FUNCTION)item[i].data, item[i].value));
}

UIW_SYSTEM_BUTTON::UIW_SYSTEM_BUTTON(SYF_FLAGS _syFlags) :
	UIW_BUTTON(0, 0, 0, NULL, BTF_NO_FLAGS,
	WOF_BORDER | WOF_JUSTIFY_CENTER | WOF_SUPPORT_OBJECT | WOF_NON_FIELD_REGION),
	menu(0, 0, WNF_NO_FLAGS, WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY),
	syFlags(_syFlags)
{
	// Initialize the system button information.
	UIW_SYSTEM_BUTTON::Information(INITIALIZE_CLASS, NULL);
}

UIW_SYSTEM_BUTTON::~UIW_SYSTEM_BUTTON(void)
{
	text = NULL;		   	// text is a static string.
	bitmapArray = NULL;		// bitmapArray is a static array.

/* START BLOCK COMMENT														// BUG.268.2
**	#if defined(ZIL_MSDOS)
**		// Check the status of the DOS generic menu.
**		if (FlagSet(syFlags, SYF_GENERIC) && !--_genericSystemMenuCount)
**		{
**	       	delete _genericSystemMenu;
**	       	_genericSystemMenu = NULL;
**		}
**	#endif
END BLOCK COMMENT */
}

UIW_SYSTEM_BUTTON *UIW_SYSTEM_BUTTON::Generic(void)
{
	return (new UIW_SYSTEM_BUTTON(SYF_GENERIC));
}

void *UIW_SYSTEM_BUTTON::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_SYSTEM_BUTTON;
	switch (request)
	{
	case INITIALIZE_CLASS:
		// Set the object identification and initial button settings.
		searchID = windowID[0] = ID_SYSTEM_BUTTON;
		windowID[1] = ID_BUTTON;
		menu.parent = this;
		numberID = NUMID_SYSTEM;
		strcpy(stringID, "NUMID_SYSTEM");
		woAdvancedFlags |= WOAF_NON_CURRENT;
		hotKey = HOT_KEY_SYSTEM;
		depth = 1;

/* START BLOCK COMMENT														// BUG.268.2
**	#if defined(ZIL_MSDOS)
**			// Initialize the generic system button.
**			if (FlagSet(syFlags, SYF_GENERIC))
**			{
**				++_genericSystemMenuCount;
**				if (!_genericSystemMenu)
**				{
**					_genericSystemMenu = new UIW_POP_UP_MENU(0, 0, WNF_NO_FLAGS, WOF_BORDER,
**						WOAF_TEMPORARY | WOAF_NO_DESTROY);
**					*_genericSystemMenu
**						+ new UIW_POP_UP_ITEM(sysPrompts[0], MNIF_RESTORE)
**						+ new UIW_POP_UP_ITEM(sysPrompts[1], MNIF_MOVE)
**						+ new UIW_POP_UP_ITEM(sysPrompts[2], MNIF_SIZE)
**						+ new UIW_POP_UP_ITEM(sysPrompts[3], MNIF_MINIMIZE)
**						+ new UIW_POP_UP_ITEM(sysPrompts[4], MNIF_MAXIMIZE)
**						+ new UIW_POP_UP_ITEM
**						+ new UIW_POP_UP_ITEM(sysPrompts[6], MNIF_CLOSE);
**				}
**			}
**	#endif
END BLOCK COMMENT */
		break;

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

#if defined(ZIL_MSWINDOWS)
	case CHANGED_FLAGS:
		parent->Information(SET_DWSTYLE, &dwStyle);
		break;
#elif defined(ZIL_OS2)
	case CHANGED_FLAGS:
		parent->Information(SET_FLFLAG, &flFlag);
		break;
#endif

	case GET_FLAGS:
	case SET_FLAGS:
	case CLEAR_FLAGS:
		// Get, set or clear the button flag settings.
		if (objectID && objectID != ID_SYSTEM_BUTTON)
			data = UIW_BUTTON::Information(request, data, objectID);
		else if (request == GET_FLAGS && !data)
			data = &syFlags;
		else if (request == GET_FLAGS)
			*(SYF_FLAGS *)data = syFlags;
		else if (request == SET_FLAGS)
			syFlags |= *(SYF_FLAGS *)data;
		else
			syFlags &= ~(*(SYF_FLAGS *)data);
		break;

	case PRINT_INFORMATION:
	case PRINT_USER_FUNCTION:
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
UIW_SYSTEM_BUTTON::UIW_SYSTEM_BUTTON(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	UIW_BUTTON(0, 0, 0, NULL, BTF_NO_FLAGS,
	WOF_BORDER | WOF_JUSTIFY_CENTER | WOF_SUPPORT_OBJECT | WOF_NON_FIELD_REGION),
	menu(0, 0, WNF_NO_FLAGS, WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY)
{
	// Initialize the system button information.
	UIW_SYSTEM_BUTTON::Load(name, directory, file);
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
	UIW_BUTTON::Information(INITIALIZE_CLASS, NULL);
	UIW_SYSTEM_BUTTON::Information(INITIALIZE_CLASS, NULL);
}

void UIW_SYSTEM_BUTTON::Load(const char *, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Load the system button information.
	file->Load(&syFlags);
	if (!FlagSet(syFlags, SYF_GENERIC))
		menu.Load(NULL, directory, file);
}

void UIW_SYSTEM_BUTTON::Store(const char *, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Store the system button information.
	file->Store(syFlags);
	if (!FlagSet(syFlags, SYF_GENERIC))
		menu.Store(NULL, directory, file);

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
