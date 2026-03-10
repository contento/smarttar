//	Zinc Interface Library - D_GROUP.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_GROUP ----------------------------------------------------------

EVENT_TYPE UIW_GROUP::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_GROUP);
	switch (ccode)
	{
	case S_SIZE:
	case S_CREATE:
		clipList.Destroy();
		UIW_WINDOW::Event(event);
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		{
		ccode = UIW_WINDOW::Event(event);
		int width = (text && text[0]) ?
			display->TextWidth(text, screenID, font) + 2 * display->cellWidth : 0;
		if (display->isText)
			display->Text(screenID, true.left + 1, true.top, text,
				lastPalette, -1, FALSE, FALSE, &clip);
		else
		{
			int polygon[12];
			polygon[10] = true.left + 2 * display->cellWidth;
			polygon[0] = polygon[10] + width;
			polygon[1] = polygon[3] = polygon[9] = polygon[11] = true.top + display->cellHeight / 2;
			polygon[2] = polygon[4] = true.right;
			polygon[5] = polygon[7] = true.bottom;
			polygon[6] = polygon[8] = true.left;
			UI_PALETTE *outline = LogicalPalette(ccode, ID_OUTLINE);
			lastPalette = LogicalPalette(ccode, ID_PROMPT);
			if (text && text[0])
				display->Text(screenID, polygon[10] + display->cellWidth,
					true.top + display->cellHeight / 4, text, lastPalette, -1,
					FALSE, FALSE, &clip);
			display->Polygon(screenID, 6, polygon, outline, FALSE, FALSE, &clip);
		}
		}
		break;

	case L_PREVIOUS:
	case L_NEXT:
		ccode = S_UNKNOWN;
		break;

	case L_LEFT:
	case L_RIGHT:
		if (current && Current()->Event(event) != S_UNKNOWN)
			break;
		// Continue to L_UP and L_DOWN.
	case L_UP:
	case L_DOWN:
		ccode = UIW_WINDOW::Event(UI_EVENT((ccode == L_LEFT || ccode == L_UP) ? L_PREVIOUS : L_NEXT));
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}


