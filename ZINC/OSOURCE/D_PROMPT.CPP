//	Zinc Interface Library - D_PROMPT.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_PROMPT ---------------------------------------------------------

EVENT_TYPE UIW_PROMPT::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	// Draw the prompt.
	UI_REGION region = true;
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);
	lastPalette = LogicalPalette(ccode);
	UI_PALETTE palette = *lastPalette;
	// Use the parent's background.
	UI_PALETTE *tPalette = parent->LogicalPalette(ccode);
	palette.colorAttribute &= 0x0F;
	palette.colorAttribute |= (0xF0 & tPalette->colorAttribute);
	palette.monoAttribute &= 0x0F;
	palette.monoAttribute |= (0xF0 & tPalette->monoAttribute);
	palette.colorBackground = tPalette->colorBackground;
	palette.bwBackground = tPalette->bwBackground;
	palette.grayScaleBackground = tPalette->grayScaleBackground;
	if (!display->isText)
		display->Rectangle(screenID, region, &palette, 0, TRUE, FALSE, &clip);
	DrawText(screenID, region, text, &palette, FALSE, ccode);
	return (ccode);
}

EVENT_TYPE UIW_PROMPT::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_PROMPT);
	switch (ccode)
	{
	case S_INITIALIZE:
		woStatus |= WOS_OWNERDRAW;
		UI_WINDOW_OBJECT::Event(event);
		if (hotKey && Next())
			Next()->HotKey(hotKey);
		break;

	case S_SIZE:
	case S_CREATE:
		if (!FlagSet(woFlags, WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT))		// BUG.1399
		{
			relative.right = relative.left + display->TextWidth(text, screenID, font);
			if (FlagSet(woFlags, WOF_BORDER))
				relative.right += display->cellWidth;
		}
		if (!FlagSet(woFlags, WOF_BORDER))
			relative.bottom = relative.top + display->cellHeight - display->preSpace - display->postSpace - 1;
		UI_WINDOW_OBJECT::Event(event);
		break;

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

