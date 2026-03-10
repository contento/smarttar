//	Zinc Interface Library - W_ICON.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_ICON -----------------------------------------------------------

#if defined(WIN32)
static WNDPROC _iconCallback = NULL;
#else
static int _iconOffset = -1;
static FARPROC _iconCallback = (FARPROC)DefWindowProc;
long FAR PASCAL _export IconJumpProcedure(HWND hWnd, WORD wMsg,
	WORD wParam, LONG lParam)
{
	UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)GetWindowLong(hWnd, _iconOffset);
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}
static FARPROC _iconJumpInstance = (FARPROC)IconJumpProcedure;
#endif

EVENT_TYPE UIW_ICON::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
	SCREENID screenID = this->screenID;
	UI_DISPLAY *display = this->display;
#endif

	int listItem = parent->Inherited(ID_LIST);
	int iconOffset = listItem ? 4 : 0;

	// Virtualize the display.
	display->VirtualGet(screenID, true);
	if (listItem)
		lastPalette = LogicalPalette(ccode, ID_LIST);

	// Draw the object shadow and fill it's region.
	UI_REGION region = true;
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);
	if (listItem)
	{
		display->Rectangle(screenID, region, lastPalette, 0, TRUE);
		if (FlagSet(woStatus, WOS_CURRENT))
			display->Rectangle(screenID, true, lastPalette, 1, FALSE, TRUE);
	}

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
	if (icon && listItem)
		DrawIcon(UI_MSWINDOWS_DISPLAY::hDC, left, top, icon);
	else if (icon)
		DrawIcon(UI_MSWINDOWS_DISPLAY::hDC, left - true.left, top - true.top, icon);
	else if (iconArray)
		display->Bitmap(screenID, left, top, iconWidth, iconHeight, iconArray);

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
		if (listItem)
			DrawText(screenID, region, title, lastPalette, FALSE, ccode);
		else if (FlagSet(woStatus, WOS_CURRENT))
		{
			display->Rectangle(screenID, region, lastPalette, 0, TRUE);
			DrawText(screenID, region, title, lastPalette, FALSE, ccode);
		}
		else
		{
			display->Rectangle(screenID, region, NULL, 0, TRUE);
			DrawText(screenID, region, title, NULL, FALSE, ccode);
		}
		woFlags = flags;
	}

	// Restore the display.
	display->VirtualPut(screenID);

	// Return the control code.
	return (TRUE);
}

EVENT_TYPE UIW_ICON::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_ICON);
	switch (ccode)
	{
	case S_INITIALIZE:
		// Initialize the icon information.
		if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
			icFlags |= ICF_MINIMIZE_OBJECT;
		UI_WINDOW_OBJECT::Event(event);
//		if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT))							// BUG.1060
//			parent->Information(SET_ICON, &icon);
		// Continue to S_SIZE.

	case S_SIZE:
		// Compute the icon region.
		if (ccode == S_SIZE)
			UI_WINDOW_OBJECT::Event(event);
		relative.right = relative.left + iconWidth + 2;
		relative.bottom = relative.top + iconHeight + 2;
		if (title && FlagSet(woFlags, WOF_JUSTIFY_CENTER))
		{
			relative.bottom += (display->cellHeight - display->preSpace - display->postSpace);
			if (relative.Width() < display->TextWidth(title, screenID, font))
				relative.right = relative.left + display->TextWidth(title, screenID, font);
		}
		true = relative;
		break;

	case S_CREATE:
		// Convert the icon array to a handle if possible.
		if (iconArray && !icon)
		{
			display->IconArrayToHandle(screenID, iconWidth, iconHeight,
				iconArray, NULL, &icon);
			if (icon && !FlagSet(icFlags, ICF_STATIC_ICONARRAY))
			{
				delete iconArray;
				iconArray = NULL;
			}
		}

		// Attach it to parent as a minimize object if it is a support object.
		if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT))							// BUG.1060
		{
			parent->Information(SET_ICON, &icon);
			break;
		}

		// else fall through to S_REGISTER_OBJECT

	case S_REGISTER_OBJECT:
#if defined(WIN32)
		RegisterObject("UIW_ICON", "BUTTON", &_iconCallback, title);
#else
		RegisterObject("UIW_ICON", "BUTTON", &_iconOffset,
			&_iconJumpInstance, &_iconCallback, title);
#endif
		break;

	case L_END_SELECT:
	case L_SELECT:
		ccode = UserFunction(UI_EVENT(ccode), L_SELECT);
		parent->Information(CHECK_SELECTION, NULL);
		break;

	default:
		{
		WORD message = event.message.message;
		if (event.type != E_MSWINDOWS)
			ccode = UI_WINDOW_OBJECT::Event(event);
		else if (message == WM_PAINT)
			DrawItem(event, ccode);
		else if (message == BM_SETSTATE)
		{
			if (event.message.wParam)
				icStatus |= BTS_DEPRESSED;
			else
				icStatus &= ~BTS_DEPRESSED;
//			ccode = UI_WINDOW_OBJECT::Event(event);							// BUG.1567
		}
		else if (message == WM_SYSCHAR && event.message.wParam == hotKey)
			ccode = UserFunction(UI_EVENT(E_KEY), L_SELECT);
		else if (message == WM_LBUTTONUP && !FlagSet(woStatus, WOS_EDIT_MODE))
		{
			if (FlagSet(icStatus, BTS_DEPRESSED))
			{
				ccode = UI_WINDOW_OBJECT::Event(event);
				UIW_ICON::Event(L_END_SELECT);
			}
			else
				ccode = UI_WINDOW_OBJECT::Event(event);
			icStatus = 0;
		}
		else if (message == WM_LBUTTONDBLCLK && !FlagSet(woStatus, WOS_EDIT_MODE) &&
			FlagSet(icFlags, ICF_DOUBLE_CLICK) && userFunction)
		{
			ccode = UI_WINDOW_OBJECT::Event(event);
			UI_EVENT uEvent;
			uEvent = event;
			ccode = (*userFunction)(this, uEvent, L_DOUBLE_CLICK);
		}
		else if (message == WM_SETFOCUS || message == WM_KILLFOCUS)			// BUG.1691
			DrawItem(event, ccode);
		else
			ccode = UI_WINDOW_OBJECT::Event(event);
		}
		break;
	}

	// Return the control code.
	return (ccode);
}


