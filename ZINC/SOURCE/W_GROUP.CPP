//	Zinc Interface Library - W_GROUP.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_WINDOW ---------------------------------------------------------

#if defined(WIN32)
static WNDPROC _groupCallback = NULL;
#else
static int _groupOffset = -1;
static FARPROC _groupCallback = (FARPROC)DefWindowProc;
long FAR PASCAL _export GroupJumpProcedure(HWND hWnd, WORD wMsg, WORD wParam, LONG lParam)
{
	UIW_GROUP *object = (UIW_GROUP *)GetWindowLong(hWnd, _groupOffset);
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}
static FARPROC _groupJumpInstance = (FARPROC)GroupJumpProcedure;
#endif

EVENT_TYPE UIW_GROUP::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_GROUP);
	switch (ccode)
	{
	case S_INITIALIZE:
		UIW_WINDOW::Event(event);
		break;

	case S_CREATE:
		UI_WINDOW_OBJECT::Event(event);
#if defined(WIN32)
		RegisterObject("UIW_GROUP", "BUTTON", &_groupCallback, text);
#else
		RegisterObject("UIW_GROUP", "BUTTON", &_groupOffset,
			&_groupJumpInstance, &_groupCallback, text);
#endif
		// Compute the list object regions.
		for (object = First(); object; object = object->Next())
			object->Event(event);
		break;

	case S_REGISTER_OBJECT:
#if defined(WIN32)
		RegisterObject("UIW_GROUP", "BUTTON", &_groupCallback, text);
#else
		RegisterObject("UIW_GROUP", "BUTTON", &_groupOffset,
			&_groupJumpInstance, &_groupCallback, text);
#endif
		break;

	case L_PREVIOUS:
	case L_NEXT:
		ccode = S_UNKNOWN;
		break;

	case L_LEFT:
	case L_RIGHT:
		if (current && Current()->Event(event) != S_UNKNOWN)
			break;
		// Continue to L_UP and L_DOWN.
	case L_UP:
	case L_DOWN:
		ccode = UIW_WINDOW::Event(UI_EVENT((ccode == L_LEFT || ccode == L_UP) ? L_PREVIOUS : L_NEXT));
		break;

	default:
		if (event.type != E_MSWINDOWS)
			ccode = UIW_WINDOW::Event(event);
		else if (event.message.message == WM_ERASEBKGND)
		{
			lastPalette = parent->LogicalPalette(ccode);
			UI_MSWINDOWS_DISPLAY::hDC = (HDC)event.message.wParam;
			display->VirtualGet(ID_DIRECT, true);
			display->Rectangle(screenID, true, lastPalette, 0, TRUE);
			display->VirtualPut(ID_DIRECT);
			UI_MSWINDOWS_DISPLAY::hDC = 0;
		}
		else
			ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}


