//	Zinc Interface Library - D_SCROLL.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_SCROLL_BAR -----------------------------------------------------

EVENT_TYPE UIW_SCROLL_BAR::Event(const UI_EVENT &event)
{
	UI_EVENT sEvent;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_SCROLL_BAR);
	switch (ccode)
	{
	case S_INITIALIZE:
		if (!FlagSet(woFlags, WOF_SUPPORT_OBJECT))
			;
		else if (FlagSet(sbFlags, SBF_VERTICAL) && parent)
			parent->Information(SET_VSCROLL, this);
		else if (FlagSet(sbFlags, SBF_HORIZONTAL) && parent)
			parent->Information(SET_HSCROLL, this);
		ccode = UIW_WINDOW::Event(event);
		break;

	case S_SIZE:
	case S_CREATE:
		{
		// Compute the object region.
		clipList.Destroy();
		UI_WINDOW_OBJECT::Event(event);
		if (display->isText)
		{
			if (FlagSet(sbFlags, SBF_CORNER | SBF_VERTICAL))
				true.left = true.right;
			if (FlagSet(sbFlags, SBF_CORNER | SBF_HORIZONTAL))
				true.top = true.bottom;
		}
		else
		{
			if (FlagSet(woFlags, WOF_BORDER))
			{
				true.right++;
				true.bottom++;
			}
			if (FlagSet(sbFlags, SBF_CORNER | SBF_VERTICAL))
				true.left = true.right - display->cellHeight * 3 / 4;
			else if (FlagSet(woFlags, WOF_BORDER))
				true.left--;
			if (FlagSet(sbFlags, SBF_CORNER | SBF_HORIZONTAL))
				true.top = true.bottom - display->cellHeight * 3 / 4;
			else if (FlagSet(woFlags, WOF_BORDER))
				true.top--;
		}
		// Compute the scroll button regions.
		clipList.Destroy();
		UI_REGION region = true;
		if (FlagSet(woFlags, WOF_BORDER) && !display->isText)
			--region;
		clipList.Add(new UI_REGION_ELEMENT(screenID, region));
		for (UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
		{
			object->Event(event);
			if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
				clipList.Split(screenID, object->true, FALSE);
		}
		for (object = First(); object; object = object->Next())
		{
			object->Event(event);
			if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
				clipList.Split(screenID, object->true, FALSE);
		}
		}
		break;

	case S_VSCROLL_SET:
	case S_HSCROLL_SET:
		scroll = event.scroll;
		// Continue to S_VSCROLL.
	case S_VSCROLL:
	case S_HSCROLL:
		{
		int tScroll = (ccode == S_VSCROLL || ccode == S_HSCROLL) ?
			scroll.current + event.scroll.delta : scroll.current;
		if (tScroll < scroll.minimum)
			tScroll = scroll.minimum;
		else if (tScroll > scroll.maximum)
			tScroll = scroll.maximum;
//		else if (ccode == S_VSCROLL_SET || ccode == S_HSCROLL_SET ||
		if (ccode == S_VSCROLL_SET || ccode == S_HSCROLL_SET ||				// BUG.1202
			(event.scroll.delta != 0 && tScroll != scroll.current))
		{
			scroll.current = tScroll;
			int size = FlagSet(sbFlags, SBF_VERTICAL) ?
				mButton->true.bottom - mButton->true.top :
				mButton->true.right - mButton->true.left;
			UI_REGION region = clipList.First()->region;
			UI_REGION update = mButton->true;
			if (FlagSet(sbFlags, SBF_VERTICAL) && display->isText)
			{
				long value = region.bottom - region.top;	// Use long for precision.
				value *= (scroll.current - scroll.minimum);	// Bug.A737
				if (scroll.maximum > scroll.minimum)
					value /= (scroll.maximum - scroll.minimum);
				mButton->true.top = mButton->true.bottom = region.top + (int)value;
			}
			else if (FlagSet(sbFlags, SBF_HORIZONTAL) && display->isText)
			{
				long value = region.right - region.left;	// Use long for precision.
				value *= (scroll.current - scroll.minimum);	// Bug.A737
				if (scroll.maximum > scroll.minimum)
					value /= (scroll.maximum - scroll.minimum);
				mButton->true.left = mButton->true.right = region.left + (int)value;
			}
			else if (FlagSet(sbFlags, SBF_VERTICAL))
			{
				long value = region.bottom - region.top - size + 2;	// Use long for precision.
				value *= (scroll.current - scroll.minimum);	// Bug.A737
				if (scroll.maximum > scroll.minimum)
					value /= (scroll.maximum - scroll.minimum);
				mButton->true.top = region.top - 1 + (int)value;
				mButton->true.bottom = mButton->true.top + size;
			}
			else
			{
				long value = region.right - region.left - size + 2;	// Use long for precision.
				value *= (scroll.current - scroll.minimum);	// Bug.A737
				if (scroll.maximum > scroll.minimum)
					value /= (scroll.maximum - scroll.minimum);
				mButton->true.left = region.left - 1 + (int)(value);
				mButton->true.right = mButton->true.left + size;
			}
			if (screenID && (mButton->true.top != update.top ||
				mButton->true.left != update.left))
			{
				update.left = Min(update.left, mButton->true.left);
				update.top = Min(update.top, mButton->true.top);
				update.right = Max(update.right, mButton->true.right);
				update.bottom = Max(update.bottom, mButton->true.bottom);
				Event(UI_EVENT(S_DISPLAY_ACTIVE, 0, update));
			}
		}
		}
		break;

	case L_BEGIN_SELECT:
		// Check for the edit mode.
		if (FlagSet(woStatus, WOS_EDIT_MODE) || FlagSet(sbFlags, SBF_CORNER) ||
			!true.Overlap(event.position))
			return (UI_WINDOW_OBJECT::Event(event));

		// Selection on the middle button.
		else if (mButton->true.Overlap(event.position))
		{
			UI_REGION startRegion = mButton->true;
			UI_REGION newRegion = startRegion;
			UI_POSITION position = event.position;
			UI_REGION updateRegion;
			if (clipList.First())	// Un-optimized for Zortech bug.
				updateRegion = clipList.First()->region;
			else
				updateRegion = true;
			if (!display->isText)
				++updateRegion;
			display->Rectangle(ID_DIRECT, newRegion, display->xorPalette, 1, FALSE, TRUE);
			while (ccode != L_END_SELECT)
			{
				eventManager->Get(sEvent);
				ccode = LogicalEvent(sEvent, ID_SCROLL_BAR);
				// Send all user messages to the window manager.
				if (sEvent.type > 9999)
					windowManager->Event(sEvent);
				else
				{
					int deltaX = sEvent.position.column - position.column;
					int deltaY = sEvent.position.line - position.line;
					if (FlagSet(sbFlags, SBF_VERTICAL))
						deltaX = 0;
					else if (newRegion.left + deltaX < updateRegion.left)
						deltaX = updateRegion.left - newRegion.left;
					else if (newRegion.right + deltaX > updateRegion.right)
						deltaX = updateRegion.right - newRegion.right;
					if (FlagSet(sbFlags, SBF_HORIZONTAL))
						deltaY = 0;
					else if (newRegion.top + deltaY < updateRegion.top)
						deltaY = updateRegion.top - newRegion.top;
					else if (newRegion.bottom + deltaY > updateRegion.bottom)
						deltaY = updateRegion.bottom - newRegion.bottom;
					if (deltaX || deltaY)
					{
						UI_REGION oldRegion = newRegion;
						newRegion.left += deltaX;
						newRegion.top += deltaY;
						newRegion.right += deltaX;
						newRegion.bottom += deltaY;
						position.column += deltaX;
						position.line += deltaY;
						if (!display->isText)
							display->RectangleXORDiff(oldRegion, newRegion);
					}
				}
			}
			display->Rectangle(ID_DIRECT, display->isText ? startRegion : newRegion, display->xorPalette, 1, FALSE, TRUE);
			if (!true.Overlap(sEvent.position))
				break;
			else if (FlagSet(sbFlags, SBF_VERTICAL))
			{
				long val1 = newRegion.top - updateRegion.top;
				long val2 = scroll.maximum - scroll.minimum;
				long val3 = updateRegion.bottom - updateRegion.top - display->cellHeight * 3 / 4;
				sEvent.type = S_VSCROLL;
				sEvent.scroll.delta = val3 ? (int)(val1 * val2 / val3) - scroll.current : 0;
			}
			else if (FlagSet(sbFlags, SBF_HORIZONTAL))
			{
				long val1 = newRegion.left - updateRegion.left;
				long val2 = scroll.maximum - scroll.minimum;
				long val3 = updateRegion.right - updateRegion.left - display->cellHeight * 3 / 4;
				sEvent.type = S_HSCROLL;
				sEvent.scroll.delta = val3 ? (int)(val1 * val2 / val3) - scroll.current : 0;
			}
			// Reset the scroll region.
			if (sEvent.scroll.delta && FlagSet(woFlags, WOF_SUPPORT_OBJECT))
				parent->Event(sEvent);
			else if (sEvent.scroll.delta)
				Event(sEvent);
			break;	// Do not continue to default operation of other buttons.
		}

		// Selection on the left/top button.
		else if (lButton->true.Overlap(event.position))
		{
			current = lButton;
			sEvent.type = FlagSet(sbFlags, SBF_VERTICAL) ? S_VSCROLL : S_HSCROLL;
			sEvent.scroll.delta = -scroll.delta;
			lButton->Event(event);
			sEvent.scroll.delta = -scroll.delta;
		}

		// Selection on the right/bottom button.
		else if (rButton->true.Overlap(event.position))
		{
			current = rButton;
			sEvent.type = FlagSet(sbFlags, SBF_VERTICAL) ? S_VSCROLL : S_HSCROLL;
			sEvent.scroll.delta = scroll.delta;
			rButton->Event(event);
		}

		// Selection of vertical page (previous/next).
		else if (FlagSet(sbFlags, SBF_VERTICAL))
		{
			current = NULL;
			sEvent.type = S_VSCROLL;
			sEvent.scroll.delta = (event.position.line < mButton->true.top) ?
				-scroll.showing : scroll.showing;
		}

		// Selection of horizontal page (previous/next).
		else if (FlagSet(sbFlags, SBF_HORIZONTAL))
		{
			current = NULL;
			sEvent.type = S_HSCROLL;
			sEvent.scroll.delta = (event.position.column < mButton->true.left) ?
				-scroll.showing : scroll.showing;
		}

		// Unknown option.
		else
			break;

		// Continue the process until L_END_SELECT is detected.
		{
		if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
			parent->Event(sEvent);
		else
			Event(sEvent);
		UI_EVENT tEvent;
		tEvent = event;
		UI_TIME currentTime, lastTime;
		while (ccode != L_END_SELECT)
		{
			currentTime.Import();
			long elapsedTime = currentTime - lastTime;						// BUG.1228
			if (eventManager->Get(tEvent, Q_NO_BLOCK) != -2)
			{
				ccode = LogicalEvent(tEvent, ID_SCROLL_BAR);
				// Send all user messages to the window manager.
				if (tEvent.type > 9999)
					windowManager->Event(tEvent);
				else if (current)
					Current()->Event(tEvent);
			}
			else if (current && Abs(elapsedTime) > repeatRate &&
				Current()->true.Overlap(tEvent.position))
			{
				if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
					parent->Event(sEvent);
				else
					Event(sEvent);
				lastTime = currentTime;
			}
		}
		current = mButton;
		eventManager->Put(tEvent, Q_BEGIN); // Let current window process end-select.
		}
		break;

	case L_CONTINUE_SELECT:
	case L_END_SELECT:
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- UIW_SCROLL_BUTTON --------------------------------------------------

static UCHAR _upArrow[49] =
{
	BACKGROUND,	BACKGROUND,	BACKGROUND,	BLACK,		BACKGROUND,	BACKGROUND,	BACKGROUND,
	BACKGROUND,	BACKGROUND,	BLACK,		BLACK,		BLACK,		BACKGROUND,	BACKGROUND,
	BACKGROUND,	BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BACKGROUND,
	BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,
	BACKGROUND,	BACKGROUND,	BLACK,		BLACK,		BLACK,		BACKGROUND,	BACKGROUND,
	BACKGROUND,	BACKGROUND,	BLACK,		BLACK,		BLACK,		BACKGROUND,	BACKGROUND,
	BACKGROUND,	BACKGROUND,	BLACK,		BLACK,		BLACK,		BACKGROUND,	BACKGROUND
};
static UCHAR _downArrow[49] =
{
	BACKGROUND,	BACKGROUND,	BLACK,		BLACK,		BLACK,		BACKGROUND,	BACKGROUND,
	BACKGROUND,	BACKGROUND,	BLACK,		BLACK,		BLACK,		BACKGROUND,	BACKGROUND,
	BACKGROUND,	BACKGROUND,	BLACK,		BLACK,		BLACK,		BACKGROUND,	BACKGROUND,
	BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,
	BACKGROUND,	BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BACKGROUND,
	BACKGROUND,	BACKGROUND,	BLACK,		BLACK,		BLACK,		BACKGROUND,	BACKGROUND,
	BACKGROUND,	BACKGROUND,	BACKGROUND,	BLACK,		BACKGROUND,	BACKGROUND,	BACKGROUND
};
static UCHAR _leftArrow[49] =
{
	BACKGROUND,	BACKGROUND,	BACKGROUND,	BLACK,		BACKGROUND,	BACKGROUND,	BACKGROUND,
	BACKGROUND,	BACKGROUND,	BLACK,		BLACK,		BACKGROUND,	BACKGROUND,	BACKGROUND,
	BACKGROUND,	BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,
	BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,
	BACKGROUND,	BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,
	BACKGROUND,	BACKGROUND,	BLACK,		BLACK,		BACKGROUND,	BACKGROUND,	BACKGROUND,
	BACKGROUND,	BACKGROUND,	BACKGROUND,	BLACK,		BACKGROUND,	BACKGROUND,	BACKGROUND
};
static UCHAR _rightArrow[49] =
{
	BACKGROUND,	BACKGROUND,	BACKGROUND,	BLACK,		BACKGROUND,	BACKGROUND,	BACKGROUND,
	BACKGROUND,	BACKGROUND,	BACKGROUND,	BLACK,		BLACK,		BACKGROUND,	BACKGROUND,
	BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BACKGROUND,
	BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,
	BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BACKGROUND,
	BACKGROUND,	BACKGROUND,	BACKGROUND,	BLACK,		BLACK,		BACKGROUND,	BACKGROUND,
	BACKGROUND,	BACKGROUND,	BACKGROUND,	BLACK,		BACKGROUND,	BACKGROUND,	BACKGROUND
};

UIW_SCROLL_BUTTON::UIW_SCROLL_BUTTON(BUTTON_TYPE _btType) :
	UIW_BUTTON(0, 0, 0, NULLP(char), BTF_NO_FLAGS | BTF_STATIC_BITMAPARRAY,
		WOF_BORDER | WOF_JUSTIFY_CENTER | WOF_NON_FIELD_REGION),
	btType(_btType)
{
	depth = 1;
	if (FlagSet(btType, LTBUTTON | RBBUTTON))
		woFlags |= WOF_SUPPORT_OBJECT;
}

UIW_SCROLL_BUTTON::~UIW_SCROLL_BUTTON(void)
{
	text = NULL;
	bitmapArray = NULL;
}

EVENT_TYPE UIW_SCROLL_BUTTON::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_SCROLL_BAR);
	switch (ccode)
	{
	case S_INITIALIZE:
		UI_WINDOW_OBJECT::Event(event);
		if (display->isText)
		{
			// Null out the graphics information.
			bitmapArray = NULL;
			bitmapWidth = bitmapHeight = 0;
			// Set the text information.
			woFlags |= WOF_NON_FIELD_REGION;
			if (btType == VTOP_BUTTON)
				text = _iSBUpArrow;
			else if (btType == VBOTTOM_BUTTON)
				text = _iSBDownArrow;
			else if (btType == HLEFT_BUTTON)
				text = _iSBLeftArrow;
			else if (btType == HRIGHT_BUTTON)
				text = _iSBRightArrow;
			else
			{
				text = _iSBMiddleArrow;
				woFlags &= ~WOF_NON_FIELD_REGION;
			}
		}
		else
		{
			// Null out the text information.
			text = NULL;
			// Set the graphics information.
			woFlags |= WOF_NON_FIELD_REGION;
			if (btType == VTOP_BUTTON)
				bitmapArray = _upArrow;
			else if (btType == VBOTTOM_BUTTON)
				bitmapArray = _downArrow;
			else if (btType == HLEFT_BUTTON)
				bitmapArray = _leftArrow;
			else if (btType == HRIGHT_BUTTON)
				bitmapArray = _rightArrow;
			else
				woFlags &= ~WOF_NON_FIELD_REGION;
			if (bitmapArray)
				bitmapWidth = bitmapHeight = 7;
		}
		break;

	case S_SIZE:
	case S_CREATE:
		{
		int width, height, clipWidth, clipHeight;
		UI_WINDOW_OBJECT::Event(event);
		SBF_FLAGS sbFlags;
		parent->Information(GET_FLAGS, &sbFlags, ID_SCROLL_BAR);
//		clipWidth = clip.right - clip.left;
//		clipHeight = clip.bottom - clip.top;
		clipWidth = parent->true.right - parent->true.left;		// Bug.A737
		clipHeight = parent->true.bottom - parent->true.top;
		if (!display->isText)
		{
			clipWidth -= 2;
			clipHeight -= 2;
		}
		if (FlagSet(sbFlags, SBF_VERTICAL))
			width = height = clipWidth;
		else
			width = height = clipHeight;
		if (btType == VTOP_BUTTON)
		{
			// Resize if the region doesn't fit all the scroll buttons.
			if (display->isText && parent->true.Height() < 2)
				true.top--;
			else if (!display->isText && clipHeight / 3 < width)
				width = clipHeight / 3;
			true.bottom = true.top + width;
		}
		else if (btType == VBOTTOM_BUTTON)
		{
			// Resize if the region doesn't fit all the scroll buttons.
			if (display->isText && parent->true.Height() < 2)
				true.top++;
			else if (!display->isText && clipHeight / 2 < width)
				width = clipHeight / 2;
			true.top = true.bottom - width;
		}
		else if (btType == HLEFT_BUTTON)
		{
			// Resize if the region doesn't fit all the scroll buttons.
			if (display->isText && parent->true.Width() < 2)
				true.left--;
			else if (!display->isText && clipWidth / 3 < height)
				height = clipWidth / 3;
			true.right = true.left + height;
		}
		else if (btType == HRIGHT_BUTTON)
		{
			// Resize if the region doesn't fit all the scroll buttons.
			if (display->isText && parent->true.Width() < 2)
				true.right++;
			else if (!display->isText && clipWidth / 2 < height)
				height = clipWidth / 2;
			true.left = true.right - height;
		}
		else if (FlagSet(btType, MBUTTON))
		{
			true.left = true.right = clip.left;
			true.top = true.bottom = clip.top;
			if (display->isText && FlagSet(sbFlags, SBF_VERTICAL) && parent->true.Height() == 2)
				true.left++;
			else if (display->isText && FlagSet(sbFlags, SBF_HORIZONTAL) && parent->true.Width() == 2)
				true.top++;
			else if (!display->isText && FlagSet(sbFlags, SBF_VERTICAL) && clipHeight < height)
				height = clipHeight;
			else if (!display->isText && FlagSet(sbFlags, SBF_HORIZONTAL) && clipWidth < width)
				width = clipWidth;
			if (!display->isText)
			{
				true.right += width;
				true.bottom += height;
			}
		}
		if (!display->isText && FlagSet(parent->woFlags, WOF_BORDER))
			++true;
		}
		break;

	default:
		ccode = UIW_BUTTON::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

