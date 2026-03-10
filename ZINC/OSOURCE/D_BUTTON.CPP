//	Zinc Interface Library - D_BUTTON.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

const int BUTTON_OFFSET = 4;

// ----- UIW_BUTTON ---------------------------------------------------------

EVENT_TYPE UIW_BUTTON::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
	SCREENID screenID = this->screenID;
	UI_DISPLAY *display = this->display;
	UI_REGION clip = this->clip;
	UI_PALETTE *lastPalette = this->lastPalette;
#endif
	UI_REGION region = true;

	// Handle text mode buttons.
	if (display->isText)
	{
		if (!text)
			return (ccode);
		if (FlagSet(woFlags, WOF_BORDER))
			DrawBorder(screenID, region, FALSE, ccode);
		DrawShadow(screenID, region, FlagSet(btStatus, BTS_DEPRESSED) ? -depth : depth, TRUE, ccode);
		if (FlagSet(btFlags, BTF_CHECK_BOX))
		{
			display->Text(screenID, region.left, region.top,
				FlagSet(woStatus, WOS_SELECTED) ? _iActiveCheck : _iInactiveCheck,
				lastPalette, -1, FALSE, FALSE, &clip);
			region.left += 4;
		}
		else if (FlagSet(btFlags, BTF_RADIO_BUTTON))
		{
			display->Text(screenID, region.left, region.top,
				FlagSet(woStatus, WOS_SELECTED) ? _iActiveRadio : _iInactiveRadio,
				lastPalette, -1, FALSE, FALSE, &clip);
			region.left += 4;
		}
		DrawText(screenID, region, text, lastPalette, TRUE, ccode);
		return (ccode);
	}

	// Draw the border and shadow.
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);
	UI_PALETTE *outline = LogicalPalette(ccode, ID_OUTLINE);
	if (depth > 1)
	{
		display->Line(screenID, region.left + 1, region.top,
			region.right - 1, region.top, outline, 1, FALSE, &clip);
		display->Line(screenID, region.left + 1, region.bottom,
			region.right - 1, region.bottom, outline, 1, FALSE, &clip);
		display->Line(screenID, region.left, region.top + 1,
			region.left, region.bottom - 1, outline, 1, FALSE, &clip);
		display->Line(screenID, region.right, region.top + 1,
			region.right, region.bottom - 1, outline, 1, FALSE, &clip);
		--region;
	}
	if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON) && ccode != S_CURRENT)
		lastPalette = LogicalPalette(ccode, ID_WINDOW);
 	else if (FlagSet(btFlags, BTF_NO_3D) && ccode != S_CURRENT)
	{
		if (parent->Inherited(ID_LIST) && !parent->Inherited(ID_COMBO_BOX))	// BUG.General
			lastPalette = LogicalPalette(ccode, ID_LIST);
		else
			lastPalette = LogicalPalette(ccode, ID_WINDOW);
	}
	if (FlagSet(btFlags, BTF_NO_3D))
	{
		int left = parent->Inherited(ID_VT_LIST) ? region.left - relative.left : region.left;
		display->Rectangle(screenID, left, region.top, region.right,
			region.bottom, lastPalette, 0, TRUE, FALSE, &clip);
	}
	else
		DrawShadow(screenID, region, FlagSet(btStatus, BTS_DEPRESSED) ? -depth : depth, TRUE, ccode);

	// Draw the text.
	if (!FlagSet(woFlags, WOF_JUSTIFY_CENTER))				// BUG.internal
	{
		region.left += BUTTON_OFFSET;
		if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
			region.right -= BUTTON_OFFSET;
	}
	if (!bitmapArray && !colorBitmap && !FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON))
	{
		if (text)
			DrawText(screenID, region, text, lastPalette, FALSE, ccode);
		return (ccode);
	}

	// Draw the bitmap.
	int fieldWidth = region.right - region.left + 1;
	int fieldHeight = region.bottom - region.top + 1;
	if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON))
		bitmapWidth = bitmapHeight = display->TextHeight(NULL, screenID, font) + 2;
	int left = region.left;
	if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
		left = region.right - bitmapWidth + 1;
	else if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
		left = region.left + (fieldWidth - bitmapWidth) / 2;
	int top = (FlagSet(woFlags, WOF_JUSTIFY_CENTER) && text) ?
		region.top + 2 : region.top + (fieldHeight - bitmapHeight) / 2;
	int right = left + bitmapWidth - 1;
	int bottom = top + bitmapHeight - 1;

	if (FlagSet(btFlags, BTF_CHECK_BOX))
	{
		display->Rectangle(screenID, left, top, right, bottom, lastPalette, 1, TRUE, FALSE, &clip);
		if (FlagSet(woStatus, WOS_SELECTED))
		{
			display->Line(screenID, left, top, right, bottom, lastPalette, 1, FALSE, &clip);
			display->Line(screenID, right, top, left, bottom, lastPalette, 1, FALSE, &clip);
		}
	}
	else if (FlagSet(btFlags, BTF_RADIO_BUTTON))
	{
		int radius = bitmapWidth / 2;
		display->Ellipse(screenID, left + radius, top + radius, 0, 360,
			radius, radius, lastPalette, TRUE, FALSE, &clip);
		if (FlagSet(woStatus, WOS_SELECTED))
			display->Ellipse(screenID, left + radius, top + radius, 0, 360,
				radius / 2, radius / 2, outline, TRUE, FALSE, &clip);
	}
	else if (bitmapArray || colorBitmap)
		display->Bitmap(screenID, left, top, bitmapWidth, bitmapHeight,
			bitmapArray, NULL, &clip, &colorBitmap, &monoBitmap);

	// Draw the text.
	if (text)
	{
		WOF_FLAGS flags = woFlags;
		if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
			region.top = region.bottom - display->cellHeight + 4;
		else if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
		{
			region.right -= bitmapWidth - BUTTON_OFFSET;
			woFlags &= ~(WOF_JUSTIFY_RIGHT);
		}
		else
			region.left += bitmapWidth + BUTTON_OFFSET;
		DrawText(screenID, region, text, lastPalette, FALSE, ccode);
		woFlags = flags;
	}

	// Return the control code.
