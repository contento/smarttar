//	Zinc Interface Library - D_WIN2.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stdio.h>
#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

char *UI_WINDOW_OBJECT::pasteBuffer = NULL;
int UI_WINDOW_OBJECT::pasteLength = 0;

// ----- UI_WINDOW_OBJECT ---------------------------------------------------

EVENT_TYPE UI_WINDOW_OBJECT::DrawBorder(SCREENID screenID, UI_REGION &region,
	int fillRegion, EVENT_TYPE ccode)
{
	if (display->isText && (region.bottom == region.top || !FlagSet(woAdvancedFlags, WOAF_TEMPORARY)))
		return (ccode);

#if defined(ZIL_OPTIMIZE)
	UI_DISPLAY *display = this->display;
	UI_REGION clip = this->clip;
#endif

	// Draw the object border.
	UI_PALETTE *outline = LogicalPalette(ccode, ID_OUTLINE);
	display->Rectangle(screenID, region, outline, 1, FALSE, FALSE, &clip);
	--region;

	// Fill in the region.
	if (fillRegion)
		display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);
	return (ccode);
}

EVENT_TYPE UI_WINDOW_OBJECT::DrawItem(const UI_EVENT &, EVENT_TYPE )
{
	return (FALSE);
}

EVENT_TYPE UI_WINDOW_OBJECT::DrawShadow(SCREENID screenID, UI_REGION &region,
	int depth, int fillRegion, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
	UI_DISPLAY *display = this->display;
	UI_REGION clip = this->clip;
#endif

	// Make sure there is depth to the object
	if (!depth || (display->isText && (Abs(depth) < 2 || region.bottom - region.top < 1)))
		return (ccode);

	// Draw the shadow.
 	if (!display->isText)
	{
		UI_PALETTE *lightShadow = LogicalPalette(ccode, (depth > 0) ? ID_WHITE_SHADOW : ID_DARK_SHADOW);
		UI_PALETTE *darkShadow = LogicalPalette(ccode, (depth > 0) ? ID_DARK_SHADOW : ID_LIGHT_SHADOW);
		depth = Abs(depth);
		for (int i = 0; i < depth; i++)
		{
			display->Line(screenID, region.right, region.top, region.right,
				region.bottom, darkShadow, 1, FALSE, &clip);
			display->Line(screenID, region.left, region.bottom, region.right - 1,
				region.bottom, darkShadow, 1, FALSE, &clip);
			display->Line(screenID, region.left, region.top, region.right - 1,
				region.top, lightShadow, 1, FALSE, &clip);
			display->Line(screenID, region.left, region.top + 1, region.left,
				region.bottom - 1, lightShadow, 1, FALSE, &clip);
			--region;
		}
	}
	else if (searchID == ID_BORDER)
	{
		display->Rectangle(screenID, region, NULL, 0, TRUE, FALSE,
			(parent->parent ? &clip : NULL));
		region.right--;
		region.bottom--;
	}
	else if (depth > 0)
	{
//		UI_PALETTE *palette = parent->LogicalPalette(ccode);		// BUG.1732.a(internal)
		UI_PALETTE *palette = parent->lastPalette;
		char shadow = _iLowerBox;
		display->Text(screenID, region.right, region.top, &shadow, palette, 1, TRUE, FALSE, &clip);
		shadow = _iFullBox;
		for (int row = region.top + 1; row < region.bottom; row++)
			display->Text(screenID, region.right, row, &shadow, palette, 1, TRUE, FALSE, &clip);
		shadow = palette->fillCharacter;
		display->Text(screenID, region.left, region.bottom, &shadow, palette, 1, TRUE, FALSE, &clip);
		shadow = _iUpperBox;
		for (int column = region.left + 1; column <= region.right; column++)
			display->Text(screenID, column, region.bottom, &shadow, palette, 1, TRUE, FALSE, &clip);
		region.right--;
		region.bottom--;
	}
	else
	{
		UI_PALETTE *palette = parent->LogicalPalette(ccode);
		display->Rectangle(screenID, region, palette, 0, TRUE, FALSE, &clip);
		region.left++;
		region.bottom--;
	}

	// Fill in the region.
	if (fillRegion && region.bottom > region.top)
		display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE, &clip);
	return (ccode);
}

