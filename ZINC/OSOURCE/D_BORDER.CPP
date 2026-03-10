//	Zinc Interface Library - D_BORDER.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_BORDER ---------------------------------------------------------

EVENT_TYPE UIW_BORDER::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
	UI_WINDOW_OBJECT *parent = this->parent;
	UI_REGION true = this->true;
	UI_REGION clip = this->clip;
	SCREENID screenID = this->screenID;
	UI_DISPLAY *display = this->display;
#endif

	// Check for MDI child window.
	if (parent->parent &&
		FlagSet(parent->woAdvancedFlags, WOAF_MDI_OBJECT) &&
		FlagSet(parent->woStatus, WOS_MAXIMIZED))
		return (ccode);
	else if (event.type == S_DISPLAY_INACTIVE &&
		event.region.Encompassed(parent->true))
		return (ccode);

	// Check for text mode display.
	if (display->isText)
	{
		UI_REGION region = parent->true;
		UI_REGION clipRegion;
		if (ccode == S_CURRENT)
			clipRegion = clip;
		else if (region.Overlap(event.region))
		{
			clipRegion.left = Max(clip.left, event.region.left);
			clipRegion.top = Max(clip.top, event.region.top);
			clipRegion.right = Min(clip.right, event.region.right);
			clipRegion.bottom = Min(clip.bottom, event.region.bottom);
		}
		else
			return (ccode);
		display->Rectangle(screenID, region, NULL, 0, TRUE, FALSE, &clipRegion);
		return (ccode);
	}

	// Draw the border.
	int size = display->cellHeight;
	UI_REGION region = parent->true;
	UI_PALETTE *outline = LogicalPalette(ccode, ID_OUTLINE);
	display->Rectangle(screenID, region, outline, 1, FALSE, FALSE, &clip);
	display->Rectangle(screenID, true, outline, 1, FALSE, FALSE, &clip);
	--region;
	if (ccode == S_DISPLAY_ACTIVE && FlagSet(parent->woAdvancedFlags, WOAF_DIALOG_OBJECT))
		lastPalette = UI_PALETTE_MAP::MapPalette(paletteMapTable, PM_SPECIAL, ID_BORDER);

	// Fill the border region.
	display->Rectangle(screenID, region.left, region.top,
		region.right, true.top - 1, lastPalette, 0, TRUE, FALSE, &clip);
	display->Rectangle(screenID, region.left, true.bottom + 1,
		region.right, region.bottom, lastPalette, 0, TRUE, FALSE, &clip);
	display->Rectangle(screenID, region.left, true.top,
		true.left - 1, true.bottom, lastPalette, 0, TRUE, FALSE, &clip);
	display->Rectangle(screenID, true.right + 1, true.top,
		region.right, true.bottom, lastPalette, 0, TRUE, FALSE, &clip);

	// Draw the size regions.
	if (!FlagSet(parent->woAdvancedFlags, WOAF_NO_SIZE))
	{
		++region;
		display->Rectangle(screenID, true.left + size, region.top,
			true.right - size, true.top, outline, 1, FALSE, FALSE, &clip);
		display->Rectangle(screenID, true.left + size, true.bottom,
			true.right - size, region.bottom, outline, 1, FALSE, FALSE, &clip);
		display->Rectangle(screenID, region.left, true.top + size,
			true.left, true.bottom - size, outline, 1, FALSE, FALSE, &clip);
		display->Rectangle(screenID, true.right, true.top + size,
			region.right, true.bottom - size, outline, 1, FALSE, FALSE, &clip);
	}

	// Return the control code.
	return (ccode);
}

EVENT_TYPE UIW_BORDER::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_BORDER);
	switch (ccode)
	{
	case S_SIZE:
	case S_CREATE:
		UI_WINDOW_OBJECT::Event(event);
		if (parent->parent &&
			FlagSet(parent->woAdvancedFlags, WOAF_MDI_OBJECT) &&
			FlagSet(parent->woStatus, WOS_MAXIMIZED))
		{
			if (!display->isText)
				++true;
		}
		else if (display->isText)
		{
			true = parent->true;
			true.right--;
			true.bottom--;
		}
		else 
			true -= width;
		break;

	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		{
		UI_EVENT tEvent = event;
		UI_REGION region = event.region;
		// Ugly code to figure out if the parent window is really an
		// active MDI child.
		if (ccode == S_DISPLAY_INACTIVE &&
			FlagSet(parent->woAdvancedFlags, WOAF_MDI_OBJECT) &&
			parent->parent && FlagSet(parent->parent->woStatus, WOS_CURRENT) &&
			(!parent->Next() || !FlagSet(parent->Next()->woAdvancedFlags, WOAF_MDI_OBJECT)))
			tEvent.type = S_DISPLAY_ACTIVE;
		UI_WINDOW_OBJECT::Event(tEvent);
		if (parent->true.Overlap(region))
			woStatus |= WOS_REDISPLAY;
		if (display->isText && region.Encompassed(true))
			woStatus &= ~WOS_REDISPLAY;
		if (FlagSet(woStatus, WOS_REDISPLAY) || display->isText)
			DrawItem(tEvent, ccode);
		woStatus &= ~WOS_REDISPLAY;
		}
		break;

	case L_VIEW:
	case L_BEGIN_SELECT:
		{
		DEVICE_IMAGE image = DM_VIEW;
		if (!FlagSet(parent->woAdvancedFlags, WOAF_NO_SIZE) &&		// BUG.internal
			!FlagSet(parent->woStatus, WOS_MAXIMIZED | WOS_MINIMIZED))
		{
			UI_REGION region = true;
			if (!display->isText)
				region -= display->cellHeight;

			RAW_CODE sizeFlags = 0;
			if (event.position.column <= region.left)
				sizeFlags |= M_LEFT_CHANGE;
			else if (event.position.column >= region.right)
				sizeFlags |= M_RIGHT_CHANGE;
			if (event.position.line <= region.top)
				sizeFlags |= M_TOP_CHANGE;
			else if (event.position.line >= region.bottom)
				sizeFlags |= M_BOTTOM_CHANGE;
			if (ccode == L_BEGIN_SELECT)
			{
				parent->Event(UI_EVENT(L_SIZE, sizeFlags, event.position));
				break;
			}
			if (FlagsSet(sizeFlags, M_LEFT_CHANGE | M_TOP_CHANGE) ||
			    FlagsSet(sizeFlags, M_RIGHT_CHANGE | M_BOTTOM_CHANGE))
				image = DM_DIAGONAL_ULLR;
			else if (FlagsSet(sizeFlags, M_LEFT_CHANGE | M_BOTTOM_CHANGE) ||
				 FlagsSet(sizeFlags, M_RIGHT_CHANGE | M_TOP_CHANGE))
				image = DM_DIAGONAL_LLUR;
			else if (FlagSet(sizeFlags, M_LEFT_CHANGE | M_RIGHT_CHANGE))
				image = DM_HORIZONTAL;
			else if (FlagSet(sizeFlags, M_TOP_CHANGE | M_BOTTOM_CHANGE))
				image = DM_VERTICAL;
			}
		eventManager->DeviceState(E_MOUSE, image);
		}
		break;

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}


