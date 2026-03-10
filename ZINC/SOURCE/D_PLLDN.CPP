//	Zinc Interface Library - D_PLLDN.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <ctype.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_PULL_DOWN_MENU -------------------------------------------------

EVENT_TYPE UIW_PULL_DOWN_MENU::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_PULL_DOWN_MENU);
	switch (ccode)
	{
	case S_SIZE:
	case S_CREATE:
		UI_WINDOW_OBJECT::Event(event);
		clipList.Destroy();
//		// Continue to S_REDISPLAY.												// BUG.1251
//	case S_REDISPLAY:
		{
		int left = 0, top = 0, height = display->cellHeight;
		if (display->isText)
		{
			woFlags &= ~WOF_BORDER;
			true.bottom = true.top;
			relative.right = true.Width();
			relative.bottom = true.Height();
		}
		else
		{
			woFlags |= WOF_BORDER;
			if (ccode != S_REDISPLAY)
				true.left--, true.right++, true.top--;
			height -= (display->preSpace + display->postSpace);
			true.bottom = true.top + height + 1;
			relative.right = true.right - true.left - 1;
			relative.bottom = true.bottom - true.top - 1;
		}
		for (object = First(); object; object = object->Next())
		{
			int width = object->relative.Width();
			if (left + width > relative.right)
			{
				left = 0;
				top += relative.bottom;
				true.bottom += height;
			}
			object->relative.left = left;
			object->relative.top = top;
			left += width;
			object->relative.right = left - 1;
			object->relative.bottom = top + height - 1;
		}

		// Compute the actual regions.
		UI_REGION region = true;
		if (FlagSet(woFlags, WOF_BORDER))
			--region;
		clipList.Destroy();
		clipList.Add(new UI_REGION_ELEMENT(screenID, region));
		UI_EVENT tEvent = event;
//		if (ccode == S_REDISPLAY)												// BUG.1251
//			tEvent.type = S_CREATE; 
		for (object = First(); object; object = object->Next())
			object->Event(tEvent);

//		// Redisplay the field.													// BUG.1251
//		if (ccode == S_REDISPLAY)
//			UI_WINDOW_OBJECT::Event(event);
		}
		break;

	case L_UP:
	case L_DOWN:
		ccode = Current()->Event(event);
		break;

	case L_LEFT:
	case L_RIGHT:
		ccode = UIW_WINDOW::Event(UI_EVENT((ccode == L_LEFT) ? L_PREVIOUS : L_NEXT));
		break;

	case L_PREVIOUS:
	case L_NEXT:
		ccode = S_UNKNOWN;
		break;

#if defined(ZIL_EDIT)
	case L_BEGIN_SELECT:
	case L_CONTINUE_SELECT:
	case L_END_SELECT:
		if (!FlagSet(woStatus, WOS_EDIT_MODE))
			ccode = UIW_WINDOW::Event(event);
		break;
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

