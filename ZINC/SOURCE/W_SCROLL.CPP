//	Zinc Interface Library - W_SCROLL.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_WINDOW ---------------------------------------------------------

#if defined(WIN32)
static WNDPROC _scrollCallback = NULL;
#else
static int _scrollOffset = -1;
static FARPROC _scrollCallback = (FARPROC)DefWindowProc;
long FAR PASCAL _export ScrollJumpProcedure(HWND hWnd, WORD wMsg, WORD wParam, LONG lParam)
{
	UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)GetWindowLong(hWnd, _scrollOffset);
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}
static FARPROC _scrollJumpInstance = (FARPROC)ScrollJumpProcedure;
#endif

EVENT_TYPE UIW_SCROLL_BAR::Event(const UI_EVENT &event)
{
	// Ignore the corner scroll bar if windows is running.
	if (FlagSet(sbFlags, SBF_CORNER) && FlagsSet(woFlags, WOF_SUPPORT_OBJECT))
		return (S_ERROR);

	// Switch on the event type.
	EVENT_TYPE ccode = event.type;
	WORD message = event.message.message;
	switch (ccode)
	{
	case S_INITIALIZE:
		UI_WINDOW_OBJECT::Event(event);
		if (FlagsSet(woFlags, WOF_SUPPORT_OBJECT))
		{
			if (FlagSet(sbFlags, SBF_VERTICAL))
				dwStyle = WS_VSCROLL;
			else if (FlagSet(sbFlags, SBF_HORIZONTAL))
				dwStyle = WS_HSCROLL;
			parent->Information(SET_DWSTYLE, &dwStyle);
		}
		else
		{
			if (FlagSet(sbFlags, SBF_VERTICAL))
				dwStyle |= SBS_VERT;
			else if (FlagSet(sbFlags, SBF_HORIZONTAL))
				dwStyle |= SBS_HORZ;
			else
				dwStyle |= SBS_SIZEBOX;
		}
		break;

	case S_SIZE:
	case S_CREATE:
		{
		if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
			break;

//		if (FlagSet(sbFlags, SBF_CORNER | SBF_VERTICAL))
//			relative.left = relative.right - display->cellHeight * 3 / 4;
//		if (FlagSet(sbFlags, SBF_CORNER | SBF_HORIZONTAL))
// 			relative.top = relative.bottom - display->cellHeight * 3 / 4;

		if (FlagSet(sbFlags, SBF_CORNER | SBF_VERTICAL) &&
			display && relative.right == display->cellWidth - 1)
			relative.right = relative.left + display->cellHeight * 3 / 4;
		if (FlagSet(sbFlags, SBF_CORNER | SBF_HORIZONTAL) &&
			display && relative.bottom == display->cellHeight - 3)
		 	relative.bottom = relative.top + display->cellHeight * 3 / 4;

		UI_WINDOW_OBJECT::Event(event);

		if (FlagSet(woFlags, WOF_NON_FIELD_REGION) && FlagSet(sbFlags, SBF_CORNER | SBF_VERTICAL))
			true.left = true.right - display->cellHeight * 3 / 4;
		if (FlagSet(woFlags, WOF_NON_FIELD_REGION) && FlagSet(sbFlags, SBF_CORNER | SBF_HORIZONTAL))
			true.top = true.bottom - display->cellHeight * 3 / 4;

#if defined(WIN32)
		RegisterObject("UIW_SCROLL_BAR", "SCROLLBAR", &_scrollCallback);
#else
		RegisterObject("UIW_SCROLL_BAR", "SCROLLBAR", &_scrollOffset,
			&_scrollJumpInstance, &_scrollCallback);
#endif
		if (scroll.maximum > 0)
		{
			SetScrollRange(screenID, SB_CTL, scroll.minimum, scroll.maximum, FALSE);
			SetScrollPos(screenID, SB_CTL, scroll.current, FALSE);
		}
		else
			SetScrollRange(screenID, SB_CTL, 0, 0, FALSE);
		}
		break;

	case S_REGISTER_OBJECT:
#if defined(WIN32)
		RegisterObject("UIW_SCROLL_BAR", "SCROLLBAR", &_scrollCallback);
#else
		RegisterObject("UIW_SCROLL_BAR", "SCROLLBAR", &_scrollOffset,
			&_scrollJumpInstance, &_scrollCallback);
#endif
		if (scroll.maximum > 0)
		{
			SetScrollRange(screenID, SB_CTL, scroll.minimum, scroll.maximum, FALSE);
			SetScrollPos(screenID, SB_CTL, scroll.current, FALSE);
		}
		else
			SetScrollRange(screenID, SB_CTL, 0, 0, FALSE);
		break;

	case S_VSCROLL_SET:
	case S_HSCROLL_SET:
		{
		if ((ccode == S_HSCROLL_SET && !FlagSet(sbFlags, SBF_HORIZONTAL)) ||
			(ccode == S_VSCROLL_SET && !FlagSet(sbFlags, SBF_VERTICAL)))
			return (S_ERROR);
		scroll = event.scroll;
		if (scroll.maximum > 0)
		{
			SetScrollRange(screenID, SB_CTL, scroll.minimum, scroll.maximum, FALSE);
			SetScrollPos(screenID, SB_CTL, scroll.current, FALSE);
		}
		else
			SetScrollRange(screenID, SB_CTL, 0, 0, FALSE);
		}
		break;

	default:
		if (event.type != E_MSWINDOWS)
			ccode = UI_WINDOW_OBJECT::Event(event);
		else if (message != WM_HSCROLL && message != WM_VSCROLL)
			ccode = UI_WINDOW_OBJECT::Event(event);
		else
			ccode = S_ERROR;
		break;
	}

	if (ccode != S_ERROR || event.type != E_MSWINDOWS ||
		(FlagSet(sbFlags, SBF_VERTICAL) && message != WM_VSCROLL) ||
		(FlagSet(sbFlags, SBF_HORIZONTAL) && message != WM_HSCROLL))
		return (ccode);

	int tCurrent = scroll.current;
	UI_EVENT tEvent;
	ccode = event.message.message;
#if defined(WIN32)
	WORD notification = LOWORD(event.message.wParam);
#else
	WORD notification = event.message.wParam;
#endif
	switch (notification)
	{
	case SB_BOTTOM:
		scroll.current = scroll.maximum;
		tEvent.type = L_BOTTOM;
		break;

	case SB_TOP:
		scroll.current = scroll.minimum;
		tEvent.type = L_TOP;
		break;

	case SB_LINEDOWN:
		scroll.current += scroll.delta;
		tEvent.type = L_DOWN;
		break;

	case SB_LINEUP:
		scroll.current -= scroll.delta;
		tEvent.type = L_UP;
		break;

	case SB_PAGEDOWN:
		scroll.current += scroll.showing;
		tEvent.type = L_PGDN;
		break;

	case SB_PAGEUP:
		scroll.current -= scroll.showing;
		tEvent.type = L_PGDN;
		break;

	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
#if defined(WIN32)
		scroll.current = HIWORD(event.message.wParam);
#else
		scroll.current = LOWORD(event.message.lParam);
#endif
		tEvent.type = notification == SB_THUMBTRACK ? L_CONTINUE_SELECT : L_END_SELECT;
		break;
	}
	if (scroll.current < scroll.minimum)
		scroll.current = scroll.minimum;
	else if (scroll.current > scroll.maximum)
		scroll.current = scroll.maximum;

	if (tCurrent != scroll.current)
		SetScrollPos(screenID, SB_CTL, scroll.current, TRUE);

	tEvent.scroll = scroll;
	if (!FlagSet(woFlags, WOF_SUPPORT_OBJECT) && userFunction)
		UserFunction(tEvent, L_SELECT);
//	else if (FlagSet(sbFlags, SBF_VERTICAL))
//		ScrollWindow(parent->screenID, 0, tCurrent - scroll.current, NULL, NULL);
//	else
//		ScrollWindow(parent->screenID, tCurrent - scroll.current, 0, NULL, NULL);

	// Return the control code.
	return (ccode);
}