EVENT_TYPE UI_WINDOW_OBJECT::DrawText(SCREENID screenID, UI_REGION &region,
	const char *text, UI_PALETTE *palette, int fillRegion, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
	UI_DISPLAY *display = this->display;
	UI_REGION clip = this->clip;
#endif

	// Fill in the region.
	if (!palette)
		palette = lastPalette;
	display->VirtualGet(screenID, region);
	if (fillRegion)
		display->Rectangle(screenID, region, palette, 0, TRUE, FALSE, &clip);

	// Draw the text.
	if (text && text[0])													// BUG.1435
	{
		UI_REGION clipRegion;
		clip.Overlap(region, clipRegion);
		int position = region.left;												// BUG.1271
		if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
			position = region.right - display->TextWidth(text, screenID, font);
		else if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
			// Add 1 to round during divide, and 1 more for the
			// white space at the right side of the character.
			position = region.left + (region.right - region.left + 2 - display->TextWidth(text, screenID, font)) / 2;
		if (position > region.left)
			region.left = position;
		position = region.top;													// BUG.1271
		position += (region.Height() - display->TextHeight(text, screenID, font)) / 2;
		if (position > region.top)
			region.top = position;
		display->Text(screenID, region.left, region.top, text, palette, -1, FALSE, FALSE, &clipRegion, font);
		if (display->isText && !FlagSet(font, IGNORE_UNDERSCORE))	     //BUG.1254
		{
			int offset = 0; // Count extra ZIL_HOTMARK characters.
			for (const char *hotChar = strchr(text, ZIL_HOTMARK); hotChar;
				hotChar = strchr(++hotChar, ZIL_HOTMARK))
			{
				++offset;
				++hotChar;
				if (*hotChar != ZIL_HOTMARK)
				{
					palette = LogicalPalette(ccode, ID_HOT_KEY);
					display->Text(screenID, region.left + (int)(hotChar - text) - offset,
						region.top, hotChar, palette, 1, FALSE, FALSE, &clipRegion, font);
					break;
				}
			}
		}
	}

	display->VirtualPut(screenID);
	return (ccode);
}

