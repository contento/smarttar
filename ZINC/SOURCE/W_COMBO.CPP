//	Zinc Interface Library - W_COMBO.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_COMBO_BOX ------------------------------------------------------

#if defined(WIN32)
static WNDPROC _comboboxCallback = NULL;
#else
static int _comboboxOffset = -1;
static FARPROC _comboboxCallback = (FARPROC)DefWindowProc;
long FAR PASCAL _export ComboboxJumpProcedure(HWND hWnd, WORD wMsg, WORD wParam, LONG lParam)
{
	UIW_COMBO_BOX *object = (UIW_COMBO_BOX *)GetWindowLong(hWnd, _comboboxOffset);
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}
static FARPROC _comboboxJumpInstance = (FARPROC)ComboboxJumpProcedure;
#endif

EVENT_TYPE UIW_COMBO_BOX::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_COMBO_BOX);
	switch (ccode)
	{
	case S_INITIALIZE:
		UIW_WINDOW::Event(event);
		list.Event(event);
		break;

	case S_CREATE:
		{
		UI_WINDOW_OBJECT::Event(event);
		dwStyle |= (list.dwStyle & (WS_VSCROLL | WS_HSCROLL));

		UI_REGION region = true;											// BUG.1493
		true.bottom += list.relative.bottom - list.relative.top;
#if defined(WIN32)
		RegisterObject("UIW_COMBO_BOX", "COMBOBOX", &_comboboxCallback, NULL);
#else
		RegisterObject("UIW_COMBO_BOX", "COMBOBOX", &_comboboxOffset,
			&_comboboxJumpInstance, &_comboboxCallback, NULL);
#endif
		true = region;														// BUG.1493
		list.screenID = screenID;
		for (object = list.First(); object; object = object->Next())
			object->Event(event);
		if (text)
		{
			SendMessage(screenID, WM_SETTEXT, 0, (LONG)text);
			delete text;
			text = NULL;
		}
		}
		break;

	case S_REGISTER_OBJECT:
		{
		UI_REGION region = true;											// BUG.1493
		true.bottom += list.relative.bottom - list.relative.top;
#if defined(WIN32)
		RegisterObject("UIW_COMBO_BOX", "COMBOBOX", &_comboboxCallback, text);
#else
		RegisterObject("UIW_COMBO_BOX", "COMBOBOX", &_comboboxOffset,
			&_comboboxJumpInstance, &_comboboxCallback, text);
#endif
		true = region;														// BUG.1493
		if (text)
			delete text;
		text = NULL;
		}
		break;

	case S_CURRENT:
	case S_REDISPLAY:
		ccode = UI_WINDOW_OBJECT::Event(event);
//		if (screenID && current &&											// BUG.1380
//			SendMessage(screenID, CB_GETCURSEL, 0, 0) != list.Index(current))
//			SendMessage(screenID, CB_SETCURSEL, list.Index(current), 0);
		if (screenID && list.current &&											// BUG.1258
			SendMessage(screenID, CB_GETCURSEL, 0, 0) != list.Index(list.current))
			SendMessage(screenID, CB_SETCURSEL, list.Index(list.current), 0);
		break;

	case S_NON_CURRENT:
		ccode = UI_WINDOW_OBJECT::Event(event);
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
//			SendMessage(screenID, CB_SETCURSEL, list.Index(current), 0);	// BUG.1517
			SendMessage(screenID, CB_SETCURSEL, list.Index(list.current), 0);
		}
		break;		// Stop at the combo-box level.

	case S_SUBTRACT_OBJECT:
		object = (UI_WINDOW_OBJECT *)event.data;
		if (list.Index(object) != -1)
		{
			if (screenID)
				SendMessage(screenID, CB_DELETESTRING, list.Index(object), 0);
			list.Subtract(object);
		}
		break;

	case S_DEINITIALIZE:
		list.Event(event);
		break;

	case L_PREVIOUS:
	case L_NEXT:
		ccode = S_UNKNOWN;
		break;

	default:
		{
		WORD message = event.message.message;
#if defined(WIN32)
		WORD command = HIWORD(event.message.wParam);
		WORD wID = LOWORD(event.message.wParam);							// BUG.1259
#else
		WORD command = HIWORD(event.message.lParam);
		WORD wID = event.message.wParam;									// BUG.1259
#endif
		if ((ccode == L_SELECT && !FlagSet(wnFlags, WNF_BITMAP_CHILDREN) &&
			 !FlagSet(woFlags, WOF_VIEW_ONLY)) ||
			(message == WM_COMMAND && command == CBN_SELCHANGE && wID != numberID))		//BUG.1259
		{
			ccode = UI_WINDOW_OBJECT::Event(event);
			long selection = SendMessage(screenID, CB_GETCURSEL, 0, 0);
			object = (selection >= 0) ? (UI_WINDOW_OBJECT *)list.UI_LIST::Get(LOWORD(selection)) : NULL;
			if (object)
			{
				list.Add(object);
//				object->Event(UI_EVENT(L_SELECT, 0));						// BUG.1259
			}
		}
		else if (event.type != E_MSWINDOWS)
			ccode = UIW_WINDOW::Event(event);
		else if (message == WM_COMPAREITEM)
			ccode = list.Event(event);
		else if (message == WM_MEASUREITEM)
		{
			MEASUREITEMSTRUCT *measure = (MEASUREITEMSTRUCT *)event.message.lParam;
			if (measure->itemID == (unsigned)-1)
			{
				measure->itemWidth = relative.right - relative.left - 2;
				measure->itemHeight = display->cellHeight - display->preSpace - display->postSpace;
				for (object = list.First(); object; object = object->Next())
					if (measure->itemHeight < object->relative.Height())
						measure->itemHeight = object->relative.Height();
			}
			else
			{
				object = (UI_WINDOW_OBJECT *)measure->itemData;
				object->Event(event);
			}
		}
		else if (message == WM_COMMAND && !FlagSet(woStatus, WOS_CURRENT) &&
			HIWORD(event.message.lParam) == WM_KEYDOWN)
		{
			woStatus |= WOS_WINDOWS_ACTION;
			UI_WINDOW_OBJECT::Event(UI_EVENT(S_ADD_OBJECT, 0));
			woStatus &= ~WOS_WINDOWS_ACTION;
		}
		else
			ccode = UI_WINDOW_OBJECT::Event(event);
		}
		break;
	}

	// Return the control code.
	return (ccode);
}


