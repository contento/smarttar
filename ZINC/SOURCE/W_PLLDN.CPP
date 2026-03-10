//	Zinc Interface Library - W_PLLDN.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_PULL_DOWN_MENU -------------------------------------------------

EVENT_TYPE UIW_PULL_DOWN_MENU::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	// Switch on the event type.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_INITIALIZE:
		{
		if (parent->parent)		// Sub-windows cannot have pull-down menus.
			return (S_UNKNOWN);
		UI_WINDOW_OBJECT::Event(event);
		screenID = CreateMenu();
		woFlags |= WOF_SUPPORT_OBJECT;
		parent->Information(SET_MENU, &screenID);
		for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
			object->Event(event);
		if (parent->screenID)
			SetMenu(parent->screenID, screenID);
		}
		break;

	case S_ADD_OBJECT:
		UIW_WINDOW::Event(event);
		break;

	case S_SUBTRACT_OBJECT:
		object = (UI_WINDOW_OBJECT *)event.data;
		if (screenID)
			DeleteMenu(screenID, UI_LIST::Index(object), MF_BYPOSITION);
		break;

	case S_REDISPLAY:
		if (parent->screenID)												// BUG.1380
			DrawMenuBar(parent->screenID);
		break;

	default:
		if (event.type == E_MSWINDOWS)
		{
			WORD message = event.message.message;
			if (message == WM_MENUSELECT)
			{
				for (UIW_PULL_DOWN_ITEM *item = (UIW_PULL_DOWN_ITEM *)First();
					item; item = (UIW_PULL_DOWN_ITEM *)item->Next())
				{
#if defined(WIN32)
					if (FlagSet(HIWORD(event.message.wParam), MF_POPUP))
					{
						if ((HMENU)event.message.lParam == item->menu.screenID)
							break;
					}
					else if (LOWORD(event.message.wParam) == item->NumberID())
						break;
#else
					if (FlagSet(LOWORD(event.message.lParam), MF_POPUP))
					{
						if (event.message.wParam == item->menu.screenID)
							break;
					}
					else if (event.message.wParam == item->NumberID())
						break;
#endif
				}
				if (item && item != current)
					UIW_WINDOW::Add(item);
				else if (current)
					Current()->Event(event);
			}
		}
		else
			ccode = S_UNKNOWN;
	}

	// Return the control code.
	return (ccode);
}


