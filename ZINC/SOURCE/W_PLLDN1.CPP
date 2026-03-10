//	Zinc Interface Library - W_PLLDN1.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_PULL_DOWN_ITEM -------------------------------------------------

EVENT_TYPE UIW_PULL_DOWN_ITEM::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	return (ccode);
}

EVENT_TYPE UIW_PULL_DOWN_ITEM::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_INITIALIZE:
		UI_WINDOW_OBJECT::Event(event);
		menu.woFlags |= WOF_SUPPORT_OBJECT;
		if (menu.First())
		{
			menu.Event(event);
			WORD wFlags = MF_STRING | MF_POPUP;
			if (FlagSet(woFlags, WOF_NON_SELECTABLE))
				wFlags |= MF_GRAYED;
#if defined(WIN32)
			if (parent->screenID && FlagSet(woStatus, WOS_WINDOWS_ACTION))
				ModifyMenu(parent->screenID, ListIndex(),
//					(wFlags & ~MF_BYCOMMAND) | MF_BYPOSITION,
					wFlags | MF_BYPOSITION,
					(UINT)menu.screenID, text);
			else
				AppendMenu(parent->screenID, wFlags, (UINT)menu.screenID, text);
#else
			if (parent->screenID && FlagSet(woStatus, WOS_WINDOWS_ACTION))
				ModifyMenu(parent->screenID, ListIndex(),
//					(wFlags & ~MF_BYCOMMAND) | MF_BYPOSITION,
					wFlags | MF_BYPOSITION,
					menu.screenID, text);
			else
				AppendMenu(parent->screenID, wFlags, menu.screenID, text);
#endif
		}
		else
		{
			WORD wFlags = MF_STRING;
			if (FlagSet(woFlags, WOF_NON_SELECTABLE))
				wFlags |= MF_GRAYED;
			AppendMenu(parent->screenID, wFlags, numberID, text);
		}
		screenID = parent->screenID;
		break;

	case S_DEINITIALIZE:
		if (parent->screenID)
			DeleteMenu(parent->screenID, numberID, MF_BYCOMMAND);
		break;

	case S_REDISPLAY:														// BUG.1253
		// Parent must be redisplayed in Windows.
		parent->Event(event);
		break;

	case S_ADD_OBJECT:
	case S_SUBTRACT_OBJECT:
		menu.Event(event);
		break;

	case L_SELECT:
	case S_CURRENT:
	case S_NON_CURRENT:
		ccode = UIW_BUTTON::Event(event);
		break;

	default:
		if (event.type == E_MSWINDOWS)
		{
			WORD message = event.message.message;
			if (message == WM_MENUSELECT)
			{
				for (UIW_POP_UP_ITEM *item = (UIW_POP_UP_ITEM *)menu.First(); item; item = (UIW_POP_UP_ITEM *)item->Next())
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
				if (item && item != menu.Current())
					menu.Add(item);
				else if (menu.Current())
					menu.Current()->Event(event);
			}
		}
		else
			ccode = UIW_BUTTON::Event(event);
	}

	// Return the control code.
	return (ccode);
}