#if defined(ZIL_OPTIMIZE)
	this->lastPalette = lastPalette;
#endif
	return (ccode);
}

EVENT_TYPE UIW_BUTTON::Event(const UI_EVENT &event)
{
	static initializedTime = FALSE;
	static UI_TIME lastTime;
	if (!initializedTime)
	{
		lastTime.Import();
		initializedTime = TRUE;
	}
	UI_WINDOW_OBJECT *object;

	WOS_STATUS oldStatus = woStatus;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_BUTTON);
	switch (ccode)
	{
	case S_INITIALIZE:
	case S_CREATE:
		UI_WINDOW_OBJECT::Event(event);

		// Compute the button region.
		if (!display->isText)
		{
			if (FlagSet(btFlags, BTF_AUTO_SIZE))
			{
				int offset = 2;
				if (FlagSet(woFlags, WOF_BORDER))
					offset += 2;
				if (!FlagSet(btFlags, BTF_NO_3D))
					offset += 6;
				int height = 0;
				if (bitmapHeight)
				{
					height = bitmapHeight + offset;
					if (text && FlagSet(woFlags, WOF_JUSTIFY_CENTER))
						height += display->cellHeight;
				}
				height = Max(height, display->cellHeight * 10 / 9);
				relative.top = relative.bottom - height;
				true.top = true.bottom - height;
			}
		}
		else if (FlagSet(btFlags, BTF_AUTO_SIZE) && depth == 2)
			relative.top = relative.bottom - 1;
		else
			relative.bottom = relative.top;
		if (ccode == S_INITIALIZE)
			break;

		// Convert the bitmap array to a handle if possible.
		if (bitmapArray && !colorBitmap)
		{
			display->BitmapArrayToHandle(screenID, bitmapWidth, bitmapHeight,
				bitmapArray, NULL, &colorBitmap, &monoBitmap);
			if (colorBitmap && bitmapArray && !FlagSet(btFlags, BTF_STATIC_BITMAPARRAY))
			{
				delete bitmapArray;
				bitmapArray = NULL;
			}
		}
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		UI_WINDOW_OBJECT::Event(event);
		if (FlagSet(woStatus, WOS_REDISPLAY))
		{
			DrawItem(event, ccode);
			woStatus &= ~WOS_REDISPLAY;
		}
		break;

	case L_LEFT:
	case L_UP:
		for (object = Previous(); object; object = object->Previous())
		{
			if (object->Inherited(ID_BUTTON) && !FlagSet(object->woFlags, WOF_NON_SELECTABLE))
				break;
		}
		if (!object)
		{
			for (object = this; object->Next(); object = object->Next())
				;
			for (; object; object = object->Previous())
			{
				if (object->Inherited(ID_BUTTON) && !FlagSet(object->woFlags, WOF_NON_SELECTABLE))
					break;
			}
		}
		if (object && object != this)
		{
			UI_EVENT event(S_ADD_OBJECT);
			event.data = object;
			parent->Event(event);
		}
		break;

	case L_RIGHT:
	case L_DOWN:
		for (object = Next(); object; object = object->Next())
		{
			if (object->Inherited(ID_BUTTON) && !FlagSet(object->woFlags, WOF_NON_SELECTABLE))
				break;
		}
		if (!object)
		{
 			for (object = this; object->Previous(); object = object->Previous())
				;
			for (; object; object = object->Next())
			{
				if (object->Inherited(ID_BUTTON) && !FlagSet(object->woFlags, WOF_NON_SELECTABLE))
					break;
			}
		}
		if (object && object != this)
		{
			UI_EVENT event(S_ADD_OBJECT);
			event.data = object;
			parent->Event(event);
		}
		break;

	case L_BEGIN_SELECT:
	case L_CONTINUE_SELECT:
	case L_END_SELECT:
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UI_WINDOW_OBJECT::Event(event));
		{
		BTS_STATUS oldStatus = btStatus;
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (ccode == L_END_SELECT && !FlagSet(btStatus, BTS_DEPRESSED))
			break;	// Case where selection was down-click from another object.
		else if (ccode == S_UNKNOWN || ccode == L_END_SELECT)
			btStatus &= ~BTS_DEPRESSED;
		else
			btStatus |= BTS_DEPRESSED;
		if (oldStatus != btStatus && !FlagSet(btFlags, BTF_NO_3D) && depth)
		{
			UI_REGION region = true; --region;
//			if (display->isText)					// BUG.1623
				DrawItem(event, S_CURRENT);
//			else
//				DrawShadow(screenID, region,
//					 FlagSet(btStatus, BTS_DEPRESSED) ? -depth : depth,
//					 FALSE, ccode);
		}
		}

		if (FlagSet(btFlags, BTF_REPEAT) && ccode == L_BEGIN_SELECT)
		{
			UI_EVENT tEvent = event;
			UI_TIME currentTime;
			while (ccode != L_END_SELECT)
			{
				currentTime.Import();
				long elapsedTime = currentTime - lastTime;					// BUG.1228
				if (eventManager->Get(tEvent, Q_NO_BLOCK) != -2)
				{
					ccode = LogicalEvent(tEvent, ID_BUTTON);
					// Send all user messages to the window manager.
					if (tEvent.type > 9999)
						windowManager->Event(tEvent);
				}
				else if (true.Overlap(tEvent.position) &&
					Abs(elapsedTime) > repeatRate)
				{
					UserFunction(UI_EVENT(L_CONTINUE_SELECT), L_SELECT);
					lastTime = currentTime;
				}
			}
			Event(UI_EVENT(tEvent));
			break;
		}

		if ((FlagSet(btFlags, BTF_DOWN_CLICK) && ccode != L_BEGIN_SELECT) ||
			(!FlagSet(btFlags, BTF_DOWN_CLICK) && ccode != L_END_SELECT))
			break;
		// Continue to L_SELECT.
	case L_SELECT:
		{
		UI_TIME currentTime;
		long elapsedTime = currentTime - lastTime;							// BUG.1228
		if (FlagSet(btFlags, BTF_DOUBLE_CLICK) && ccode == L_END_SELECT &&
			userFunction && !parent->Inherited(ID_LIST) &&
			Abs(elapsedTime) < doubleClickRate)
		{
			UI_EVENT uEvent = event;
			ccode = (*userFunction)(this, uEvent, L_DOUBLE_CLICK);
		}
		else if (ccode == L_END_SELECT)
		{
			ccode = UserFunction(UI_EVENT(L_END_SELECT), L_SELECT);
			lastTime = currentTime;
		}
		else
			ccode = UserFunction(event, L_SELECT);
		if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON) && FlagSet(woStatus ^ oldStatus, WOS_SELECTED))
		{
			lastPalette = LogicalPalette(S_CURRENT);
			DrawItem(event, S_CURRENT);
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


