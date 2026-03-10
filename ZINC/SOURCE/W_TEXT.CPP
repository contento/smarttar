//	Zinc Interface Library - W_TEXT.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif
const int STRING_OFFSET = 5;

// ----- UIW_TEXT -----------------------------------------------------------

#if defined(WIN32)
static WNDPROC _textCallback = NULL;
#else
static int _textOffset = -1;
static FARPROC _textCallback = (FARPROC)DefWindowProc;
long FAR PASCAL _export TextJumpProcedure(HWND hWnd, WORD wMsg, WORD wParam, LONG lParam)
{
	UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)GetWindowLong(hWnd, _textOffset);
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}
static FARPROC _textJumpInstance = (FARPROC)TextJumpProcedure;
#endif

EVENT_TYPE UIW_TEXT::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	// Virtualize the display.
	display->VirtualGet(screenID, true);
	lastPalette = LogicalPalette(ccode, parent->Inherited(ID_LIST) ? ID_LIST : searchID);

	// Draw the text information.
	UI_REGION region = true;
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);
	display->Rectangle(screenID, region, lastPalette, 0, TRUE);
	if (FlagSet(woStatus, WOS_CURRENT))
		display->Rectangle(screenID, true, lastPalette, 1, FALSE, TRUE);
	DrawText(screenID, region, text, lastPalette, FALSE, ccode);

	// Restore the display and return the control code.
	display->VirtualPut(screenID);
	return (TRUE);
}

EVENT_TYPE UIW_TEXT::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_TEXT);
	switch (ccode)
	{
	case S_INITIALIZE:
		UIW_WINDOW::Event(event);
		break;

	case S_CREATE:
		UI_WINDOW_OBJECT::Event(event);
		// Continue to S_REGISTER_OBJECT.

	case S_REGISTER_OBJECT:
		if (true.Height() < display->cellHeight)
		{
			dwStyle &= ~(ES_AUTOVSCROLL | ES_MULTILINE);
			dwStyle |= ES_AUTOHSCROLL;
		}
#if defined(WIN32)
		RegisterObject("UIW_TEXT", "EDIT", &_textCallback, text);
#else
		RegisterObject("UIW_TEXT", "EDIT", &_textOffset, &_textJumpInstance,
			&_textCallback, text);
#endif
		if (screenID != parent->screenID)
			SendMessage(screenID, EM_LIMITTEXT, maxLength - 1, FALSE);
		break;

	case L_PREVIOUS:
	case L_NEXT:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case L_SELECT:
		if (!FlagSet(woFlags, WOF_VIEW_ONLY))
		{
			ccode = CallWindowProc((FARPROC)defaultCallback, screenID,
				event.message.message, event.message.wParam, event.message.lParam);
			UserFunction(event, ccode);
		}
		break;

	case S_NON_CURRENT:
		if (screenID != parent->screenID && SendMessage(screenID, EM_GETMODIFY, 0, 0))
		{
			woStatus |= WOS_CHANGED;
			SendMessage(screenID, EM_SETMODIFY, 0, 0);
		}
		if (FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
			DataGet();
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case S_VSCROLL:
		{
		long delta = event.scroll.delta;
		SendMessage(screenID, EM_LINESCROLL, 0, delta);
		}
		break;

	case S_HSCROLL:
		{
		long delta = event.scroll.delta;
#if defined(WIN32)
		SendMessage(screenID, EM_LINESCROLL, delta, 0);
#else
		delta <<= 16;
		SendMessage(screenID, EM_LINESCROLL, 0, delta);
#endif
		}
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
		else if (!FlagSet(woFlags, WOF_VIEW_ONLY) || message != WM_CHAR)
			ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}


