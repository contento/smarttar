//	Zinc Interface Library - D_PLLDN1.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_PULL_DOWN_ITEM -------------------------------------------------

EVENT_TYPE UIW_PULL_DOWN_ITEM::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	const int ITEM_OFFSET = 4;

	// Draw the string.
	UI_REGION region = true;
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);
	display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);
	if (!display->isText)
		region.left += ITEM_OFFSET;
	DrawText(screenID, region, text, lastPalette, FALSE, ccode);
	return (ccode);
}

EVENT_TYPE UIW_PULL_DOWN_ITEM::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_PULL_DOWN_ITEM);
	switch (ccode)
	{
	case S_INITIALIZE:
		woStatus |= WOS_OWNERDRAW;
		ccode = UIW_BUTTON::Event(event);
		relative.right = relative.left + display->TextWidth(text, screenID, font) + 2 * display->cellWidth - 1;
		relative.bottom = relative.top + display->cellHeight - display->preSpace - display->postSpace;
		break;

	case S_RESET_DISPLAY:
		ccode = UIW_BUTTON::Event(event);
		menu.Event(event);
		break;

	case S_ADD_OBJECT:
	case S_SUBTRACT_OBJECT:
		menu.Event(event);
		break;

	case L_BEGIN_SELECT:
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (ccode == S_UNKNOWN)
			break;
		// Continue to L_SELECT.
	case L_SELECT:
		if (userFunction)
		{
			UI_EVENT uEvent;
			uEvent = event;
			ccode = (*userFunction)(this, uEvent, L_SELECT);
			break;
		}
		// Continue to L_UP & L_DOWN.
	case L_UP:
	case L_DOWN:
		if (menu.First() && windowManager->Index(&menu) == -1)
		{
	 		menu.RegionConvert(menu.relative, TRUE);
			menu.relative.left = true.left;
			menu.relative.top = true.bottom + 1;
			if (menu.Current())
				menu.Current()->woStatus &= ~WOS_CURRENT;
			*windowManager + &menu;
		}
		break;

	default:
		ccode = UIW_BUTTON::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}