EVENT_TYPE UI_WINDOW_OBJECT::Event(const UI_EVENT &event)
{
	static initializedTime = FALSE;
	static UI_TIME lastTime;
	if (!initializedTime)
	{
		lastTime.Import();
		initializedTime = TRUE;
	}

	// Switch on the system event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW_OBJECT);
	switch (ccode)
	{
	case S_INITIALIZE:
		if (!numberID && parent)
		{
			for (UI_WINDOW_OBJECT *object = parent; object->parent; object = object->parent)
				;
			numberID = object->NumberID() ? object->NumberID() : 1;
			object->NumberID(numberID + 1);
			if (stringID[0] == '\0')
				sprintf(stringID, "FIELD_%d", numberID);
		}
		RegionConvert(relative, (parent && !FlagSet(woFlags, WOF_NON_FIELD_REGION)) ? FALSE : TRUE);
		break;

	case S_DEINITIALIZE:
		screenID = 0;
		break;

	case S_SIZE:
	case S_CREATE:
		woStatus |= WOS_REDISPLAY;
		if (!screenID)
		{
			if (!parent || FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT | WOAF_TEMPORARY))
				screenID = ++(windowManager->currentScreenID);
			else
				screenID = parent->screenID;
		}
		if (!parent || FlagSet(woAdvancedFlags, WOAF_TEMPORARY))
			RegionMax(this);
		else
		{
			parent->RegionMax(this);
			if (parent->Inherited(ID_GROUP))
			{
				true.top -= display->cellHeight / 4;
				true.bottom -= display->cellHeight / 4;
			}
		}
		break;

	case S_RESET_DISPLAY:
		{
		UI_REGION region;
		region = relative;
		if (!display->isText && FlagSet(woStatus, WOS_GRAPHICS) && FlagSet(woFlags, WOF_MINICELL))
		{
			long miniNX = display->miniNumeratorX, miniDX = display->miniDenominatorX;
			long miniNY = display->miniNumeratorY, miniDY = display->miniDenominatorY;
			region.left = (int)((miniDX * region.left) / (miniNX * display->cellWidth));
			region.top = (int)((miniDY * region.top) / (miniNY * display->cellHeight));
			region.right = (int)((miniDX * region.right) / (miniNX * display->cellWidth));
			region.bottom = (int)((miniDY * region.bottom) / (miniNY * display->cellHeight));
			woStatus &= ~WOS_GRAPHICS;
			woStatus |= WOS_REDISPLAY;
		}
		else if (!display->isText && FlagSet(woStatus, WOS_GRAPHICS))
		{
			int width = (region.right - region.left) / display->cellWidth;
			int height = (region.bottom - region.top) / display->cellHeight;
			region.left /= display->cellWidth;
			region.bottom /= display->cellHeight;
			region.right = region.left + width;
			region.top = region.bottom - height;
			woStatus &= ~WOS_GRAPHICS;
			woStatus |= WOS_REDISPLAY;
		}
		relative = region;
		true = relative;
		screenID = NULL;
		}
		break;

	case S_REGION_DEFINE:
		display->RegionDefine(screenID, true);
		break;

	case S_CURRENT:
		if (parent && FlagSet(parent->woStatus, WOS_REDISPLAY))
			woStatus |= WOS_REDISPLAY;
		// Continue to S_NON_CURRENT.
	case S_NON_CURRENT:
		// Check the object status.
		if (ccode == S_CURRENT)
		{
			if (!FlagSet(woStatus, WOS_CURRENT))
				woStatus |= WOS_REDISPLAY;
			woStatus |= WOS_CURRENT;
		}
		else
		{
			if (FlagSet(woStatus, WOS_CURRENT))
				woStatus |= WOS_REDISPLAY;
			woStatus &= ~WOS_CURRENT;
		}
		// Continue to S_DISPLAY_ACTIVE.
	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		{
		UI_PALETTE *palette = LogicalPalette(ccode);
		if (FlagSet(woStatus, WOS_REDISPLAY) || palette != lastPalette ||
			(ccode != S_NON_CURRENT && true.Overlap(event.region)))
		{
			woStatus |= WOS_REDISPLAY;
			lastPalette = palette;
		}
		// Call the associated user or validate function.
		if (!FlagSet(woStatus, WOS_INTERNAL_ACTION) &&
			(ccode == S_CURRENT || ccode == S_NON_CURRENT))
			ccode = UserFunction(event, ccode);
		if (FlagsSet(woStatus, WOS_OWNERDRAW | WOS_REDISPLAY))
		{
			display->VirtualGet(screenID, true);
			if (DrawItem(event, ccode))
				woStatus &= ~WOS_REDISPLAY;
			display->VirtualPut(screenID);
		}
		}
		break;

	case S_REDISPLAY:
		if (screenID)
		{
			// Find the root to determine activity.
			UI_EVENT event;
			event.type = FlagSet(woStatus, WOS_CURRENT) ? S_CURRENT : S_DISPLAY_ACTIVE;
			UI_WINDOW_OBJECT *root = this;
			while (root->parent)
			{
				root = root->parent;
				if (!FlagSet(root->woStatus, WOS_CURRENT))
					event.type = S_DISPLAY_ACTIVE;
			}
			if (!FlagSet(root->woStatus, WOS_CURRENT))
				event.type = S_DISPLAY_INACTIVE;
			event.region = true;
			WOS_STATUS _woStatus = woStatus;
			woStatus |= WOS_REDISPLAY | WOS_INTERNAL_ACTION;	// Don't call user-function.
			Event(event);
			if (!FlagSet(_woStatus, WOS_INTERNAL_ACTION))
				woStatus &= ~WOS_INTERNAL_ACTION;
		}
//		else if (parent && parent->screenID)
//			return (parent->Event(event));
		break;

	case S_ADD_OBJECT:
	case S_SUBTRACT_OBJECT:
		if (parent)
			parent->Event(event);
		break;

	case L_HELP:
		if (parent && helpContext == NO_HELP_CONTEXT)
			return (S_UNKNOWN);
		else if (helpSystem)
			helpSystem->DisplayHelp(windowManager, helpContext);
		break;

	case L_VIEW:
		if (true.Overlap(event.position))
			eventManager->DeviceState(E_MOUSE, DM_VIEW);
		break;

	case L_SELECT:
		ccode = UserFunction(event, ccode);
		break;

	case L_BEGIN_SELECT:
	case L_CONTINUE_SELECT:
	case L_END_SELECT:
		{
		UI_EVENT dEvent;
		if (!true.Overlap(event.position) || !clip.Overlap(event.position))
			ccode = S_UNKNOWN;
#if defined(ZIL_EDIT)
		if (ccode == L_END_SELECT && !parent && FlagSet(woStatus, WOS_EDIT_MODE))
		{
			UI_TIME currentTime;
			long elapsedTime = currentTime - lastTime;						// BUG.1228
			if (Abs(elapsedTime) < doubleClickRate)
			{
				dEvent.type = D_EDIT_OBJECT;
				dEvent.rawCode = searchID;
				dEvent.data = this;
				eventManager->Put(dEvent);
			}
			else
				lastTime = currentTime;
			break;
		}
#endif

		// Check for an edit mode object.
		if (ccode != L_BEGIN_SELECT || !FlagSet(woStatus, WOS_EDIT_MODE))
			break;

#if defined(ZIL_EDIT)
		dEvent.position = event.position;
		int hOffset = 1, vOffset = 0;
		if (!display->isText)
			hOffset = vOffset = 5;
		else if (true.right == true.left)
			hOffset = 0;
		if (event.position.column >= true.left + hOffset && event.position.line >= true.top + vOffset &&
			event.position.column <= true.right - hOffset && event.position.line <= true.bottom - vOffset)
		{
			dEvent.type = L_MOVE;
			dEvent.rawCode = M_LEFT_CHANGE | M_TOP_CHANGE | M_RIGHT_CHANGE | M_BOTTOM_CHANGE;
		}
		else
		{
			dEvent.type = L_SIZE;
			dEvent.rawCode = 0;
			if (event.position.column <= true.left + hOffset)
				dEvent.rawCode |= M_LEFT_CHANGE;
			else if (event.position.column >= true.right - hOffset)
				dEvent.rawCode |= M_RIGHT_CHANGE;
			if (event.position.line <= true.top + vOffset)
				dEvent.rawCode |= M_TOP_CHANGE;
			else if (event.position.line >= true.bottom - vOffset)
				dEvent.rawCode |= M_BOTTOM_CHANGE;
		}
		if (parent)
			Modify(dEvent);

		// Set the current object.
		dEvent.type = D_SET_OBJECT;
		dEvent.rawCode = searchID;
		dEvent.data = this;
		eventManager->Put(dEvent);
		dEvent.type = D_SET_POSITION;
		dEvent.rawCode = M_LEFT | M_LEFT_CHANGE;
		dEvent.position = event.position;
		eventManager->Put(dEvent);
		if (!parent)
			break;
		UI_TIME currentTime;
		long elapsedTime = currentTime - lastTime;							// BUG.1228
		if (Abs(elapsedTime) < doubleClickRate)
		{
			dEvent.type = D_EDIT_OBJECT;
			dEvent.rawCode = searchID;
			dEvent.data = this;
			eventManager->Put(dEvent);
		}
		else
			lastTime = currentTime;
#endif
		}
		break;

#if defined(ZIL_EDIT)
	case L_BEGIN_ESCAPE:
		if (FlagSet(woStatus, WOS_EDIT_MODE))
		{
			UI_EVENT dEvent;
			dEvent.type = D_SET_POSITION;
			dEvent.rawCode = event.rawCode;
			dEvent.position = event.position;
			eventManager->Put(dEvent);
			break;
		}
		// Continue to default.
#endif
	default:
		ccode = S_UNKNOWN;
		break;
	}

	// Return the control code.
	return (ccode);
}

