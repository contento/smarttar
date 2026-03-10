//	Zinc Interface Library - W_TBAR.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_TOOL_BAR -------------------------------------------------------

#if defined(WIN32)
static WNDPROC _toolbarCallback = NULL;
#else
static int _toolbarOffset = -1;
static FARPROC _toolbarCallback = (FARPROC)DefWindowProc;
long FAR PASCAL _export ToolbarJumpProcedure(HWND hWnd, WORD wMsg, WORD wParam, LONG lParam)
{
	UIW_TOOL_BAR *object = (UIW_TOOL_BAR *)GetWindowLong(hWnd, _toolbarOffset);
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}
static FARPROC _toolbarJumpInstance = (FARPROC)ToolbarJumpProcedure;
#endif

EVENT_TYPE UIW_TOOL_BAR::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_TOOL_BAR);
	switch (ccode)
	{
	case S_INITIALIZE:
		UIW_WINDOW::Event(event);
		break;

	case S_SIZE:
		if (!screenID || FlagSet(woStatus, WOS_INTERNAL_ACTION))
			break;
		// Continue to S_CREATE.
	case S_CREATE:
		{
		WOF_FLAGS _woFlags = woFlags;		// Get the remaining window region.
		woFlags |= WOF_NON_FIELD_REGION;
		UI_WINDOW_OBJECT::Event(event);
		woFlags = _woFlags;
		true.left--; true.right++;
		true.bottom = --true.top + relative.bottom - relative.top + 2;

		if (!FlagSet(wnFlags, WNF_NO_WRAP) && First())
		{
			int height = 1;
			for (object = First(); object; object = object->Next())
				if (height < object->relative.bottom - object->relative.top)
					height = object->relative.bottom - object->relative.top + 1;
			true.bottom = true.top + height - 1;
			relative.right = true.right - true.left;
			relative.bottom = true.bottom - true.top;
			int left = 0, top = 0;
			for (object = First(); object; object = object->Next())
			{
				int width = object->relative.right - object->relative.left + 1;
//				if (left + width > relative.right)							// BUG.1689
				if (left > 0 && left + width > relative.right)
				{
					left = 0;
					top += height;
					true.bottom += height;
				}
				object->relative.left = left;
				object->relative.top = top;
				left += width;
				object->relative.right = left - 1;
				object->relative.bottom = top + height - 1;
			}
			true.bottom += 2;
		}

		if (ccode == S_CREATE)
		{
#if defined(WIN32)
			RegisterObject("UIW_TOOL_BAR", NULL, &_toolbarCallback);
#else
			RegisterObject("UIW_TOOL_BAR", NULL, &_toolbarOffset,
				&_toolbarJumpInstance, &_toolbarCallback);
#endif
			for (object = First(); object; object = object->Next())
				object->Event(event);
		}
		else if (!FlagSet(wnFlags, WNF_NO_WRAP))
		{
			for (object = First(); object; object = object->Next())
			{
				UI_REGION region = object->true;
				object->Event(event);
				if (object->true != region)
					SetWindowPos(object->screenID, 0,
						object->true.left, object->true.top,
						object->true.Width(), object->true.Height(),
						SWP_NOZORDER | SWP_NOREDRAW);
			}
		}
		}
		break;

	case S_REGISTER_OBJECT:
#if defined(WIN32)
		RegisterObject("UIW_TOOL_BAR", NULL, &_toolbarCallback);
#else
		RegisterObject("UIW_TOOL_BAR", NULL, &_toolbarOffset,
			&_toolbarJumpInstance, &_toolbarCallback);
#endif
		break;

	case L_PREVIOUS:
	case L_NEXT:
		ccode = S_UNKNOWN;
		break;

	case L_LEFT:
	case L_RIGHT:
		// Make sure the lower-level object does not want the message.
		if (current && Current()->Inherited(ID_STRING) &&
			Current()->Event(event) != S_UNKNOWN)
			break;
	case L_UP:
	case L_DOWN:
		ccode = UIW_WINDOW::Event(UI_EVENT((ccode == L_LEFT || ccode == L_UP) ? L_PREVIOUS : L_NEXT));
		break;

	case S_ADD_OBJECT:
		UIW_WINDOW::Event(event);
		parent->Event(UI_EVENT(S_SIZE, 0));
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}


