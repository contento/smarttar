//	Zinc Interface Library - W_BUTTON.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define OEMRESOURCE				// Windows button messages and flags.
#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif
const int BUTTON_OFFSET = 4;

// ----- UIW_BUTTON ---------------------------------------------------------

#if defined(WIN32)
static WNDPROC _buttonCallback = NULL;
#else
static int _buttonOffset = -1;
static FARPROC _buttonCallback = (FARPROC)DefWindowProc;
long FAR PASCAL _export ButtonJumpProcedure(HWND hWnd, WORD wMsg, WORD wParam, LONG lParam)
{
	UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)GetWindowLong(hWnd, _buttonOffset);
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}
static FARPROC _buttonJumpInstance = (FARPROC)ButtonJumpProcedure;
#endif

EVENT_TYPE UIW_BUTTON::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
	SCREENID screenID = this->screenID;
	UI_DISPLAY *display = this->display;
#endif
	const int BUTTON_OFFSET = 4;
	UI_REGION region = true;
	int listItem = parent->Inherited(ID_LIST);

	// Virtualize the display.
	display->VirtualGet(screenID, region);
 	if (FlagSet(btFlags, BTF_NO_3D) && !FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON)) // BUG.1277
		lastPalette = LogicalPalette(ccode, listItem ? ID_LIST : ID_WINDOW);
	else if (!FlagSet(btFlags, BTF_NO_3D | BTF_CHECK_BOX | BTF_RADIO_BUTTON))
		lastPalette = LogicalPalette(ccode);
	else
		lastPalette = LogicalPalette(ccode, listItem ? ID_LIST : searchID);

	// Draw the object shadow and fill it's region.
	if (FlagSet(woFlags, WOF_BORDER) || (!listItem && FlagSet(woStatus, WOS_CURRENT)))
		DrawBorder(screenID, region, FALSE, ccode);
	if (!FlagSet(btFlags, BTF_NO_3D))
		DrawShadow(screenID, region, FlagSet(btStatus, BTS_DEPRESSED) ? -depth : depth, FALSE, ccode);
	display->Rectangle(screenID, region, lastPalette, 0, TRUE);
	if (depth && FlagSet(btStatus, BTS_DEPRESSED))
	{
		region.left += depth;
		region.top++;
	}

	// Compute the draw region.
	region.left += BUTTON_OFFSET;
	if (listItem && parent->Inherited(ID_VT_LIST))
		region.left += relative.left;
	if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT | WOF_JUSTIFY_CENTER))
		region.right -= BUTTON_OFFSET;
	int fieldWidth = region.Width();
	int fieldHeight = region.Height();
	int left = region.left;
	if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
		left = region.right - bitmapWidth + 1;
	else if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
		left = region.left + (fieldWidth - bitmapWidth) / 2;
	int top = (FlagSet(woFlags, WOF_JUSTIFY_CENTER) && text) ?
		region.top + 2 : region.top + (fieldHeight - bitmapHeight) / 2;

	// Draw the bitmap.
	if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON))
	{
		int xOffset = 0, yOffset = 0;
		if (FlagSet(woStatus, WOS_SELECTED))
			xOffset = bitmapWidth;
		if (FlagSet(btFlags, BTF_RADIO_BUTTON))
			yOffset = bitmapHeight;
		HDC hDC = UI_MSWINDOWS_DISPLAY::hDC;
		HDC hMemDC = CreateCompatibleDC(hDC);
		COLORREF oldForeground, oldBackground;
		if (lastPalette)
		{
			oldForeground = SetTextColor(hDC, display->MapColor(lastPalette, TRUE));
			oldBackground = SetBkColor(hDC, display->MapColor(lastPalette, FALSE));
		}
		SelectObject(hMemDC, colorBitmap);
		BitBlt(UI_MSWINDOWS_DISPLAY::hDC, left, top, bitmapWidth, bitmapHeight,
			hMemDC, xOffset, yOffset, SRCCOPY);
		if (lastPalette)
		{
			SetTextColor(hDC, oldForeground);
			SetBkColor(hDC, oldBackground);
		}
		DeleteDC(hMemDC);
	}
	else if (bitmapArray || colorBitmap)
		display->Bitmap(screenID, left, top, bitmapWidth, bitmapHeight,
			bitmapArray, NULL, NULL, &colorBitmap, &monoBitmap);

	// Draw the text.
	if (text)
	{
		WOF_FLAGS flags = woFlags;
		if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
			region.top = region.bottom - display->cellHeight + 4;
		else if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
		{
//			region.right -= bitmapWidth - BUTTON_OFFSET;					// BUG.1579.B
//			woFlags &= ~(WOF_JUSTIFY_RIGHT);
			if (bitmapArray || colorBitmap)
			{
				region.right -= bitmapWidth - BUTTON_OFFSET;
				woFlags &= ~(WOF_JUSTIFY_RIGHT);
			}
			else
			{
				region.right -= BUTTON_OFFSET,
				region.left =
					region.right - display->TextWidth(text, screenID, font);
			}
		}
		else
			region.left += bitmapWidth + BUTTON_OFFSET;
		DrawText(screenID, region, text, lastPalette, FALSE, ccode);
		woFlags = flags;
		region += 2;

		// Draw the focus rectangle.
		if (FlagSet(woStatus, WOS_CURRENT))
			display->Rectangle(screenID, listItem ? true : region, lastPalette, 1, FALSE, TRUE);
	}
	else if (listItem && FlagSet(woStatus, WOS_CURRENT))
		display->Rectangle(screenID, true, lastPalette, 1, FALSE, TRUE);

	// Restore the display and return the control code.
	display->VirtualPut(screenID);
	return (TRUE);
}

