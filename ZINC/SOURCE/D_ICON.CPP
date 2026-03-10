//	Zinc Interface Library - D_ICON.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_ICON -----------------------------------------------------------

EVENT_TYPE UIW_ICON::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
	SCREENID screenID = this->screenID;
	UI_DISPLAY *display = this->display;
	UI_REGION clip = this->clip;
	UI_PALETTE *lastPalette = this->lastPalette;
#endif
	UI_REGION region = true;

	// Check for text mode operation.
	if (display->isText)
	{
//		DrawText(screenID, region, title, lastPalette, FALSE, ccode);
		DrawText(screenID, region, title, lastPalette, TRUE, ccode);	// BUG.1974
		return (ccode);
	}

	// Draw the icon.
	int listItem = parent->Inherited(ID_LIST);
	if (!listItem)
	{
		region = iconRegion;
		if (FlagSet(woFlags, WOF_BORDER))
			DrawBorder(screenID, region, FALSE, ccode);
		if (iconArray || icon.colorBitmap)
		{
			if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT) && parent && parent->parent)
				display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);
			else if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
				display->Rectangle(screenID, region, display->backgroundPalette, 0, TRUE, FALSE, &clip);
			display->Bitmap(screenID, region.left, region.top, iconWidth, iconHeight, iconArray, NULL,
				&clip, &icon.colorBitmap, &icon.monoBitmap);
		}
		else
			display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);
		if (title)
		{
			region = titleRegion;
			DrawText(screenID, region, title, lastPalette, TRUE, ccode);
		}
		return (ccode);
	}

	// Draw the list item.
	int iconOffset = listItem ? 4 : 0;
	if (ccode != S_CURRENT)
		lastPalette = LogicalPalette(ccode, ID_LIST);

	display->VirtualGet(screenID, region);
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);
	display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);

	// Compute the draw region.
	region.left += iconOffset;
	if (parent->Inherited(ID_VT_LIST))
		region.left += relative.left;
	if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT | WOF_JUSTIFY_CENTER))
		region.right -= iconOffset;
	int fieldWidth = region.Width();
	int fieldHeight = region.Height();
	int left = region.left;
	if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
		left = region.right - iconWidth + 1;
	else if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
		left = region.left + (fieldWidth - iconWidth) / 2;
	int top = (FlagSet(woFlags, WOF_JUSTIFY_CENTER) && title) ?
		region.top + 2 : region.top + (fieldHeight - iconHeight) / 2;

	// Draw the icon.
	if (iconArray || icon.colorBitmap)
		display->Bitmap(screenID, left, top, iconWidth, iconHeight,
			iconArray, NULL, &clip, &icon.colorBitmap, &icon.monoBitmap);

	// Draw the title.
	if (title)
	{
		WOF_FLAGS flags = woFlags;
		if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
			region.top = region.bottom - display->cellHeight + 4 + display->preSpace + display->postSpace;
		else if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
		{
			region.right -= iconWidth - iconOffset;
			woFlags &= ~(WOF_JUSTIFY_RIGHT);
		}
		else
			region.left += iconWidth + iconOffset;
		region.bottom--;
		DrawText(screenID, region, title, lastPalette, FALSE, ccode);
		woFlags = flags;
	}

	// Restore the display.
	display->VirtualPut(screenID);

	// Return the control code.
#if defined(ZIL_OPTIMIZE)
	this->lastPalette = lastPalette;
#endif
	return (ccode);
}

