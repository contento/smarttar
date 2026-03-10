//	Zinc Interface Library - D_COMBO.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

#define CELL_RATIO	3 / 4

// ----- UIW_COMBO_BOX ------------------------------------------------------

static UCHAR _gDownArrow[65] =
{
	7, 9,
	BACKGROUND,	BACKGROUND,	BLACK,		BLACK,		BLACK,		BACKGROUND,	BACKGROUND,
	BACKGROUND,	BACKGROUND,	BLACK,		BLACK,		BLACK,		BACKGROUND,	BACKGROUND,
	BACKGROUND,	BACKGROUND,	BLACK,		BLACK,		BLACK,		BACKGROUND,	BACKGROUND,
	BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,
	BACKGROUND,	BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BACKGROUND,
	BACKGROUND,	BACKGROUND,	BLACK,		BLACK,		BLACK,		BACKGROUND,	BACKGROUND,
	BACKGROUND,	BACKGROUND,	BACKGROUND,	BLACK,		BACKGROUND,	BACKGROUND,	BACKGROUND,
	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,
	BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK
};

EVENT_TYPE UIW_COMBO_BUTTON::Event(const UI_EVENT &event)
{
 	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event);
	switch (ccode)
	{
	case S_INITIALIZE:
		if (display->isText)
		{
			text = _iWDownArrow;
			bitmapArray = NULL;
			bitmapWidth = bitmapHeight = 0;
		}
		else
		{
			text = NULL;
			UCHAR *bitmap = _gDownArrow;
			btFlags |= BTF_STATIC_BITMAPARRAY;
			bitmapWidth = bitmap[0];
			bitmapHeight = bitmap[1];
			bitmapArray = &bitmap[2];
		}
		ccode = UIW_BUTTON::Event(event);
		break;

	case L_BEGIN_SELECT:
		ccode = UI_WINDOW_OBJECT::Event(event);
		if (ccode != S_UNKNOWN)
			eventManager->Put(UI_EVENT(L_SELECT));
		break;

	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		userFlags = FALSE;
		// Continue to default.
	default:
		ccode = UIW_BUTTON::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

EVENT_TYPE UIW_COMBO_STRING::Event(const UI_EVENT &event)
{
 	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event);
	switch (ccode)
	{
	case S_CURRENT:
	case S_NON_CURRENT:
	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		woFlags &= ~(WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT);
		woFlags |= (parent->woFlags & (WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT));
		if (userObject)
		{
			UI_WINDOW_OBJECT::Event(event);
			break;
		}
		// Continue to default.
	default:
		ccode = UIW_STRING::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

EVENT_TYPE UIW_COMBO_BOX::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;
	UI_WINDOW_OBJECT *oldCurrent = list.Current();

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_COMBO_BOX);
	switch (ccode)
	{
	case S_INITIALIZE:
		UIW_WINDOW::Event(event);
		list.Event(event);
		if (FlagSet(woFlags, WOF_UNANSWERED))				// BUG.1281d
			list.current = NULL;
		if (!display->isText && FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
		{
			int maxHeight = display->cellHeight - display->preSpace - display->postSpace;
			for (UI_WINDOW_OBJECT *object = list.First(); object; object = object->Next())
				if (maxHeight <= object->relative.Height())
					maxHeight = object->relative.Height();
			relative.bottom = relative.top + maxHeight;
		}
		else if (!display->isText)
			relative.bottom = relative.top + display->cellHeight - display->preSpace - display->postSpace;
		else
			relative.bottom = relative.top;
		break;

	case S_SIZE:
	case S_CREATE:
		UIW_WINDOW::Event(event);
		button->Event(event);
		button->true = true;
		button->true.left = display->isText ? true.right - 2 :
			true.right - display->cellHeight * CELL_RATIO;
		if (string)
		{
			string->Event(event);
			string->true = true;
			string->true.right = (FlagSet(wnFlags, WNF_BITMAP_CHILDREN) || FlagSet(woFlags, WOF_VIEW_ONLY)) ?
				button->true.left : button->true.left - display->cellWidth;
			if (display->isText && (FlagSet(wnFlags, WNF_BITMAP_CHILDREN) || FlagSet(woFlags, WOF_VIEW_ONLY)))
				string->true.right--;
		}
		list.woStatus |= WOS_INTERNAL_ACTION;
		list.Event(event);
		list.woStatus &= ~WOS_INTERNAL_ACTION;
		if (text)
		{
			string->Information(SET_TEXT, text);
			delete text;
			text = NULL;
		}
		break;

	case S_RESET_DISPLAY:
		UIW_WINDOW::Event(event);
		list.Event(event);
		break;

	case L_PREVIOUS:
	case L_NEXT:
		ccode = S_UNKNOWN;
		break;

	case L_SELECT:
		if ((!FlagSet(wnFlags, WNF_BITMAP_CHILDREN) && !FlagSet(woFlags, WOF_VIEW_ONLY)) &&
			event.type == E_KEY && event.key.value == ' ')
			return (string->Event(event));
		{
		list.RegionConvert(list.relative, parent ? FALSE : TRUE);
		list.wnFlags = wnFlags | WNF_AUTO_SELECT;
		int height = list.relative.bottom - list.relative.top;
		int width = relative.right - relative.left;
		if (!FlagSet(wnFlags, WNF_BITMAP_CHILDREN) && !FlagSet(woFlags, WOF_VIEW_ONLY))
			width -= display->cellWidth;
		list.relative.left = true.right - width;
		list.relative.top = true.bottom;
		if (display->isText)
			list.relative.top++;
		list.relative.right = true.right;
		list.relative.bottom = list.relative.top + height;
		(void *)&(*windowManager + &list);
		}
		break;

	case L_FIRST:
	case L_LAST:
		if (!FlagSet(wnFlags, WNF_BITMAP_CHILDREN) && !FlagSet(woFlags, WOF_VIEW_ONLY))
			return (string->Event(event));
		// Continue to L_UP.
	case L_UP:
	case L_DOWN:
	case L_PGUP:
	case L_PGDN:
		{
		WOS_STATUS _woStatus = list.woStatus;
		list.woStatus |= WOS_INTERNAL_ACTION;
		list.Event(event);
		if (!FlagSet(_woStatus, WOS_INTERNAL_ACTION))
			list.woStatus &= ~WOS_INTERNAL_ACTION;
		// Make sure the user function (if any) is called for the items.
		if (oldCurrent != list.Current())
		{
			oldCurrent->UI_WINDOW_OBJECT::Event(UI_EVENT(S_NON_CURRENT));
			list.Current()->UI_WINDOW_OBJECT::Event(UI_EVENT(S_CURRENT));
			list.Current()->UI_WINDOW_OBJECT::Event(UI_EVENT(L_SELECT));
		}
		}
		break;

	case S_ADD_OBJECT:
		object = (UI_WINDOW_OBJECT *)event.data;
		if (list.Index(object) == -1)
			list.Event(event);
		else if (object != list.current)
		{
			if (list.current)
				list.Current()->woStatus &= ~WOS_CURRENT;
			list.current = object;
			list.Current()->woStatus |= WOS_CURRENT;
		}
		break;		// Stop at the combo-box level.

	case S_SUBTRACT_OBJECT:
		object = (UI_WINDOW_OBJECT *)event.data;
		if (list.Index(object) != -1)
			list.Event(event);
		break;		// Stop at the combo-box level.

	case S_CURRENT:
	case S_NON_CURRENT:
	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		current = string;
		UIW_WINDOW::Event(event);
		if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN) || FlagSet(woFlags, WOF_VIEW_ONLY))
			oldCurrent = NULL;
		break;

#if defined(ZIL_EDIT)
	case L_VIEW:
	case L_BEGIN_SELECT:
	case L_CONTINUE_SELECT:
	case L_END_SELECT:
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UI_WINDOW_OBJECT::Event(event));
		// Continue to default.
#endif
	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Redraw the current combo-item.
	if (oldCurrent != list.Current())
		UIW_COMBO_BOX::Information(RESET_SELECTION, list.Current());

	// Return the control code.
	return (ccode);
}


