//	Zinc Interface Library - W_PROMPT.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_PROMPT ---------------------------------------------------------

#if defined(WIN32)
static WNDPROC _promptCallback = NULL;
#else
static int _promptOffset = -1;
static FARPROC _promptCallback = (FARPROC)DefWindowProc;
long FAR PASCAL _export PromptJumpProcedure(HWND hWnd, WORD wMsg, WORD wParam, LONG lParam)
{
	UIW_PROMPT *object = (UIW_PROMPT *)GetWindowLong(hWnd, _promptOffset);
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}
static FARPROC _promptJumpInstance = (FARPROC)PromptJumpProcedure;
#endif

EVENT_TYPE UIW_PROMPT::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	// Virtualize the display.
	display->VirtualGet(screenID, true);

	// Draw the prompt information.
	DrawText(ID_DIRECT, true, text, lastPalette, FALSE, ccode);

	// Restore the display and return the control code.
	display->VirtualPut(screenID);
	return (TRUE);
}

EVENT_TYPE UIW_PROMPT::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_INITIALIZE:
		UI_WINDOW_OBJECT::Event(event);
		if (hotKey && Next())
			Next()->HotKey(hotKey);
		break;

	case S_CREATE:
	case S_SIZE:
		if (!FlagSet(woFlags, WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT))		// BUG.1399
		{
			relative.right = relative.left + display->TextWidth(text, screenID, font) + 1;
			if (FlagSet(woFlags, WOF_BORDER))
				relative.right += display->cellWidth;
		}
		if (!FlagSet(woFlags, WOF_BORDER))
			relative.bottom = relative.top + display->cellHeight - display->preSpace - display->postSpace - 1;
		UI_WINDOW_OBJECT::Event(event);
#if defined(WIN32)
		RegisterObject("UIW_PROMPT", "STATIC", &_promptCallback, text);
#else
		RegisterObject("UIW_PROMPT", "STATIC", &_promptOffset,
			&_promptJumpInstance, &_promptCallback, text ? text : "");		// BUG.1700
#endif
		if (ccode == S_SIZE && screenID != parent->screenID)
		{
			RECT winRegion;
			winRegion.left = true.left;
			winRegion.top = true.top;
			winRegion.right = true.right + 1;
			winRegion.bottom = true.bottom + 1;
			InvalidateRect(parent->screenID, &winRegion, TRUE);
			SetWindowPos(screenID, 0, 0, 0, true.Width(), true.Height(),
				SWP_NOMOVE | SWP_NOZORDER);
		}
		break;

	case S_REGISTER_OBJECT:
#if defined(WIN32)
		RegisterObject("UIW_PROMPT", "STATIC", &_promptCallback, text);
#else
		RegisterObject("UIW_PROMPT", "STATIC", &_promptOffset,
			&_promptJumpInstance, &_promptCallback, text ? text : "");		// BUG.1700
#endif
		break;

	default:
		WORD message = event.message.message;
		if (event.type != E_MSWINDOWS)
			ccode = UI_WINDOW_OBJECT::Event(event);
		else if (message == WM_PAINT)
		{
			lastPalette = parent->LogicalPalette(ccode, parent->SearchID());
			UI_REGION region = true;
			if (FlagSet(woFlags, WOF_BORDER))
				--region;
			ccode = DrawText(screenID, region, text, lastPalette, TRUE, ccode);
		}
		else
			ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}


