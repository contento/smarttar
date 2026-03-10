//	Zinc Interface Library - D_POPUP.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <ctype.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_POP_UP_MENU ----------------------------------------------------

EVENT_TYPE UIW_POP_UP_MENU::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_POP_UP_MENU);
	switch (ccode)
	{
	case S_SIZE:
	case S_CREATE:
		{
		// Compute the pop-up item positions.
		int width = 0, height = 0, top = 0;
		for (object = First(); object; object = object->Next())
		{
			int tWidth = object->relative.right - object->relative.left + 1;
			if (tWidth > width)
				width = tWidth;
			height += (object->relative.bottom - object->relative.top + 1);
		}
		relative.right = relative.left + width - 1;
		relative.bottom = relative.top + height - 1;
		if (FlagSet(woFlags, WOF_BORDER) &&
			(!display->isText || FlagSet(woAdvancedFlags, WOAF_TEMPORARY)))
		{
			relative.right += 2;
			relative.bottom += 2;
		}

		// Check for menus off the screen.
		if (!parent || FlagSet(woAdvancedFlags, WOAF_TEMPORARY))
		{
			if (relative.right >= display->columns)
			{
				int width = relative.right - relative.left;
				relative.left = relative.right = display->columns - 1;
				relative.left -= width;
			}
			if (relative.bottom >= display->lines)
			{
				int height = relative.bottom - relative.top;
				relative.top = relative.bottom = display->lines - 1;
				relative.top -= height;
			}
		}

		for (object = First(); object; object = object->Next())
		{
			height = (object->relative.bottom - object->relative.top + 1);
			object->relative.left = 0;
			object->relative.top = top;
			object->relative.right = width - 1;
			top += height;
			object->relative.bottom = top - 1;
		}
		UIW_WINDOW::Event(event);
		}
		break;

	case L_UP:
	case L_DOWN:
		ccode = UIW_WINDOW::Event(UI_EVENT((ccode == L_UP) ? L_PREVIOUS : L_NEXT));
		break;

	case L_PREVIOUS:
	case L_NEXT:
		ccode = S_UNKNOWN;
		break;

	case L_END_SELECT:
		if (!true.Overlap(event.position) &&
			parent && parent->true.Overlap(event.position))
			break;
		ccode = UIW_WINDOW::Event(event);
		if (parent->SearchID() == ID_PULL_DOWN_ITEM)			// BUG.7157
			eventManager->Put(UI_EVENT(L_CANCEL));
		break;

#if defined(ZIL_EDIT)
	case L_VIEW:
	case L_BEGIN_SELECT:
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UI_WINDOW_OBJECT::Event(event));
		// Continue to default.
#endif
	default:
		// Try to find the hotkey menu item.
		if (ccode == E_KEY && !FlagSet(event.key.shiftState, S_ALT))
		{
			for (object = First(); object; object = object->Next())
				if (object->HotKey() == tolower(event.key.value) && !FlagSet(object->woFlags, WOF_NON_SELECTABLE))
				{
					UIW_WINDOW::Add(object);
					return (object->Event(UI_EVENT(L_SELECT)));
				}
		}
		else
			ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

