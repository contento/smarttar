//	Zinc Interface Library - W_STRING.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_STRING ---------------------------------------------------------

#if defined(WIN32)
static WNDPROC _stringCallback = NULL;
#else
static int _stringOffset = -1;
static FARPROC _stringCallback = (FARPROC)DefWindowProc;
long FAR PASCAL _export StringJumpProcedure(HWND hWnd, WORD wMsg, WORD wParam, LONG lParam)
{
	// Get the object pointer and call the C++ class object.
	UIW_STRING *object = (UIW_STRING *)GetWindowLong(hWnd, _stringOffset);
	if (FlagSet(object->stFlags, STF_VARIABLE_NAME) && wMsg == WM_CHAR && wParam == ' ')
		wParam = '_';
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}
static FARPROC _stringJumpInstance = (FARPROC)StringJumpProcedure;
#endif

EVENT_TYPE UIW_STRING::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
	SCREENID screenID = this->screenID;
	UI_DISPLAY *display = this->display;
#endif
	const int STRING_OFFSET = 4;

	// Virtualize the display.
	display->VirtualGet(screenID, true);
	lastPalette = LogicalPalette(ccode, parent->Inherited(ID_LIST) ? ID_LIST : searchID);

	// Draw the string information.
	UI_REGION region = true;
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);
	display->Rectangle(screenID, region, lastPalette, 0, TRUE);
	if (FlagSet(woStatus, WOS_CURRENT))
		display->Rectangle(screenID, true, lastPalette, 1, FALSE, TRUE);
	region.left += STRING_OFFSET;
	if (parent->Inherited(ID_VT_LIST))
		region.left += relative.left;
	if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT | WOF_JUSTIFY_CENTER))
		region.right -= STRING_OFFSET;
	DrawText(screenID, region, text, lastPalette, FALSE, ccode);

	// Restore the display and return the control code.
	display->VirtualPut(screenID);
	return (TRUE);
}

EVENT_TYPE UIW_STRING::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_STRING);
	switch (ccode)
	{
	case S_INITIALIZE:
		UI_WINDOW_OBJECT::Event(event);
		relative.bottom = relative.top + display->cellHeight - display->preSpace - display->postSpace - 1;
		break;

	case S_CREATE:
		if (FlagSet(woStatus, WOS_UNANSWERED))
			strcpy(text, "");
		// Continue to S_SIZE.
	case S_SIZE:
		UI_WINDOW_OBJECT::Event(event);
		if (FlagSet(woFlags, WOF_NON_FIELD_REGION))
		{
			true.bottom++;
			true.top = true.bottom - display->cellHeight + 1;
		}
		else if (!FlagSet(woFlags, WOF_BORDER))
		{
			true.top += (display->preSpace + 1);
			true.bottom -= (display->postSpace + 1);
		}
		// Continue to S_REGISTER_OBJECT.

	case S_REGISTER_OBJECT:
#if defined(WIN32)
		RegisterObject("UIW_STRING", "EDIT", &_stringCallback, text);
#else
		RegisterObject("UIW_STRING", "EDIT", &_stringOffset,
			&_stringJumpInstance, &_stringCallback, text);
#endif
		if (screenID != parent->screenID)
			SendMessage(screenID, EM_LIMITTEXT, maxLength - 1, FALSE);
		break;

	case L_SELECT:
	case S_NON_CURRENT:
		if (screenID != parent->screenID && SendMessage(screenID, EM_GETMODIFY, 0, 0))
		{
			woStatus |= WOS_CHANGED;
			SendMessage(screenID, EM_SETMODIFY, 0, 0);
		}
		if (FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
			DataGet();
//		ccode = UI_WINDOW_OBJECT::Event(event);								// BUG.General
//		break;
		// Fall through to L_END_SELECT.

	case L_END_SELECT:
//		ccode = UserFunction(event, L_SELECT);								// BUG.1346
		ccode = UI_WINDOW_OBJECT::Event(UI_EVENT(ccode));
		break;

	case L_PREVIOUS:														// BUG.1278
	case L_NEXT:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case L_BACKSPACE:
	case L_DELETE:
	case L_DELETE_WORD:
	case L_DELETE_EOL:
		if (FlagSet(woFlags, WOF_VIEW_ONLY))
			break;
		// Continue to default.
	default:
		WORD message = event.message.message;
		if (event.type != E_MSWINDOWS)
			ccode = UI_WINDOW_OBJECT::Event(event);
																			// BUG.1584
		else if (parent->Inherited(ID_LIST) && message == WM_CHAR && event.message.wParam == ' ')
			ccode = UI_WINDOW_OBJECT::Event(UI_EVENT(L_SELECT));
		else if (!FlagSet(woFlags, WOF_VIEW_ONLY) || message != WM_CHAR)
			ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}


