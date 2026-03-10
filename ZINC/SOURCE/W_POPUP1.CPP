//	Zinc Interface Library - W_POPUP1.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_POP_UP_ITEM ----------------------------------------------------

EVENT_TYPE UIW_POP_UP_ITEM::DrawItem(const UI_EVENT &, EVENT_TYPE )
{
	// Windows automatically displays text.
	return (FALSE);
}

EVENT_TYPE UIW_POP_UP_ITEM::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_INITIALIZE:
		// Check for system commands										// BUG.268.2
		if (FlagSet(mniFlags, 0x00FF))
		{
			if (FlagSet(mniFlags, MNIF_RESTORE))
				value = L_RESTORE;
			else if (FlagSet(mniFlags, MNIF_MOVE))
				value = L_MOVE;
  			else if (FlagSet(mniFlags, MNIF_SIZE))
				value = L_SIZE;
			else if (FlagSet(mniFlags, MNIF_MINIMIZE))
				value = L_MINIMIZE;
			else if (FlagSet(mniFlags, MNIF_MAXIMIZE))
				value = L_MAXIMIZE;
			else if (FlagSet(mniFlags, MNIF_CLOSE))
				value = S_CLOSE;

			for (UI_WINDOW_OBJECT *object = this; object; object = object->parent)
				if (FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT) && object->parent)
				{
					if (value < 0)
						value += S_MDICHILD_EVENT;
					else
						value += L_MDICHILD_EVENT;
					break;
				}
		}
		UI_WINDOW_OBJECT::Event(event);
		if (parent && parent->parent && parent->parent->Inherited(ID_SYSTEM_BUTTON))
			break;															// BUG.1680
		// Continue to S_CREATE.
	case S_CREATE:
		if (ccode == S_CREATE && (!parent->parent || !parent->parent->Inherited(ID_SYSTEM_BUTTON)))
			break;
		menu.woFlags |= WOF_SUPPORT_OBJECT;
		if (menu.First())
		{
			menu.Event(event);
			WORD wFlags = MF_STRING | MF_POPUP;
			if (FlagSet(woFlags, WOF_NON_SELECTABLE))
				wFlags |= MF_GRAYED;
			else if (FlagSet(mniFlags, MNIF_CHECK_MARK) && FlagSet(woStatus, WOS_SELECTED))
				wFlags |= MF_CHECKED;
#if defined(WIN32)
			if (parent->screenID && FlagSet(woStatus, WOS_WINDOWS_ACTION))
				ModifyMenu(parent->screenID, ListIndex(),
					wFlags | MF_BYPOSITION, (UINT)menu.screenID, text);
			else
				AppendMenu(parent->screenID, wFlags, (UINT)menu.screenID, text);
#else
			if (parent->screenID && FlagSet(woStatus, WOS_WINDOWS_ACTION))
				ModifyMenu(parent->screenID, ListIndex(),
					wFlags | MF_BYPOSITION, menu.screenID, text);
			else
				AppendMenu(parent->screenID, wFlags, menu.screenID, text);
#endif
		}
		else
		{
			WORD tNumberID = numberID;
			if (FlagSet(mniFlags, MNIF_RESTORE))
				tNumberID = SC_RESTORE;
			else if (FlagSet(mniFlags, MNIF_MOVE))
				tNumberID = SC_MOVE;
			else if (FlagSet(mniFlags, MNIF_SIZE))
				tNumberID = SC_SIZE;
			else if (FlagSet(mniFlags, MNIF_MINIMIZE))
				tNumberID = SC_MINIMIZE;
			else if (FlagSet(mniFlags, MNIF_MAXIMIZE))
				tNumberID = SC_MAXIMIZE;
			else if (FlagSet(mniFlags, MNIF_CLOSE))
				tNumberID = SC_CLOSE;
			else if (FlagSet(mniFlags, MNIF_SWITCH))
				tNumberID = SC_TASKLIST;
			WORD wFlags = MF_STRING;
			if (FlagSet(mniFlags, MNIF_SEPARATOR))
				wFlags = MF_SEPARATOR;
			else if (FlagSet(woFlags, WOF_NON_SELECTABLE))
				wFlags |= MF_GRAYED;
			else if (FlagSet(mniFlags, MNIF_CHECK_MARK) && FlagSet(woStatus, WOS_SELECTED))
				wFlags |= MF_CHECKED;
#if defined(WIN32)
			if (parent->screenID && FlagSet(woStatus, WOS_WINDOWS_ACTION))
				ModifyMenu(parent->screenID, ListIndex(),
					wFlags | MF_BYPOSITION, tNumberID, text);
			else
				AppendMenu(parent->screenID, wFlags, tNumberID, text);
#else
			if (parent->screenID && FlagSet(woStatus, WOS_WINDOWS_ACTION))
				ModifyMenu(parent->screenID, ListIndex(),
					wFlags | MF_BYPOSITION, menu.screenID, text);
			else
				AppendMenu(parent->screenID, wFlags, tNumberID, text);
#endif
		}
		screenID = parent->screenID;
		break;

	case S_DEINITIALIZE:
		if (parent->screenID)
		{
			WORD tNumberID = numberID;
			if (FlagSet(mniFlags, MNIF_RESTORE))
				tNumberID = SC_RESTORE;
			else if (FlagSet(mniFlags, MNIF_MOVE))
				tNumberID = SC_MOVE;
			else if (FlagSet(mniFlags, MNIF_SIZE))
				tNumberID = SC_SIZE;
			else if (FlagSet(mniFlags, MNIF_MINIMIZE))
				tNumberID = SC_MINIMIZE;
			else if (FlagSet(mniFlags, MNIF_MAXIMIZE))
				tNumberID = SC_MAXIMIZE;
			else if (FlagSet(mniFlags, MNIF_CLOSE))
				tNumberID = SC_CLOSE;
			else if (FlagSet(mniFlags, MNIF_SWITCH))
				tNumberID = SC_TASKLIST;
			DeleteMenu(parent->screenID, tNumberID, MF_BYCOMMAND);
		}
		break;

	case S_ADD_OBJECT:
	case S_SUBTRACT_OBJECT:
		menu.Event(event);
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
		ccode = UIW_BUTTON::Event(event);
		break;

	case L_SELECT:
		ccode = UIW_BUTTON::Event(event);
		if (FlagSet(mniFlags, MNIF_CHECK_MARK))
		{
//			int position = 0;
//			for (UI_WINDOW_OBJECT *object = Previous(); object; object = object->Previous())
//				position++;
			WORD wFlags = MF_BYPOSITION;
			if (FlagSet(woStatus, WOS_SELECTED))
				wFlags |= MF_CHECKED;
			else
				wFlags |= MF_UNCHECKED;
//			CheckMenuItem(parent->screenID, position, wFlags);
			CheckMenuItem(parent->screenID, ListIndex(), wFlags);
		}
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
		break;
	}

	// Return the control code.
	return (ccode);
}


