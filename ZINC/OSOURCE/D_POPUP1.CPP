//	Zinc Interface Library - D_POPUP1.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

const int ITEM_OFFSET = 4;

// ----- UIW_POP_UP_ITEM ----------------------------------------------------

EVENT_TYPE UIW_POP_UP_ITEM::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
	SCREENID screenID = this->screenID;
	UI_DISPLAY *display = this->display;
	UI_PALETTE *lastPalette = this->lastPalette;
	UI_REGION clip = this->clip;
#endif
	UI_REGION region = true;

	// Draw the pop-up item.
	woStatus &= ~WOS_REDISPLAY;
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);
	display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);
	if (FlagSet(mniFlags, MNIF_SEPARATOR))
	{
		lastPalette = LogicalPalette(S_DISPLAY_ACTIVE, ID_OUTLINE);
		region.top = (region.top + region.bottom) / 2;
		display->Line(screenID, region.left, region.top, region.right,
			region.top, lastPalette, 1, FALSE, &clip);
		return (ccode);
	}
	if (!display->isText)
		region.left += ITEM_OFFSET;

	// Draw the check mark.
	COLOR foregroundColor = display->MapColor(lastPalette, TRUE);
	if (!FlagSet(woStatus, WOS_SELECTED) || !FlagSet(mniFlags, MNIF_CHECK_MARK))
		;
	else if (display->isText)
		display->Text(screenID, region.left, region.top, _iCheckMark, lastPalette, 1, FALSE, FALSE, &clip);
	else
	{
		static UCHAR checkMark[90] =
		{
			BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND, BACKGROUND, BACKGROUND, BACKGROUND, BLACK,
			BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND, BACKGROUND, BACKGROUND, BACKGROUND, BLACK,
			BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND, BACKGROUND, BACKGROUND, BLACK, 		BLACK,
			BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND, BACKGROUND, BACKGROUND, BLACK, 		BACKGROUND,
			BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND, BACKGROUND, BLACK, 		BLACK, 		BACKGROUND,
			BACKGROUND,	BLACK,		BACKGROUND,	BACKGROUND,	BACKGROUND, BACKGROUND, BLACK, 		BACKGROUND, BACKGROUND,
			BLACK,		BLACK,		BLACK,		BACKGROUND,	BACKGROUND, BLACK, 		BLACK, 		BACKGROUND, BACKGROUND,
			BACKGROUND,	BACKGROUND,	BLACK,		BLACK,		BACKGROUND, BLACK, 		BACKGROUND, BACKGROUND, BACKGROUND,
			BACKGROUND,	BACKGROUND,	BACKGROUND,	BLACK,		BLACK, 		BLACK, 		BACKGROUND, BACKGROUND, BACKGROUND,
			BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,	BLACK,		BACKGROUND, BACKGROUND, BACKGROUND, BACKGROUND
		};
		for (int i = 0; i < 90; i++)
			if (checkMark[i] != BACKGROUND)
				checkMark[i] = foregroundColor;
		int top = true.top + (true.bottom - true.top - 10) / 2;
		display->Bitmap(screenID, region.left, top, 9, 10, checkMark, NULL, &clip);
	}
	region.left += 2 * display->cellWidth;

	// Draw the string.
	DrawText(screenID, region, text, lastPalette, FALSE, ccode);

	// Draw the right arrow.
	if (menu.First() && display->isText)
		display->Text(screenID, region.right, region.top, _iSBRightArrow, lastPalette, 1, FALSE, FALSE, &clip);
	else if (menu.First())
	{
		static UCHAR rightArrow[45] =
		{
			BLACK,		BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,
			BLACK,		BLACK,		BACKGROUND,	BACKGROUND,	BACKGROUND,
			BLACK,		BLACK,		BLACK,		BACKGROUND,	BACKGROUND,
			BLACK,		BLACK,		BLACK,		BLACK,		BACKGROUND,
			BLACK,		BLACK,		BLACK,		BLACK,		BLACK,
			BLACK,		BLACK,		BLACK,		BLACK,		BACKGROUND,
			BLACK,		BLACK,		BLACK,		BACKGROUND,	BACKGROUND,
			BLACK,		BLACK,		BACKGROUND,	BACKGROUND,	BACKGROUND,
			BLACK,		BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND
		};
		for (int i = 0; i < 45; i++)
			if (rightArrow[i] != BACKGROUND)
				rightArrow[i] = foregroundColor;
		region.left = true.right - 2 * ITEM_OFFSET;
		region.top = true.top + (true.bottom - true.top - 9) / 2;
		display->Bitmap(screenID, region.left, region.top, 5, 9, rightArrow, NULL, &clip);
	}

