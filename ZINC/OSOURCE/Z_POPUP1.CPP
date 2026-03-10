//	Zinc Interface Library - Z_POPUP1.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stdio.h>

#define INCL_WINMENUS			// OS/2 menu messages and flags.
#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_POP_UP_ITEM ----------------------------------------------------

UIW_POP_UP_ITEM::UIW_POP_UP_ITEM(void) :
	UIW_BUTTON(0, 0, 1, NULL, BTF_NO_3D, WOF_NO_FLAGS),
	menu(0, 0, WNF_NO_FLAGS, WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY),
	mniFlags(MNIF_SEPARATOR)
{
	// Initialize the pull-down item seperator information.
	UIW_POP_UP_ITEM::Information(INITIALIZE_CLASS, NULL);
}

UIW_POP_UP_ITEM::UIW_POP_UP_ITEM(char *_text, MNIF_FLAGS _mniFlags,
	BTF_FLAGS _btFlags, WOF_FLAGS _woFlags, USER_FUNCTION _userFunction,
	EVENT_TYPE _value) :
	UIW_BUTTON(0, 0, 0, _text, _btFlags, _woFlags, _userFunction, _value),
	menu(0, 0, WNF_NO_FLAGS, WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY),
	mniFlags(_mniFlags)
{
	// Initialize the pull-down item information.
	UIW_POP_UP_ITEM::Information(INITIALIZE_CLASS, NULL);
}

UIW_POP_UP_ITEM::UIW_POP_UP_ITEM(int left, int top, int width, char *_text,
	MNIF_FLAGS _mniFlags, BTF_FLAGS _btFlags, WOF_FLAGS _woFlags,
	USER_FUNCTION _userFunction, EVENT_TYPE _value) :
	UIW_BUTTON(left, top, width, _text, _btFlags, _woFlags, _userFunction, _value),
	menu(0, 0, WNF_NO_FLAGS, WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY),
	mniFlags(_mniFlags)
{
	// Initialize the pull-down item information.
	UIW_POP_UP_ITEM::Information(INITIALIZE_CLASS, NULL);
}

UIW_POP_UP_ITEM::~UIW_POP_UP_ITEM(void)
{
	// No item information needs to be destroyed.
}