void UI_WINDOW_OBJECT::Modify(const UI_EVENT &event)
{
#if defined(ZIL_OPTIMIZE)
	UI_DISPLAY *display = this->display;
#endif

	static UI_PALETTE rubberBandPalette =									// BUG.1385
	{
		'\260', attrib(WHITE, BLACK), attrib(MONO_HIGH, MONO_NORMAL),
		PTN_SOLID_FILL, LIGHTCYAN, BLACK, BW_WHITE, BW_BLACK, GS_GRAY, GS_BLACK
	};

	// Make sure we can move the object.
	if ((parent && FlagSet(woFlags, WOF_NON_FIELD_REGION)) ||
		(event.type == L_SIZE && Inherited(ID_ICON)))
		return;

	// Initialize the modify variables.
	UI_EVENT mEvent; mEvent.type = 0;
	UI_REGION newRegion = true, oldRegion = true;
	RAW_CODE sizeFlags = event.rawCode;
	UI_POSITION origin = event.position;
	EVENT_TYPE operation = L_SIZE;
	if (event.type == L_MOVE || Inherited(ID_ICON))
	{
		sizeFlags = M_LEFT_CHANGE | M_TOP_CHANGE | M_RIGHT_CHANGE | M_BOTTOM_CHANGE;
		operation = L_MOVE;
	}
	else if (!event.rawCode)
		sizeFlags = M_RIGHT_CHANGE | M_BOTTOM_CHANGE;

	// Determine the absolute region.
	UI_REGION absolute;
	if (parent)
		absolute = clip;
	else
	{
		absolute.left = absolute.top = 0;
		absolute.right = display->columns - 1;
		absolute.bottom = display->lines - 1;
	}

	// Determine the minimum height and width of the object.
	int minHeight = display->cellHeight;
	if (parent && (!Inherited(ID_WINDOW) || Inherited(ID_LIST)))			// BUG.General
		minHeight -= (display->preSpace + display->postSpace);
	else if (!display->isText)
		minHeight += 2 * UIW_BORDER::width;
	int minWidth = 2 * display->cellWidth;
	if (Inherited(ID_WINDOW) && !Inherited(ID_SCROLL_BAR)
		&& !Inherited(ID_LIST) && !Inherited(ID_GROUP))
		minWidth *= 5;

	// Check for special object movement.
	if (operation != L_MOVE)
	{
		if (Inherited(ID_BUTTON))
		{
			BTF_FLAGS btFlags;
			Information(GET_FLAGS, &btFlags, ID_BUTTON);
//			if (FlagSet(btFlags, BTF_AUTO_SIZE))							// BUG.GENERAL
			if (FlagSet(btFlags, BTF_AUTO_SIZE | BTF_RADIO_BUTTON | BTF_CHECK_BOX))
				sizeFlags &= ~(M_TOP_CHANGE | M_BOTTOM_CHANGE);
		}
		else if ((!Inherited(ID_WINDOW) || Inherited(ID_COMBO_BOX)) && searchID < 3000)
			sizeFlags &= ~(M_TOP_CHANGE | M_BOTTOM_CHANGE);
		else if (Inherited(ID_SCROLL_BAR))
		{
			SBF_FLAGS sbFlags = SBF_NO_FLAGS;
			Information(GET_FLAGS, &sbFlags, ID_SCROLL_BAR);
			if (!FlagSet(sbFlags, SBF_VERTICAL))
			{
				minHeight = true.bottom - true.top + 1;
				sizeFlags &= ~(M_TOP_CHANGE | M_BOTTOM_CHANGE);
			}
			if (!FlagSet(sbFlags, SBF_HORIZONTAL))
			{
				minWidth = true.right - true.left + 1;
				sizeFlags &= ~(M_LEFT_CHANGE | M_RIGHT_CHANGE);
			}
			if (!sizeFlags)
				return;
		}
	}

	// Reverse the window region.
	int xJump = display->cellWidth;
	int yJump = display->cellHeight;
	EVENT_TYPE ccode = S_UNKNOWN;

	// Prevent sizing illusion caused by double XOR with mouse cursor. 
	if (display->isText)
		eventManager->DeviceState(E_MOUSE, D_HIDE);

	int deltaX = 0, deltaY = 0;
	UI_PALETTE *oldPalette = display->xorPalette;							// BUG.1385
	display->xorPalette = &rubberBandPalette;
	display->Rectangle(ID_DIRECT, newRegion, display->xorPalette, 1, FALSE, TRUE);
	do
	{
		UI_EVENT event;
		eventManager->Get(event);
		ccode = UI_EVENT_MAP::MapEvent(eventMapTable, event);
		switch (ccode)
		{
		case L_BEGIN_SELECT:
			if (event.position.column < newRegion.left ||		// BUG.1667
			    event.position.column > newRegion.right ||
			    event.position.line < newRegion.top ||
			    event.position.line > newRegion.bottom)
			{
				ccode = L_END_SELECT;
				break;
			}
			origin = event.position;
			// Continue to L_CONTINUE_SELECT.
		case L_CONTINUE_SELECT:
			mEvent = event;
			mEvent.type = L_VIEW;
			if (event.position.column > absolute.right)
				event.position.column = absolute.right;
			else if (event.position.column < absolute.left)					// BUG.1230
				event.position.column = absolute.left;
			if (event.position.line > absolute.bottom)
				event.position.line = absolute.bottom;
			else if (event.position.line < absolute.top)					// BUG.1230
				event.position.line = absolute.top;
			deltaX = event.position.column - origin.column;		// Compute the change.
			deltaY = event.position.line - origin.line;
			break;

		case L_END_SELECT:
			break;

		case L_UP:
		case L_DOWN:
			deltaY = (ccode == L_UP) ? deltaY - yJump : deltaY + yJump;
			if (true.bottom + deltaY < absolute.top ||						// BUG.1397
				true.top + deltaY > absolute.bottom)
				deltaY = (ccode == L_UP) ? deltaY + yJump : deltaY - yJump;
			break;

		case L_LEFT:
		case L_RIGHT:
			deltaX = (ccode == L_LEFT) ? deltaX - xJump : deltaX + xJump;
			if (true.right + deltaX < absolute.left ||						// BUG.1397
				true.left + deltaX > absolute.right)
				deltaX = (ccode == L_LEFT) ? deltaX + xJump : deltaX - xJump;
			break;

		default:
			// Send all user messages to the window manager.
			if (event.type > 9999)
				windowManager->Event(event);
			break;
		}

#if defined(ZIL_EDIT)
		if (FlagSet(woStatus, WOS_EDIT_MODE))
		{
			// Check the absolute region.
			if (FlagSet(sizeFlags, M_LEFT_CHANGE))
			{
				if (true.left + deltaX < absolute.left)
					deltaX = absolute.left - true.left;
			}
			if (FlagSet(sizeFlags, M_TOP_CHANGE))
			{
				if (true.top + deltaY < absolute.top)
					deltaY = absolute.top - true.top;
			}
		}
		
		long miniNX = display->miniNumeratorX, miniDX = display->miniDenominatorX;
		long miniNY = display->miniNumeratorY, miniDY = display->miniDenominatorY;

		// Check for a cell boundary move or size.
		if (FlagSet(woStatus, WOS_EDIT_MODE) && FlagSet(woFlags, WOF_MINICELL))
		{
			long value = deltaX;
			int ceil = (value >= 0) ? (int)(miniDX - 1) : int(1 - miniDX);
			value = (value * miniDX) / (miniNX * display->cellWidth);
			value = (value * miniNX * display->cellWidth + ceil) / miniDX;
			deltaX = (int)value;

			value = deltaY;
			ceil = (value >= 0) ? (int)(miniDY - 1) : (int)(1 - miniDY);
			value = (value * miniDY) / (miniNY * display->cellHeight);
			value = (value * miniNY * display->cellHeight + ceil) / miniDY;
			deltaY = (int)value;
		}
		else if (FlagSet(woStatus, WOS_EDIT_MODE))
		{
			int value = (deltaX > 0) ? deltaX + display->cellWidth / 2 : deltaX - display->cellWidth / 2;
			deltaX = value / display->cellWidth * display->cellWidth;
			value = (deltaY > 0) ? deltaY + display->cellHeight / 2 : deltaY - display->cellHeight / 2;
			deltaY = value / display->cellHeight * display->cellHeight;
		}
#endif

		if (FlagSet(sizeFlags, M_LEFT_CHANGE))
			newRegion.left = true.left + deltaX;
		if (FlagSet(sizeFlags, M_TOP_CHANGE))
			newRegion.top = true.top + deltaY;
		if (FlagSet(sizeFlags, M_RIGHT_CHANGE))
			newRegion.right = true.right + deltaX;
		if (FlagSet(sizeFlags, M_BOTTOM_CHANGE))
			newRegion.bottom = true.bottom + deltaY;

		// Check for minimum size.
		if (operation == L_SIZE)
		{
			if (newRegion.left + minWidth - 1 > newRegion.right &&
			  !FlagSet(woStatus, WOS_MINIMIZED))
			{
				newRegion.left = oldRegion.left;
				newRegion.right = oldRegion.right;
			}
			if (newRegion.top + minHeight - 1 > newRegion.bottom &&
			  !FlagSet(woStatus, WOS_MINIMIZED))
			{
				newRegion.top = oldRegion.top;
				newRegion.bottom = oldRegion.bottom;
			}
		}
		// Move sizing rectangle.
		if (oldRegion != newRegion)
		{
			display->RectangleXORDiff(oldRegion, newRegion);
			oldRegion = newRegion;
		}
	} while (ccode != L_END_SELECT && ccode != L_SELECT && ccode != L_CANCEL);

	// Restore the region.
	display->Rectangle(ID_DIRECT, newRegion, display->xorPalette, 1, FALSE, TRUE);
	display->xorPalette = oldPalette;										// BUG.1385

	// Restore mouse cursor. 
 	if (display->isText)
		eventManager->DeviceState(E_MOUSE, D_ON);

	if (ccode == L_END_SELECT)						// BUG.1697
		eventManager->Put(UI_EVENT(L_END_SELECT));

	if (ccode == L_CANCEL || newRegion == true)
		return;				// Do not change the object region.

	relative.left += newRegion.left - true.left;
	relative.top += newRegion.top - true.top;
	relative.right += newRegion.right - true.right;
	relative.bottom += newRegion.bottom - true.bottom;

	UI_REGION region = true;
	if (FlagSet(woStatus, WOS_MINIMIZED))									// BUG.General
		Event(UI_EVENT(S_CREATE));											// BUG.A737
	else
	{
		UI_WINDOW_OBJECT *root = this;
		while (root->parent)
			root = root->parent;
		root->Event(UI_EVENT(S_SIZE));
	}

	WOS_STATUS _woStatus = woStatus;
	woStatus |= WOS_INTERNAL_ACTION;	// Don't call user-function.
	if (!parent)
	{
		if (display->isText)
			display->VirtualGet(ID_SCREEN, 0, 0, display->columns, display->lines);
		display->RegionDefine(ID_SCREEN, 0, 0, display->columns, display->lines);
		for (UI_WINDOW_OBJECT *object = windowManager->Last(); object; object = object->Previous())
			object->Event(UI_EVENT(S_REGION_DEFINE));
		display->Rectangle(ID_SCREEN, region, display->backgroundPalette, 0, TRUE);
		for (object = windowManager->Last(); object; object = object->Previous())
			if (object == this)
				object->Event(UI_EVENT(S_CURRENT, 0, true));
			else if (object->true.Overlap(region))
			{
				object->Event(UI_EVENT(S_DISPLAY_INACTIVE, 0, region));
				if (display->isText)	// Account for window shadowing.
				{
					region.left = Min(object->true.left, region.left);
					region.top = Min(object->true.top, region.top);
					region.right = Max(object->true.right + 1, region.right);
					region.bottom = Max(object->true.bottom + 1, region.bottom);
				}
			}
		if (display->isText)
			display->VirtualPut(ID_SCREEN);
	}
	else
	{
		if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
			parent->Event(UI_EVENT(S_REGION_DEFINE));

/* START BLOCK COMMENT														// BUG.7163
**			if (!true.Overlap(region))
**			{
**				if (FlagSet(parent->woAdvancedFlags, WOAF_MDI_OBJECT))		// BUG.1231
**					region = clip;
**				parent->Event(UI_EVENT(S_CURRENT, 0, region));
**	//			woStatus |= WOS_REDISPLAY;
**	//			parent->Event(UI_EVENT(S_CURRENT, 0, true));
**			}
**			else
END BLOCK COMMENT */
		{
			region.left = Min(region.left, true.left);
			region.top = Min(region.top, true.top);
			region.right = Max(region.right, true.right);
			region.bottom = Max(region.bottom, true.bottom);
			woStatus |= WOS_REDISPLAY;
			parent->Event(UI_EVENT(S_CURRENT, 0, region));
		}
	}
	if (!FlagSet(_woStatus, WOS_INTERNAL_ACTION))
		woStatus &= ~WOS_INTERNAL_ACTION;
	if (mEvent.type)
		eventManager->Put(mEvent, Q_BEGIN);
}