EVENT_TYPE UIW_BUTTON::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_BUTTON);
	switch (ccode)
	{
	case S_INITIALIZE:
	case S_CREATE:
		UI_WINDOW_OBJECT::Event(event);
		if (FlagSet(btFlags, BTF_AUTO_SIZE))
		{
			int offset = 2;
			if (FlagSet(woFlags, WOF_BORDER))
				offset += 2;
			if (!FlagSet(btFlags, BTF_NO_3D))
				offset += 6;
			int height = 0;
			if (colorBitmap || bitmapHeight)
			{
				height = bitmapHeight + offset;
				if (text && FlagSet(woFlags, WOF_JUSTIFY_CENTER))
					height += display->cellHeight;
			}
			height = Max(height, display->cellHeight * 10 / 9);
			true.top = relative.top = relative.bottom - height;
		}
		if (ccode == S_INITIALIZE)
			break;

		if ((colorBitmap || FlagSet(btFlags, BTF_NO_3D)) && 				// BUG.1277
			!FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON))					
			dwStyle |= BS_OWNERDRAW;
#if defined(WIN32)
		RegisterObject("UIW_BUTTON", "BUTTON", &_buttonCallback, text);
#else
		RegisterObject("UIW_BUTTON", "BUTTON", &_buttonOffset,
			&_buttonJumpInstance, &_buttonCallback, text ? text : "");		// BUG.7185
#endif
		if (screenID != parent->screenID &&
			FlagSet(woStatus, WOS_SELECTED) &&
			FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON))
			SendMessage(screenID, BM_SETCHECK, 1, 0);

		// Convert the bitmap array to a handle if possible.
		if (bitmapArray && !colorBitmap)
		{
			display->BitmapArrayToHandle(screenID, bitmapWidth, bitmapHeight,
				bitmapArray, NULL, &colorBitmap, &monoBitmap);
			if (colorBitmap)
			{
				delete bitmapArray;
				bitmapArray = NULL;
			}
		}
		break;

	case S_REGISTER_OBJECT:
#if defined(WIN32)
		RegisterObject("UIW_BUTTON", "BUTTON", &_buttonCallback, text);
#else
		RegisterObject("UIW_BUTTON", "BUTTON", &_buttonOffset,
			&_buttonJumpInstance, &_buttonCallback, text ? text : "");		// BUG.7185