void *UIW_POP_UP_ITEM::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_POP_UP_ITEM;
	switch (request)
	{
	case INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = windowID[0] = ID_POP_UP_ITEM;
		windowID[1] = ID_MENU_ITEM;
		windowID[2] = ID_BUTTON;
		font = FNT_SYSTEM_FONT;
		menu.parent = this;

#if defined(ZIL_OS2)
		flStyle = MIS_TEXT;
#endif
		// Continue to CHANGED_FLAGS.

	case CHANGED_FLAGS:
		// Check the button and base class flag settings.
		woFlags &= ~WOF_BORDER;
		btFlags |= BTF_NO_3D;

		// Check the flags with the number identifications.
		if (FlagSet(mniFlags, MNIF_RESTORE))
			numberID = NUMID_OPT_RESTORE;
		else if (FlagSet(mniFlags, MNIF_MOVE))
			numberID = NUMID_OPT_MOVE;
		else if (FlagSet(mniFlags, MNIF_SIZE))
			numberID = NUMID_OPT_SIZE;
		else if (FlagSet(mniFlags, MNIF_MINIMIZE))
			numberID = NUMID_OPT_MINIMIZE;
		else if (FlagSet(mniFlags, MNIF_MAXIMIZE))
			numberID = NUMID_OPT_MAXIMIZE;
		else if (FlagSet(mniFlags, MNIF_CLOSE))
			numberID = NUMID_OPT_CLOSE;
		else if (FlagSet(mniFlags, MNIF_SWITCH))
			numberID = NUMID_OPT_SWITCH;

/* START BLOCK COMMENT														// BUG.268.2
**			// Check for system commands
**			if (FlagSet(mniFlags, 0x00FF))
**			{
**				btFlags |= BTF_SEND_MESSAGE;
**				if (FlagSet(mniFlags, MNIF_RESTORE))
**					value = L_RESTORE;
**				else if (FlagSet(mniFlags, MNIF_MOVE))
**					value = L_MOVE;
**	  			else if (FlagSet(mniFlags, MNIF_SIZE))
**					value = L_SIZE;
**				else if (FlagSet(mniFlags, MNIF_MINIMIZE))
**					value = L_MINIMIZE;
**				else if (FlagSet(mniFlags, MNIF_MAXIMIZE))
**					value = L_MAXIMIZE;
**				else if (FlagSet(mniFlags, MNIF_CLOSE))
**					value = S_CLOSE;
**			}
END BLOCK COMMENT */
		// Check for system commands
//		if (FlagSet(mniFlags, 0x00FF))										// BUG.1733
		if (FlagSet(mniFlags, 0x00FF) && mniFlags != MNIF_SEPARATOR)
			btFlags |= BTF_SEND_MESSAGE;

		if ((FlagSet(btFlags, BTF_SEND_MESSAGE) || FlagSet(mniFlags, MNIF_SEND_MESSAGE)) &&
			!userFunction)
		{
			btFlags |= BTF_SEND_MESSAGE;
			mniFlags |= MNIF_SEND_MESSAGE;
			userFunction = UIW_BUTTON::Message;
		}
		if (!text || text[0] == '\0')
			mniFlags = MNIF_SEPARATOR;
		if (FlagSet(mniFlags, MNIF_NON_SELECTABLE))
			woFlags |= WOF_NON_SELECTABLE;

#if defined(ZIL_MSWINDOWS)
		if (request == CHANGED_FLAGS && parent && parent->screenID)
		{
			int position = 0;
			for (UI_WINDOW_OBJECT *object = Previous(); object; object = object->Previous())
				position++;
			WORD wFlags = MF_BYPOSITION;
			if (FlagSet(woFlags, WOF_NON_SELECTABLE))
				wFlags |= MF_GRAYED;
			else
				wFlags |= MF_ENABLED;
			EnableMenuItem(parent->screenID, position, wFlags);
			if (FlagSet(mniFlags, MNIF_CHECK_MARK))
			{
				WORD wFlags = MF_BYPOSITION;
				if (FlagSet(woStatus, WOS_SELECTED))
					wFlags |= MF_CHECKED;
				else
					wFlags |= MF_UNCHECKED;
				CheckMenuItem(parent->screenID, position, wFlags);
			}
		}
#elif defined(ZIL_OS2)
		if (request == CHANGED_FLAGS && parent && parent->screenID)			// BUG.2269
		{
			ULONG attributes = 0;
			if (FlagSet(woFlags, WOF_NON_SELECTABLE))
				attributes |= MIA_DISABLED;
			if (FlagSet(mniFlags, MNIF_CHECK_MARK) &&
				FlagSet(woStatus, WOS_SELECTED))
				attributes |= MIA_CHECKED;

			WinSendMsg(parent->screenID, MM_SETITEMATTR, MPFROM2SHORT(numberID, FALSE),
				MPFROM2SHORT(MIA_DISABLED | MIA_CHECKED, attributes));
			parent->Event(UI_EVENT(S_REDISPLAY));
		}
#elif defined(ZIL_MOTIF)
		if (FlagSet(mniFlags, MNIF_CHECK_MARK))
			Event(UI_EVENT(S_REDISPLAY));
#elif defined(ZIL_MSDOS)
		if (FlagSet(woFlags, WOF_NON_SELECTABLE) && parent && // BUG.2117
			parent->screenID && ((UIW_POP_UP_MENU *)parent)->Current() == this)
		{
			// This sets the right menu item.
			((UIW_POP_UP_MENU *)parent)->Event(UI_EVENT(L_DOWN));
		}
#endif
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

	case GET_FLAGS:
	case SET_FLAGS:
	case CLEAR_FLAGS:
		// Get, set or clear the button flag settings.
		if (objectID && objectID != ID_POP_UP_ITEM)
			data = UIW_BUTTON::Information(request, data, objectID);
		else if (request == GET_FLAGS && !data)
			data = &mniFlags;
		else if (request == GET_FLAGS)
			*(MNIF_FLAGS *)data = mniFlags;
		else if (request == SET_FLAGS)
			mniFlags |= *(MNIF_FLAGS *)data;
		else
			mniFlags &= ~(*(MNIF_FLAGS *)data);
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
UIW_POP_UP_ITEM::UIW_POP_UP_ITEM(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	UIW_BUTTON(0, 0, 1, NULL, BTF_NO_3D, WOF_NO_FLAGS),
	menu(0, 0, WNF_NO_FLAGS, WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY),
	mniFlags(MNIF_NO_FLAGS)
{
	// Initialize the pop-up item information.
	UIW_POP_UP_ITEM::Load(name, directory, file);
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
	UIW_BUTTON::Information(INITIALIZE_CLASS, NULL);
	UIW_POP_UP_ITEM::Information(INITIALIZE_CLASS, NULL);
	UIW_BUTTON::DataSet(text);
}

void UIW_POP_UP_ITEM::Load(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Load the pop-up item information.
	UIW_BUTTON::Load(name, directory, file);
	file->Load(&mniFlags);
	menu.Load(NULL, directory, file);
}

void UIW_POP_UP_ITEM::Store(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Store the pop-up item information.
	UIW_BUTTON::Store(name, directory, file);
	file->Store(mniFlags);
	menu.Store(NULL, directory, file);
}
#endif


