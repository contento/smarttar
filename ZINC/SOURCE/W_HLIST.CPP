//	Zinc Interface Library - W_HLIST.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_HZ_LIST --------------------------------------------------------

#if defined(WIN32)
static WNDPROC _hlistCallback = NULL;
#else
static int _hlistOffset = -1;
static FARPROC _hlistCallback = (FARPROC)DefWindowProc;
long FAR PASCAL _export HListJumpProcedure(HWND hWnd, WORD wMsg, WORD wParam, LONG lParam)
{
	UIW_HZ_LIST *object = (UIW_HZ_LIST *)GetWindowLong(hWnd, _hlistOffset);
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}
static FARPROC _hlistJumpInstance = (FARPROC)HListJumpProcedure;
#endif

EVENT_TYPE UIW_HZ_LIST::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_HZ_LIST);
	switch (ccode)
	{
	case S_INITIALIZE:
		UIW_WINDOW::Event(event);
		break;

	case S_CREATE:
		UI_WINDOW_OBJECT::Event(event);
#if defined(WIN32)
		RegisterObject("UIW_HZ_LIST", "LISTBOX", &_hlistCallback);
#else
		RegisterObject("UIW_HZ_LIST", "LISTBOX", &_hlistOffset,
			&_hlistJumpInstance, &_hlistCallback);
#endif
		SendMessage(screenID, LB_SETCOLUMNWIDTH, cellWidth * display->cellWidth, 0);
		SendMessage(screenID, WM_SETREDRAW, FALSE, 0);
		for (object = First(); object; object = object->Next())
			object->Event(event);
		SendMessage(screenID, WM_SETREDRAW, TRUE, 0);
		break;

	case S_REGISTER_OBJECT:
#if defined(WIN32)
		RegisterObject("UIW_HZ_LIST", "LISTBOX", &_hlistCallback);
#else
		RegisterObject("UIW_HZ_LIST", "LISTBOX", &_hlistOffset,
			&_hlistJumpInstance, &_hlistCallback);
#endif
		break;

	case S_CURRENT:
	case S_REDISPLAY:
//		ccode = UI_WINDOW_OBJECT::Event(event);								// BUG.1441
		ccode = UIW_WINDOW::Event(event);
		if (screenID && current &&											// BUG.1380
			!FlagSet(wnFlags, WNF_SELECT_MULTIPLE) &&
			SendMessage(screenID, LB_GETCURSEL, 0, 0) != Index(current))
			SendMessage(screenID, LB_SETCURSEL, Index(current), 0);
		break;

	case S_NON_CURRENT:
//		ccode = UI_WINDOW_OBJECT::Event(event);								// BUG.1441
		ccode = UIW_WINDOW::Event(event);
		break;

	case S_ADD_OBJECT:
		object = (UI_WINDOW_OBJECT *)event.data;
		if (UI_LIST::Index(object) == -1)
			UIW_WINDOW::Event(event);
		else if (object != current)
		{
			if (current)
				Current()->woStatus &= ~WOS_CURRENT;
			current = object;
			Current()->woStatus |= WOS_CURRENT;
			if (!FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
				SendMessage(screenID, LB_SETCURSEL, Index(current), 0);
		}
		break;

	case S_SUBTRACT_OBJECT:
		object = (UI_WINDOW_OBJECT *)event.data;
		if (UI_LIST::Index(object) != -1)
		{
			if (screenID)
				SendMessage(screenID, LB_DELETESTRING, UI_LIST::Index(object), 0);
			UI_LIST::Subtract(object);
		}
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
#else
		WORD command = HIWORD(event.message.lParam);
#endif
																			// BUG.
		if (ccode == L_SELECT || (event.type == E_MSWINDOWS && message == WM_LBUTTONUP))
		{
			if (Current())
			{
				if (ccode != L_SELECT)
				{
					UI_WINDOW_OBJECT::Event(event);
					Current()->Event(UI_EVENT(L_END_SELECT, 0));
				}
				else
					Current()->Event(event);
			}
		}
		else if (event.type != E_MSWINDOWS)
			ccode = UIW_WINDOW::Event(event);
		else if (message == WM_COMPAREITEM)
			ccode = UIW_WINDOW::Event(event);
		else if (message == WM_MEASUREITEM)
		{
			MEASUREITEMSTRUCT *measure = (MEASUREITEMSTRUCT *)event.message.lParam;
			measure->itemWidth = cellWidth * display->cellWidth;
//			measure->itemHeight = cellHeight * (display->cellHeight - display->preSpace - display->postSpace - 2);		//BUG.????
			measure->itemHeight = cellHeight * (display->cellHeight - display->preSpace - display->postSpace);
		}
		else if (message == WM_LBUTTONDBLCLK && current && Current()->userFunction)
		{
			ccode = UI_WINDOW_OBJECT::Event(event);
			UI_EVENT uEvent = event;
			ccode = (*Current()->userFunction)(Current(), uEvent, L_DOUBLE_CLICK);
		}
		else if (message == WM_LBUTTONDOWN &&								// BUG.????
			(FlagSet(wnFlags, WNF_SELECT_MULTIPLE) || !FlagSet(wnFlags, WNF_AUTO_SELECT)))
		{
			ccode = UI_WINDOW_OBJECT::Event(event);
			long selection = SendMessage(screenID, LB_GETCARETINDEX, 0, 0);
			SendMessage(screenID, LB_SETSEL, 0, LOWORD(selection));
		}
		else if (message == WM_COMMAND && command == LBN_SELCHANGE)
		{
			long selection = SendMessage(screenID, LB_GETCURSEL, 0, 0);
			UIW_WINDOW::Add((selection >= 0) ? (UI_WINDOW_OBJECT *)UI_LIST::Get(LOWORD(selection)) : NULL);
		}
		else
			ccode = UI_WINDOW_OBJECT::Event(event);
		}
		break;
	}

	// Return the control code.
	return (ccode);
}