#endif
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

	case L_END_SELECT:
	case L_SELECT:
//		ccode = UserFunction(UI_EVENT(ccode), L_SELECT);					// BUG.1584
		ccode = UI_WINDOW_OBJECT::Event(UI_EVENT(ccode));
		if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON) &&
			screenID && screenID != parent->screenID)
			SendMessage(screenID, BM_SETCHECK, FlagSet(woStatus, WOS_SELECTED), 0);
//		parent->Information(CHECK_SELECTION, NULL);							// BUG.General
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
		UI_WINDOW_OBJECT::Event(event);
		if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON) ||
			FlagSet(woStatus, WOS_OWNERDRAW) || screenID == parent->screenID)
			break;
		dwStyle &= 0xFFFFFFF0L;
		if (ccode == S_CURRENT)
			dwStyle |= BS_DEFPUSHBUTTON;
		else
			dwStyle |= BS_PUSHBUTTON;
		SendMessage(screenID, BM_SETSTYLE, LOWORD(dwStyle), 1);
		break;

	default:
		WORD message = event.message.message;
		if (event.type != E_MSWINDOWS)
			ccode = UI_WINDOW_OBJECT::Event(event);
		else if (message == BM_SETSTATE)
		{
			if (event.message.wParam)
				btStatus |= BTS_DEPRESSED;
			else
				btStatus &= ~BTS_DEPRESSED;
			ccode = UI_WINDOW_OBJECT::Event(event);
		}
		else if (message == WM_SYSCHAR && event.message.wParam == hotKey)
			ccode = UserFunction(UI_EVENT(E_KEY), L_SELECT);
		else if (message == WM_LBUTTONDOWN)
		{
			if (FlagSet(btFlags, BTF_DOWN_CLICK))
				ccode = UserFunction(UI_EVENT(L_BEGIN_SELECT), L_SELECT);
			else if (FlagSet(btFlags, BTF_REPEAT))
			{
				UI_EVENT tEvent = event;
				UI_TIME currentTime, lastTime;
				lastTime.Import();
				while (message != WM_LBUTTONUP)
				{
					ccode = UI_WINDOW_OBJECT::Event(tEvent);
					currentTime.Import();
					long elapsedTime = currentTime - lastTime;				// BUG.1228
					if (eventManager->Get(tEvent, Q_NO_BLOCK) != -2)
					{
						// Send all user messages to the window manager.
						if (tEvent.type > 9999)
							windowManager->Event(tEvent);
						if (tEvent.type == E_MSWINDOWS)
						    message = tEvent.message.message;
					}
					else if (FlagSet(btStatus, BTS_DEPRESSED) &&
						Abs(elapsedTime) > repeatRate)
					{
						UserFunction(UI_EVENT(L_CONTINUE_SELECT), L_SELECT);
						lastTime = currentTime;
					}
				}
				UI_WINDOW_OBJECT::Event(tEvent);
				break;
			}
			else
				ccode = UI_WINDOW_OBJECT::Event(event);
		}
		else if (message == WM_LBUTTONUP && !FlagSet(woStatus, WOS_EDIT_MODE))
		{
			if (FlagSet(btStatus, BTS_DEPRESSED) && !FlagSet(btFlags, BTF_DOWN_CLICK))
			{
				ccode = UI_WINDOW_OBJECT::Event(event);
				UIW_BUTTON::Event(L_END_SELECT);
			}
			else
				ccode = UI_WINDOW_OBJECT::Event(event);
		}
		else if (message == WM_LBUTTONDBLCLK && !FlagSet(woStatus, WOS_EDIT_MODE) &&
			FlagSet(btFlags, BTF_DOUBLE_CLICK) && userFunction)
		{
			ccode = UI_WINDOW_OBJECT::Event(event);
			UI_EVENT uEvent;
			uEvent = event;
			ccode = (*userFunction)(this, uEvent, L_DOUBLE_CLICK);
		}
		else
			ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}