EVENT_TYPE UIW_ICON::Event(const UI_EVENT &event)
{
	static initializedTime = FALSE;
	static UI_TIME lastTime;
	if (!initializedTime)
	{
		lastTime.Import();
		initializedTime = TRUE;
	}

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_ICON);
	switch (ccode)
	{
	case S_INITIALIZE:
		woStatus &= ~WOS_OWNERDRAW;
		iconHeight = iconWidth = 32;
		if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
			icFlags |= ICF_MINIMIZE_OBJECT;
		if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
		{
			woFlags |= WOF_JUSTIFY_CENTER;									// BUG.1264
			RegionConvert(relative, TRUE);
			relative.left = parent->relative.left;
			relative.top = parent->relative.top;
		}
		else
			UI_WINDOW_OBJECT::Event(event);
		relative.right = relative.left + iconWidth + 2;
		relative.bottom = relative.top + iconHeight + 2;
		if (!display->isText && title && FlagSet(woFlags, WOF_JUSTIFY_CENTER))
		{
			relative.bottom += display->cellHeight / 4;
			relative.bottom += display->TextHeight(title, screenID, font);
			int width = display->TextWidth(title, screenID, font);
			if (width > iconWidth)
				relative.right = relative.left + width + display->cellWidth;
		}
		else if (!display->isText && title)
			relative.right = relative.left + display->TextWidth(title) - 1;
		else if (title)
		{
			relative.right = relative.left + display->TextWidth(title) - 1;
			relative.bottom = relative.top;
		}
//		if (display->isText)												// BUG.1060
		else if (display->isText)
		{
			true.right = true.left;
			true.bottom = true.top;
			relative = true;
		}
		if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
		{
			true = relative;
			parent->Information(SET_ICON, this);
		}
		break;

	case S_SIZE:
	case S_CREATE:
		if (!FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
			ccode = UI_WINDOW_OBJECT::Event(event);
		else
			screenID = parent->screenID;
		if (!display->isText && title)
		{
			iconRegion.left = (true.right - true.left > iconWidth) ?
				(true.left + true.right - iconWidth) / 2 : true.left;
			iconRegion.top = true.top;
			iconRegion.right = iconRegion.left + iconWidth - 1;
			iconRegion.bottom = iconRegion.top + iconHeight - 1;
			titleRegion = true;
			titleRegion.top = titleRegion.bottom - display->TextHeight(title, screenID, font);
			DataSet(title);
		}
		else
			iconRegion = true;
		if (iconArray && !icon.colorBitmap && !FlagSet(icFlags, ICF_STATIC_ICONARRAY))	// BUG.2109
		{
			display->IconArrayToHandle(screenID, iconWidth, iconHeight,
				iconArray, NULL, &icon);
			if (icon.colorBitmap)
			{
				delete iconArray;
				iconArray = NULL;
			}
		}
		break;

	case S_REGION_DEFINE:
		if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT) && display->isText)
		{
			UI_REGION region;
			region.left = Max(true.left, clip.left);
			region.top = Max(true.top, clip.top);
			region.right = Min(true.right, clip.right);
			region.bottom = Min(true.bottom, clip.bottom);
			display->RegionDefine(screenID, region);
		}
		else if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
		{
			UI_REGION region;
			region.left = Max(iconRegion.left, clip.left);
			region.top = Max(iconRegion.top, clip.top);
			region.right = Min(iconRegion.right, clip.right);
			region.bottom = Min(iconRegion.bottom, clip.bottom);
			display->RegionDefine(screenID, region);
			region.left = Max(titleRegion.left, clip.left);
			region.top = Max(titleRegion.top, clip.top);
			region.right = Min(titleRegion.right, clip.right);
			region.bottom = Min(titleRegion.bottom, clip.bottom);
			display->RegionDefine(screenID, region);
		}
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
		{
			if (FlagSet(parent->woStatus, WOS_MINIMIZED))
				woFlags &= ~WOF_NON_SELECTABLE;
			else
				woFlags |= WOF_NON_SELECTABLE;
		}
		UI_WINDOW_OBJECT::Event(event);
		if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT) && !FlagSet(parent->woStatus, WOS_MINIMIZED))
			woStatus &= ~WOS_REDISPLAY;
		else if (FlagSet(woStatus, WOS_REDISPLAY))
		{
			DrawItem(event, ccode);
			woStatus &= ~WOS_REDISPLAY;
		}
		break;

	case L_BEGIN_SELECT:
		if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT) &&
			(iconRegion.Overlap(event.position) || titleRegion.Overlap(event.position)))
		{
			UI_TIME currentTime;
			long elapsedTime = currentTime - lastTime;						// BUG.1228
			if (Abs(elapsedTime) < doubleClickRate)
				parent->Event(UI_EVENT(L_RESTORE, 0x0001));
			else
				parent->Event(UI_EVENT(L_MOVE, 0x0001, event.position));
			lastTime = currentTime;
		}
		else if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
			return (S_UNKNOWN);
		else
			ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case L_END_SELECT:
#if defined(ZIL_EDIT)
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UI_WINDOW_OBJECT::Event(event));
#endif
		if (iconRegion.Overlap(event.position) || titleRegion.Overlap(event.position))
		{
			UI_TIME currentTime;
			long elapsedTime = currentTime - lastTime;						// BUG.1228
			if (FlagSet(icFlags, ICF_DOUBLE_CLICK &&
				Abs(elapsedTime) < doubleClickRate))
				ccode = UserFunction(UI_EVENT(L_END_SELECT), L_DOUBLE_CLICK);
			else
			{
				ccode = UserFunction(UI_EVENT(L_END_SELECT), L_SELECT);
				lastTime = currentTime;
			}
		}
		break;

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

