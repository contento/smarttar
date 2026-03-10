//	Zinc Interface Library - D_TITLE.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_TITLE ----------------------------------------------------------

EVENT_TYPE UIW_TITLE::Event(const UI_EVENT &event)
{
	static UI_TIME lastTime;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_TITLE);
	switch (ccode)
	{
	case S_SIZE:
	case S_CREATE:
		value = FlagSet(parent->woStatus, WOS_MAXIMIZED) ? L_RESTORE : L_MAXIMIZE;
		UI_WINDOW_OBJECT::Event(event);
		if (display->isText)
			true.bottom = true.top;
		else
		{
			true.left--; true.right++;
			true.bottom = --true.top + (display->cellHeight - display->preSpace - display->postSpace);
		}
		break;

	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		if (display->isText)
			lastPalette = NULL;
		UI_WINDOW_OBJECT::Event(event);
		// Continue to S_REDISPLAY.
	case S_REDISPLAY:
		if (FlagSet(woStatus, WOS_REDISPLAY) || ccode == S_REDISPLAY)
		{
			UI_REGION region = true;
			if (FlagSet(woFlags, WOF_BORDER))
				DrawBorder(screenID, region, FALSE, ccode);
			DrawText(screenID, region, text, lastPalette, TRUE, ccode);
			woStatus &= ~WOS_REDISPLAY;
		}
		break;

	case L_BEGIN_SELECT:
		{
		UI_TIME currentTime;
		long elapsedTime = currentTime - lastTime;							// BUG.1228
		if (Abs(elapsedTime) > doubleClickRate &&
			!FlagSet(parent->woAdvancedFlags, WOAF_NO_MOVE) &&
			!FlagSet(parent->woStatus, WOS_MAXIMIZED))
		{
			parent->Event(UI_EVENT(L_MOVE, 0x0001, event.position));
		}
		else if (!FlagSet(parent->woAdvancedFlags, WOAF_NO_SIZE) &&				// BUG.1263, BUG.1975
			Abs(elapsedTime) < doubleClickRate)
		{
			UI_EVENT event(value);
			event.rawCode = 0;
			event.data = parent;
			eventManager->Put(event);
		}
		lastTime = currentTime;
		}
		break;

	default:
		ccode = UIW_BUTTON::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