#if defined(ZIL_OPTIMIZE)
	this->lastPalette = lastPalette;
#endif
	return (ccode);
}

EVENT_TYPE UIW_POP_UP_ITEM::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_POP_UP_ITEM);
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
		woStatus |= WOS_OWNERDRAW;
		ccode = UIW_BUTTON::Event(event);
		break;

	case S_RESET_DISPLAY:
		ccode = UIW_BUTTON::Event(event);
		menu.Event(event);
		break;

	case S_CREATE:
		UI_WINDOW_OBJECT::Event(event);
		if (FlagSet(mniFlags, MNIF_SEPARATOR))
		{
			woFlags |= WOF_NON_SELECTABLE;
			relative.right = relative.left;
			relative.bottom = relative.top + display->cellHeight / 2;
		}
		else if (display->isText)
		{
 			relative.right = relative.left + display->TextWidth(text, screenID, font) + 2;
			if (menu.First())
				relative.right += 4;
 			relative.bottom = relative.top;
		}
		else
		{
 			relative.right = relative.left + display->TextWidth(text, screenID, font) + display->cellHeight;
			if (menu.First())
				relative.right += display->cellHeight;
 			relative.bottom = relative.top + display->cellHeight - display->preSpace - display->postSpace;
		}
		break;

	case S_ADD_OBJECT:
	case S_SUBTRACT_OBJECT:
		menu.Event(event);
		break;

	case L_END_SELECT:
		if (!true.Overlap(event.position) || !FlagSet(btStatus, BTS_DEPRESSED))
			return (UIW_BUTTON::Event(event));
		ccode = L_SELECT;
		// Continue to L_SELECT.
	case L_SELECT:
	case L_LEFT:
	case L_RIGHT:
		if (windowManager->Index(parent) == -1)
			ccode = S_UNKNOWN;
		else if (ccode == L_SELECT && menu.First() && windowManager->Index(&menu) != -1)
			break;
		else if ((ccode == L_RIGHT || ccode == L_SELECT) && menu.First())
		{
	 		menu.RegionConvert(menu.relative, TRUE);
			menu.relative.left = display->isText ?
//				true.right - ITEM_OFFSET :									// Bug.1423
				true.right - 1 :
				true.right - 2 * ITEM_OFFSET;
			menu.relative.top = true.top - 1;
			if (menu.Current())
				menu.Current()->woStatus &= ~WOS_CURRENT;
			*windowManager + &menu;
		}
		else if (ccode == L_SELECT)
		{
			ccode = UIW_BUTTON::Event(event);
			for (UI_WINDOW_OBJECT *object = windowManager->First();
				object && FlagSet(object->woAdvancedFlags, WOAF_TEMPORARY);
				object = windowManager->First())
				windowManager->Event(UI_EVENT(S_CLOSE_TEMPORARY));
		}
		else if (ccode == L_LEFT && parent->parent &&
			parent->parent->Inherited(ID_POP_UP_ITEM))
			windowManager->Event(UI_EVENT(S_CLOSE_TEMPORARY));
		else
		{
			for (UI_WINDOW_OBJECT *object = windowManager->First();
				object && FlagSet(object->woAdvancedFlags, WOAF_TEMPORARY);
				object = windowManager->First())
				windowManager->Event(UI_EVENT(S_CLOSE_TEMPORARY));
			eventManager->Put(UI_EVENT(ccode));
			eventManager->Put(UI_EVENT(L_DOWN));
		}
		break;

	default:
		ccode = UIW_BUTTON::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}


