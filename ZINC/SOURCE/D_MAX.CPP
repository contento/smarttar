//	Zinc Interface Library - D_MAX.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_MAXIMIZE_BUTTON ------------------------------------------------

static UCHAR _gNormal[37] =
{
	7, 5, 
	BACKGROUND,	BACKGROUND,	BACKGROUND,	BLACK,		BACKGROUND,	BACKGROUND,	BACKGROUND,
	BACKGROUND,	BACKGROUND,	BLACK,		BLACK,		BLACK,		BACKGROUND,	BACKGROUND,
	BACKGROUND,	BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BACKGROUND,
	BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,
	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND
};

static UCHAR _gMaximized[79] =
{
	7, 11, 
	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,
	BACKGROUND,	BACKGROUND,	BACKGROUND,	BLACK,		BACKGROUND,	BACKGROUND,	BACKGROUND,
	BACKGROUND,	BACKGROUND,	BLACK,		BLACK,		BLACK,		BACKGROUND,	BACKGROUND,
	BACKGROUND,	BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BACKGROUND,
	BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,
	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,
	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,	BACKGROUND,
	BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BLACK,
	BACKGROUND,	BLACK,		BLACK,		BLACK,		BLACK,		BLACK,		BACKGROUND,
	BACKGROUND,	BACKGROUND,	BLACK,		BLACK,		BLACK,		BACKGROUND,	BACKGROUND,
	BACKGROUND,	BACKGROUND,	BACKGROUND,	BLACK,		BACKGROUND,	BACKGROUND,	BACKGROUND
};

EVENT_TYPE UIW_MAXIMIZE_BUTTON::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_MAXIMIZE_BUTTON);
	switch (ccode)
	{
	case S_SIZE:
	case S_CREATE:
		if (FlagSet(parent->woStatus, WOS_MAXIMIZED))
		{
			woStatus |= WOS_SELECTED;
			value = L_RESTORE;
		}
		else
		{
			woStatus &= ~WOS_SELECTED;
			value = L_MAXIMIZE;
		}
		UI_WINDOW_OBJECT::Event(event);
		if (display->isText)
		{
			text = FlagSet(parent->woStatus, WOS_MAXIMIZED) ? _iWMaximized : _iWNormalMax;
			true.left = true.right - 2;
			true.bottom = true.top;
			bitmapArray = NULL;
			bitmapWidth = bitmapHeight = 0;
		}
		else
		{
			text = NULL;
			UCHAR *bitmap = FlagSet(parent->woStatus, WOS_MAXIMIZED) ? _gMaximized : _gNormal;
			bitmapWidth = bitmap[0];
			bitmapHeight = bitmap[1];
			bitmapArray = &bitmap[2];
			true.bottom = --true.top + (display->cellHeight - display->preSpace - display->postSpace);
			true.left = ++true.right - (display->cellHeight - display->preSpace - display->postSpace);
			if (colorBitmap)
				delete colorBitmap;
			if (monoBitmap)
				delete monoBitmap;
			colorBitmap = monoBitmap = NULL;
		}
		break;

	case S_DISPLAY_INACTIVE:
	case S_DISPLAY_ACTIVE:
		if (display->isText)
		{
			lastPalette = LogicalPalette(ccode);
			UIW_BUTTON::DrawItem(event, ccode);
			woStatus &= ~WOS_REDISPLAY;
		}
		else
		{
			UI_WINDOW_OBJECT::Event(event);	// Get the palette and status.
			int i, j;
			UCHAR *bitmap = bitmapArray;
			COLOR foregroundColor = display->MapColor(lastPalette, TRUE);
			for (i = 0; i < bitmapHeight; i++)
				for (j = 0; j < bitmapWidth; j++, bitmap++)
					if (*bitmap != BACKGROUND)
						*bitmap = foregroundColor;
			UIW_BUTTON::Event(event);
		}
		break;

	case L_BEGIN_SELECT:
		UIW_BUTTON::Event(event);
		while (ccode != L_END_SELECT)
		{
			UI_EVENT sEvent;
			eventManager->Get(sEvent);
			ccode = LogicalEvent(sEvent, ID_MAXIMIZE_BUTTON);
			// Send all user messages to the window manager.
			if (sEvent.type > 9999)
				windowManager->Event(sEvent);
			else
				UIW_BUTTON::Event(sEvent);
		}
		break;

	default:
		ccode = UIW_BUTTON::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}


