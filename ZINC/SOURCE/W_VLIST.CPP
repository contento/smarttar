//	Zinc Interface Library - W_VLIST.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_VT_LIST --------------------------------------------------------

#if defined(WIN32)
static WNDPROC _vlistCallback = NULL;
#else
static int _vlistOffset = -1;
static FARPROC _vlistCallback = (FARPROC)DefWindowProc;
long FAR PASCAL _export VListJumpProcedure(HWND hWnd, WORD wMsg, WORD wParam, LONG lParam)
{
	UIW_VT_LIST *object = (UIW_VT_LIST *)GetWindowLong(hWnd, _vlistOffset);
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}
static FARPROC _vlistJumpInstance = (FARPROC)VListJumpProcedure;
#endif

EVENT_TYPE UIW_VT_LIST::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_VT_LIST);
	switch (ccode)
	{
	case S_INITIALIZE:
		UIW_WINDOW::Event(event);
		break;

	case S_CREATE:
		UI_WINDOW_OBJECT::Event(event);
#if defined(WIN32)
		RegisterObject("UIW_VT_LIST", "LISTBOX", &_vlistCallback);
#else
		RegisterObject("UIW_VT_LIST", "LISTBOX", &_vlistOffset,
			&_vlistJumpInstance, &_vlistCallback);
#endif
		SendMessage(screenID, WM_SETREDRAW, FALSE, 0);
		for (object = First(); object; object = object->Next())
			object->Event(event);
		SendMessage(screenID, WM_SETREDRAW, TRUE, 0);
		break;

	case S_REGISTER_OBJECT:
#if defined(WIN32)
		RegisterObject("UIW_VT_LIST", "LISTBOX", &_vlistCallback);
#else
		RegisterObject("UIW_VT_LIST", "LISTBOX", &_vlistOffset,
			&_vlistJumpInstance, &_vlistCallback);
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
		{
			object->relative.left = 0;
			object->relative.right = true.right - true.left;
			if (FlagSet(woFlags, WOF_BORDER))
				object->relative.right -= 2;
			UIW_WINDOW::Event(event);
		}
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
			{
				if(Inherited(ID_COMBO_BOX))									// BUG.1550
					SendMessage(screenID, CB_DELETESTRING, UI_LIST::Index(object), 0);
				else
					SendMessage(screenID, LB_DELETESTRING, UI_LIST::Index(object), 0);
			}
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
			object = (UI_WINDOW_OBJECT *)measure->itemData;
			object->Event(event);
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
			object = (selection >= 0) ? (UI_WINDOW_OBJECT *)UI_LIST::Get(LOWORD(selection)) : NULL;
			UIW_WINDOW::Add(object);
		}
		// Check for PageDown on bitmap children (0x0022).
		else if (message == WM_KEYDOWN && event.message.wParam == 0x0022 &&	// BUG.1381
			FlagSet(wnFlags, WNF_BITMAP_CHILDREN) && Current())
		{
			long selection;
//			if (FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
			if (FlagSet(wnFlags, WNF_SELECT_MULTIPLE) || !FlagSet(wnFlags, WNF_AUTO_SELECT))
//				selection = SendMessage(screenID, LB_SETTOPINDEX, 0, 0);
				selection = SendMessage(screenID, LB_GETCARETINDEX, 0, 0);
			else
				selection = SendMessage(screenID, LB_GETCURSEL, 0, 0);
			object = (selection >= 0) ? (UI_WINDOW_OBJECT *)UI_LIST::Get(LOWORD(selection)) : NULL;
			if (!object || !object->Next())
				return (UI_WINDOW_OBJECT::Event(event));
			int height = true.Height() - object->relative.Height();
			while (height > 0 && object->Next())
			{
				object = object->Next();
				height -= object->relative.Height();
				if (height >= 0)
					selection++;
			}
//			if (FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
			if (FlagSet(wnFlags, WNF_SELECT_MULTIPLE) || !FlagSet(wnFlags, WNF_AUTO_SELECT))
			{
//				SendMessage(screenID, LB_SETTOPINDEX, LOWORD(selection), 0);
				SendMessage(screenID, LB_SETCARETINDEX, LOWORD(selection), 0);
			}
			else
			{
				UIW_WINDOW::Add(object);
				SendMessage(screenID, LB_SETCURSEL, LOWORD(selection), 0);
			}
		}
		else
			ccode = UI_WINDOW_OBJECT::Event(event);
		}
		break;
	}

	// Return the control code.
	return (ccode);
}


