//	Zinc Interface Library - W_WIN1.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <ctype.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_WINDOW ---------------------------------------------------------

UI_WINDOW_OBJECT *UIW_WINDOW::Add(UI_WINDOW_OBJECT *object)
{
	int newObject = FALSE;

	// See if the new object exists or is already current.
	if (!object)
		return (NULL);
	else if (object == current)
	{
		object->woStatus |= WOS_CURRENT;
		if (object->screenID && screenID != object->screenID && 			// BUG.1345
			!FlagSet(object->woStatus, WOS_INTERNAL_ACTION | WOS_WINDOWS_ACTION) &&
			GetFocus() != object->screenID)
			SetFocus(object->screenID);
		return (object);
	}
	// Add the object to the list.
	else if (FlagSet(object->woFlags, WOF_SUPPORT_OBJECT) &&
		support.Index(object) == -1)
	{
		newObject = TRUE;
		support.Add(object);
		object->parent = this;
	}
	else if (!FlagSet(object->woFlags, WOF_SUPPORT_OBJECT) &&
		((!object->Previous() && !object->Next() && object != First()) ||
		 UI_LIST::Index(object) == -1))
	{
		newObject = TRUE;
		UI_LIST::Add(object);
		object->parent = this;

		if (FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT) || FlagSet(object->woStatus, WOS_CURRENT))
		{
			if (current)
				Current()->woStatus &= ~WOS_CURRENT;
			object->woStatus |= WOS_CURRENT;
			current = object;
		}
	}
	else
	{
		UI_WINDOW_OBJECT *tObject = Current();

		// Place MDI objects at the end of the list.
		if (FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT))
		{
			UI_LIST::Subtract(object);
			UI_LIST::Add(object);
		}
		// Make the old current field non-current.
		current = object;
		if (tObject && tObject != object)
		{
			if (FlagSet(woStatus, WOS_INTERNAL_ACTION))
				tObject->woStatus &= ~WOS_CURRENT;
			else
				tObject->Event(UI_EVENT(S_NON_CURRENT, 0, object->true));
		}
		// Update the new current field.
		if (tObject != object)
		{
			if (FlagSet(woStatus, WOS_INTERNAL_ACTION))
				object->woStatus |= WOS_CURRENT;
			else
			{
				eventManager->DeviceState(E_CURSOR, D_OFF);
				object->Event(UI_EVENT(S_CURRENT, 0, object->true));
			}
			if (object->screenID && FlagSet(wnFlags, WNF_AUTO_SELECT) && !FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
				object->Event(L_SELECT);
		}
	}

	// Initialize the object if its parent is already shown on the screen.
	if (screenID && !object->screenID && newObject)
	{
		if (!FlagSet(woStatus, WOS_WINDOWS_ACTION))
			SendMessage(screenID, WM_SETREDRAW, FALSE, 0);
		object->Event(UI_EVENT(S_INITIALIZE, 0));
		object->Event(UI_EVENT(S_CREATE, 0));
		if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
			clipList.Split(screenID, object->true, parent ? FALSE : TRUE);
		if (!current && !FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT) &&
			!FlagSet(object->woFlags, WOF_NON_SELECTABLE | WOF_SUPPORT_OBJECT))
			current = object;
		if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) &&					// BUG.1438, 1546
			FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT))
				SetFocus(object->screenID);
	}
	else if (object->screenID && screenID != object->screenID && 			// BUG.1345
//			!FlagSet(object->woStatus, WOS_INTERNAL_ACTION | WOS_WINDOWS_ACTION) &&
			!FlagSet(object->woStatus, WOS_INTERNAL_ACTION) &&
			GetFocus() != object->screenID)
				SetFocus(object->screenID);

	// Return a pointer to the object.
	return (object);
}

EVENT_TYPE UIW_WINDOW::DrawItem(const UI_EVENT &, EVENT_TYPE)
{
	return (FALSE);
}

#if defined(WIN32)
static WNDPROC _windowCallback = NULL;
static WNDPROC _mdiFrameCallback = NULL;
static WNDPROC _mdiChildCallback = NULL;
#else
static int _windowOffset = -1;
static FARPROC _windowCallback = (FARPROC)DefWindowProc;
long FAR PASCAL _export WindowJumpProcedure(HWND hWnd, WORD wMsg, WORD wParam, LONG lParam)
{
	// Call the user class object event function.
	UIW_WINDOW *object = (UIW_WINDOW *)GetWindowLong(hWnd, _windowOffset);
	long ccode = object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam));

	// Check for a destroyed window.
	if (wMsg == WM_SYSCOMMAND && (wParam & 0xFFF0) == SC_CLOSE &&
		!object->screenID && !object->parent &&
		!FlagSet(object->woAdvancedFlags, WOAF_NO_DESTROY))
		delete object;
	return (ccode);
}
static FARPROC _windowJumpInstance = (FARPROC)WindowJumpProcedure;

static int _mdiFrameOffset = -1;
static FARPROC _mdiFrameCallback = NULL;
long FAR PASCAL _export MDIFrameJumpProcedure(HWND hWnd, WORD wMsg, WORD wParam, LONG lParam)
{
	// Call the user class object event function.
	UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)GetWindowLong(hWnd, _mdiFrameOffset);
	long ccode = object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam));

	// Check for a destroyed window.
	if (wMsg == WM_SYSCOMMAND && (wParam & 0xFFF0) == SC_CLOSE &&
		!object->screenID && !object->parent &&
		!FlagSet(object->woAdvancedFlags, WOAF_NO_DESTROY))
		delete object;
	return (ccode);
}
static FARPROC _mdiFrameJumpInstance = (FARPROC)MDIFrameJumpProcedure;

static int _mdiChildOffset = -1;
static FARPROC _mdiChildCallback = (FARPROC)DefMDIChildProc;
long FAR PASCAL _export MDIChildJumpProcedure(HWND hWnd, WORD wMsg, WORD wParam, LONG lParam)
{
	UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)GetWindowLong(hWnd, _mdiChildOffset);
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}
static FARPROC _mdiChildJumpInstance = (FARPROC)MDIChildJumpProcedure;
#endif

EVENT_TYPE UIW_WINDOW::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW);
	switch (ccode)
	{
	case S_INITIALIZE:
		UI_WINDOW_OBJECT::Event(event);
//		if (!parent && !screenID)
//		{
//			relative.right -= 2 * display->cellWidth;
//			relative.bottom -= 2 * display->cellHeight;
//			relative.right += 2 * GetSystemMetrics(SM_CXFRAME);
//			relative.bottom += 2 * GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) + 2;
//		}
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			object->Event(event);
		for (object = First(); object; object = object->Next())
		{
			if (FlagSet(object->woStatus, WOS_CURRENT) ||
				(!current && !FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
				 !FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT)))
			{
				if (current)
					Current()->woStatus &= ~WOS_CURRENT;
				if (current && FlagSet(wnFlags, WNF_AUTO_SELECT) && !FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
					Current()->woStatus &= ~WOS_SELECTED;
				current = object;
				object->woStatus |= WOS_CURRENT;
				if (FlagSet(wnFlags, WNF_AUTO_SELECT) && !FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
					object->woStatus |= WOS_SELECTED;
			}
			else
				object->woStatus &= ~WOS_CURRENT;
			object->Event(event);
		}
		break;

	case S_DEINITIALIZE:
		UI_WINDOW_OBJECT::Event(event);
		if (FlagSet(woStatus, WOS_MINIMIZED | WOS_MAXIMIZED))				// BUG.A403
			relative = true = restore;
		menu = 0, icon = 0, title = NULL;
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			object->Event(event);
		for (object = First(); object; object = object->Next())
			object->Event(event);
		break;

	case S_CREATE:
		{
		clipList.Destroy();
		// Specify an absolute minimum size for non-parent and MDI windows.
		if (!parent || FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
		{
			int minWidth = 10 * display->cellWidth;
			if (relative.Width() < minWidth)
				relative.right = relative.left + minWidth - 1;
			int minHeight = display->cellHeight;
			minHeight += 2 * UIW_BORDER::width;
			if (relative.Height() < minHeight)
				relative.bottom = relative.top + minHeight - 1;
		}
		UI_WINDOW_OBJECT::Event(event);
		if (FlagSet(woStatus, WOS_MAXIMIZED))
			dwStyle |= WS_MAXIMIZE;
		else if (FlagSet(woStatus, WOS_MINIMIZED))
			dwStyle |= WS_MINIMIZE;
		if (parent && parent->Inherited(ID_LIST))
			SendMessage(parent->screenID, LB_INSERTSTRING, ListIndex(), (LONG)this);
		else if (parent && parent->Inherited(ID_COMBO_BOX))
			SendMessage(parent->screenID, CB_INSERTSTRING, ListIndex(), (LONG)this);
		else if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && !parent)
		{
#if defined(WIN32)
			RegisterObject("UIW_MDIFRAME", NULL, &_mdiFrameCallback,
				title ? (char *)title->Information(GET_TEXT, NULL) : NULL, menu);
#else
			RegisterObject("UIW_MDIFRAME", NULL, &_mdiFrameOffset,
				&_mdiFrameJumpInstance, &_mdiFrameCallback,
				title ? (char *)title->Information(GET_TEXT, NULL) : NULL, menu);
#endif
			CLIENTCREATESTRUCT clientInfo;
			clientInfo.hWindowMenu = 0;
			clientInfo.idFirstChild = 0x1000;
//			DWORD mdiStyle = WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | (dwStyle & (SBS_VERT | SBS_HORZ | SBS_SIZEBOX)); // BUG.372
			DWORD mdiStyle = WS_CHILD | WS_VISIBLE | (dwStyle & (SBS_VERT | SBS_HORZ | SBS_SIZEBOX));
			mdiClient = CreateWindow("MDICLIENT", NULL, mdiStyle, 0, 0, 0, 0,
				screenID, 0,  display->hInstance, (LPSTR)&clientInfo);
		}
		else if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
		{
#if defined(WIN32)
			RegisterObject("UIW_MDICHILD", NULL, &_mdiChildCallback,
				title ? (char *)title->Information(GET_TEXT, NULL) : NULL, menu);
#else
			RegisterObject("UIW_MDICHILD", NULL, &_mdiChildOffset,
				&_mdiChildJumpInstance, &_mdiChildCallback,
				title ? (char *)title->Information(GET_TEXT, NULL) : NULL, menu);
#endif
		}
		else
		{
#if defined(WIN32)
			RegisterObject("UIW_WINDOW", NULL, &_windowCallback,
				title ? (char *)title->Information(GET_TEXT, NULL) : NULL, menu);
#else
			RegisterObject("UIW_WINDOW", NULL, &_windowOffset,
				&_windowJumpInstance, &_windowCallback,
				title ? (char *)title->Information(GET_TEXT, NULL) : NULL, menu);
#endif
		}
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			object->Event(event);
		for (object = First(); object; object = object->Next())
		{
			object->Event(event);
			if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
				clipList.Split(screenID, object->true, parent ? FALSE : TRUE);
		}
		}
		break;

	case S_REGISTER_OBJECT:
#if defined(WIN32)
		if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && !parent)
			RegisterObject("UIW_MDIFRAME", NULL, &_mdiFrameCallback,
				title ? (char *)title->Information(GET_TEXT, NULL) : NULL, menu);
		else if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
			RegisterObject("UIW_MDICHILD", NULL, &_mdiChildCallback,
				title ? (char *)title->Information(GET_TEXT, NULL) : NULL, menu);
		else
			RegisterObject("UIW_WINDOW", NULL, &_windowCallback,
				title ? (char *)title->Information(GET_TEXT, NULL) : NULL, menu);
#else
		if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && !parent)
			RegisterObject("UIW_MDIFRAME", NULL, &_mdiFrameOffset,
				&_mdiFrameJumpInstance, &_mdiFrameCallback,
				title ? (char *)title->Information(GET_TEXT, NULL) : NULL, menu);
		else if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
			RegisterObject("UIW_MDICHILD", NULL, &_mdiChildOffset,
				&_mdiChildJumpInstance, &_mdiChildCallback,
				title ? (char *)title->Information(GET_TEXT, NULL) : NULL, menu);
		else
			RegisterObject("UIW_WINDOW", NULL, &_windowOffset,
				&_windowJumpInstance, &_windowCallback,
				title ? (char *)title->Information(GET_TEXT, NULL) : NULL, menu);
#endif
		break;

	case S_SIZE:
		if (!screenID || FlagSet(woStatus, WOS_INTERNAL_ACTION))
			break;
		else if (parent && !FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
			return (UI_WINDOW_OBJECT::Event(event));
		ccode = S_UNKNOWN;
		clipList.Destroy();
		for (object = First(); object; object = object->Next())
		{
			UI_REGION region = object->true;
			object->Event(UI_EVENT(S_SIZE, 0));
			if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
				clipList.Split(screenID, object->true, parent ? FALSE : TRUE);
			if (object->true != region)
			{
				WORD sizeFlags = FlagSet(woStatus, WOS_MINIMIZED) ? SWP_NOSIZE : 0;
				WOS_STATUS _woStatus = object->woStatus;
				object->woStatus |= WOS_INTERNAL_ACTION;
				SetWindowPos(object->screenID, 0,
					object->true.left, object->true.top,
					object->true.Width(), object->true.Height(),
					SWP_NOZORDER | SWP_NOREDRAW | sizeFlags);
				// Invalidate the old and new object regions.
				woStatus |= WOS_REDISPLAY;
				RECT winRegion;
				winRegion.left = region.left;
				winRegion.top = region.top;
				winRegion.right = region.right + 1;
				winRegion.bottom = region.bottom + 1;
				InvalidateRect(screenID, &winRegion, TRUE);
				winRegion.left = object->true.left;
				winRegion.top = object->true.top;
				winRegion.right = object->true.right + 1;
				winRegion.bottom = object->true.bottom + 1;
				InvalidateRect(screenID, &winRegion, TRUE);
				// Send the redisplay message to mdi-children.
				if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && parent)
				{
					SendMessage(object->screenID, WM_SETREDRAW, TRUE, 0);
					SendMessage(object->screenID, WM_NCPAINT, TRUE, 0);
					InvalidateRect(object->screenID, NULL, TRUE);
				}
				if (!FlagSet(_woStatus, WOS_INTERNAL_ACTION))
					object->woStatus &= ~WOS_INTERNAL_ACTION;
			}
		}
		if (mdiClient && clipList.First())
		{
			UI_REGION region = clipList.First()->region;
			SetWindowPos(mdiClient, mdiClient, region.left, region.top,
				region.Width(), region.Height(), SWP_NOZORDER | SWP_NOREDRAW);
		}
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
		woStatus |= WOS_WINDOWS_ACTION;
		UI_WINDOW_OBJECT::Event(event);
		if (current)
			ccode = Current()->Event(UI_EVENT(ccode, 0));
		woStatus &= ~WOS_WINDOWS_ACTION;
		break;

	case S_MDICHILD_EVENT + S_CLOSE:										// BUG.1244, 1245
		if (mdiClient && current)
		{
			UI_WINDOW_OBJECT *dObject = Current();
		 	if (dObject->Inherited(ID_PULL_DOWN_MENU))
		 		dObject = (UI_WINDOW_OBJECT *)dObject->userObject;
			woStatus |= WOS_WINDOWS_ACTION;									// BUG.1233.C
			Subtract(dObject);
			woStatus &= ~WOS_WINDOWS_ACTION;
			if (!FlagSet(dObject->woAdvancedFlags, WOAF_NO_DESTROY))
				delete dObject;
		}
		break;

	case S_REDISPLAY:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case S_RESTORE:
	case S_MAXIMIZE:
	case S_MINIMIZE:
		{
		int nCmdShow = SW_RESTORE;
		if (ccode == S_MAXIMIZE)
			nCmdShow = SW_SHOWMAXIMIZED;
		else if (ccode == S_MINIMIZE)
			nCmdShow = SW_SHOWMINIMIZED;
		ShowWindow(screenID, nCmdShow);
		}
		break;

	case S_ADD_OBJECT:
		object = (UI_WINDOW_OBJECT *)event.data;
#if defined(ZIL_EDIT)														// BUG.Optimization
		if (UI_LIST::Index(object) == -1 && FlagSet(object->woStatus, WOS_EDIT_MODE))
		{
			UI_REGION region;
			if (clipList.First())
				region = clipList.First()->region;
			else
			{
				region.left = region.top = 0;
				region.right = relative.right - relative.left;
				region.bottom = relative.bottom - relative.top;
			}
			if (!FlagSet(object->woStatus, WOS_GRAPHICS))
			{
				region.left /= display->cellWidth;
				region.top /= display->cellHeight;
			}
			object->relative.left -= region.left;
			object->relative.top -= region.top;
			object->relative.right -= region.left;
			object->relative.bottom -= region.top;
			UIW_WINDOW::Add(object);
		}
#endif
		UIW_WINDOW::Add(object);
		break;

	case S_SUBTRACT_OBJECT:
		Subtract((UI_WINDOW_OBJECT *)event.data);
		break;

	case L_HELP:
		// See if the low level object processes the help.
		if (current && Current()->Event(event) != S_UNKNOWN)
			return (ccode);
		else if (parent && helpContext == NO_HELP_CONTEXT)
			return (S_UNKNOWN);
		else if (helpSystem)
			helpSystem->DisplayHelp(windowManager, helpContext);
		break;

	case L_FIRST:
		ccode = current ? Current()->Event(event) : S_UNKNOWN;
		for (object = First(); object && ccode == S_UNKNOWN; object = object->Next())
			if (!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT) &&
				!FlagSet(object->woFlags, WOF_NON_SELECTABLE))
			{
				wnFlags |= WNF_MOVE_REVERSE;
				UIW_WINDOW::Add(object);
				wnFlags &= ~WNF_MOVE_REVERSE;
				ccode = L_FIRST;
			}
		break;

	case L_LAST:
		ccode = current ? Current()->Event(event) : S_UNKNOWN;
		for (object = Last(); object && ccode == S_UNKNOWN; object = object->Previous())
			if (!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT) &&
				!FlagSet(object->woFlags, WOF_NON_SELECTABLE))
			{
				wnFlags |= WNF_MOVE_FORWARD;
				UIW_WINDOW::Add(object);
				wnFlags &= ~WNF_MOVE_FORWARD;
				ccode = L_LAST;
			}
		break;

	case L_PREVIOUS:
		{
		EVENT_TYPE tcode = ccode;
		ccode = S_UNKNOWN;
		// Make sure there is a current object.
		if (!current)
			UIW_WINDOW::Add(Last());
		// See if the current object processes the message.
		else if (Current()->Event(event) != S_UNKNOWN)
			ccode = tcode;
		// Don't move if on a support object.
		else if (FlagSet(Current()->woFlags, WOF_SUPPORT_OBJECT))
			UI_ERROR_SYSTEM::Beep();
		// Don't move if on the first field of a non-wrapping window.
		else if (!Current()->Previous() && FlagSet(wnFlags, WNF_NO_WRAP))
			break;
		// Go to the previous field in the window.
		else
		{
			object = Current()->Previous() ? Current()->Previous() : Last();
			while (object && object != current && ccode == S_UNKNOWN)
				if (!FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
					!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
				{
					wnFlags |= WNF_MOVE_REVERSE;
					UIW_WINDOW::Add(object);
					wnFlags &= ~WNF_MOVE_REVERSE;
					ccode = tcode;
				}
				else if (object->Previous())
					object = object->Previous();
				else if (FlagSet(wnFlags, WNF_NO_WRAP))
					break;
				else
					object = Last();
		}
		}
		break;

	case L_NEXT:
		{
		EVENT_TYPE tcode = ccode;
		ccode = S_UNKNOWN;
		// Make sure there is a current object.
		if (!current)
			UIW_WINDOW::Add(First());
		// See if the current object processes the message.
		else if (Current()->Event(event) != S_UNKNOWN)
			ccode = tcode;
		// Don't move if on a support object.
		else if (FlagSet(Current()->woFlags, WOF_SUPPORT_OBJECT))
			UI_ERROR_SYSTEM::Beep();
		// Don't move if on the last field of a non-wrapping window.
		else if (!Current()->Next() && FlagSet(wnFlags, WNF_NO_WRAP))
			break;
		// Go to the next field in the window.
		else
		{
			object = Current()->Next() ? Current()->Next() : First();
			while (object && object != current && ccode == S_UNKNOWN)
				if (!FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
					!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
				{
					wnFlags |= WNF_MOVE_FORWARD;
					UIW_WINDOW::Add(object);
					wnFlags &= ~WNF_MOVE_FORWARD;
					ccode = tcode;
				}
				else if (object->Next())
					object = object->Next();
				else if (FlagSet(wnFlags, WNF_NO_WRAP))
					break;
				else
					object = First();
		}
		}
		break;

	// Default operations.
	default:
		if (ccode != E_MSWINDOWS && current)
			ccode = Current()->Event(event);
		else if (ccode != E_MSWINDOWS)
			ccode = UI_WINDOW_OBJECT::Event(event);
		else
			ccode = S_UNKNOWN;
		break;
	}
	if (ccode != S_UNKNOWN || event.type != E_MSWINDOWS)
		return (ccode);

	// Switch on the windows message.
	int callFunction = FALSE;
	WORD message = event.message.message;
	switch (message)
	{
	case WM_COMPAREITEM:
		{
		COMPAREITEMSTRUCT *compare = (COMPAREITEMSTRUCT *)event.message.lParam;
		UI_WINDOW_OBJECT *item1 = (UI_WINDOW_OBJECT *)compare->itemData1;
		UI_WINDOW_OBJECT *item2 = (UI_WINDOW_OBJECT *)compare->itemData2;
		if (item1->parent && item1->parent != this)
			return (item1->parent->Event(event));
		else if (compareFunction)
			ccode = (*compareFunction)((void *)item1, (void *)item2);
		else
			return (-1);
		}
		break;

	case WM_MEASUREITEM:
		{
		MEASUREITEMSTRUCT *measure = (MEASUREITEMSTRUCT *)event.message.lParam;
		for (object = First(); object; object = object->Next())
		{
#if defined(WIN32)
			LONG controlID = GetWindowLong(object->screenID, GWL_ID);
#else
			WORD controlID = GetWindowWord(object->screenID, GWW_ID);
#endif
			if (controlID == measure->CtlID &&
				FlagSet(object->woStatus, WOS_OWNERDRAW))
				return (object->Event(event));
		}
		callFunction = TRUE;
		}
		break;

 	case WM_DRAWITEM:
		{
		DRAWITEMSTRUCT *draw = (DRAWITEMSTRUCT *)event.message.lParam;
		if (draw->CtlType == ODT_BUTTON)
			ccode = SendMessage(draw->hwndItem, WM_DRAWITEM, 0, (LONG)draw);
		else if (draw->itemID != (unsigned)-1)
		{
			object = (UI_WINDOW_OBJECT *)draw->itemData;
			ccode = object->Event(event);
		}
		}
		break;

	case WM_GETMINMAXINFO:
		if (parent && !FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
		{
			POINT *rgpt = (POINT *)event.message.lParam;
			rgpt[1].x = parent->true.right - parent->true.left + 1;
			rgpt[1].y = parent->true.bottom - parent->true.top + 1;
			rgpt[2].x = rgpt[2].y = 0;
		}
		else
			callFunction = TRUE;
		break;

	case WM_INITMENUPOPUP:
		// Check the system-button for invalid options.
		{
		HMENU nmenu = (HMENU)event.message.wParam;
		if (nmenu != menu && HIWORD(event.message.lParam))
		{
			EnableMenuItem(nmenu, SC_MOVE, MF_BYCOMMAND |
				((FlagSet(woAdvancedFlags, WOAF_NO_MOVE) || FlagSet(woStatus, WOS_MAXIMIZED)) ?
				 MF_GRAYED : MF_ENABLED));
			EnableMenuItem(nmenu, SC_SIZE, MF_BYCOMMAND |
				((FlagSet(woAdvancedFlags, WOAF_NO_SIZE) || FlagSet(woStatus, WOS_MAXIMIZED | WOS_MINIMIZED)) ?
				 MF_GRAYED : MF_ENABLED));
			EnableMenuItem(nmenu, SC_CLOSE, MF_BYCOMMAND |
				(FlagSet(woAdvancedFlags, WOAF_LOCKED) || (parent && !FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))) ?
				MF_GRAYED : MF_ENABLED);
//				((parent || FlagSet(woAdvancedFlags, WOAF_LOCKED)) ? MF_GRAYED : MF_ENABLED));	// BUG.268.2
		}
		else
			callFunction = TRUE;
		}
		break;

	case WM_ERASEBKGND:
		if (FlagSet(woStatus, WOS_MINIMIZED) && icon)
		{
			HDC screenHDC = GetDC(ID_SCREEN);
			HDC objectHDC = (HDC)event.message.wParam;
			SetBkColor(objectHDC, GetBkColor(screenHDC));
			SetBkMode(objectHDC, GetBkMode(screenHDC));
			UI_EVENT tEvent;
			tEvent = event;
			tEvent.message.message = WM_ICONERASEBKGND;
			if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && !parent)
				DefFrameProc(screenID, mdiClient, tEvent.message.message,
					tEvent.message.wParam, tEvent.message.lParam);
			else
				UI_WINDOW_OBJECT::Event(tEvent);
			ReleaseDC(ID_SCREEN, screenHDC);
			ccode = TRUE;
		}
		else
			callFunction = TRUE;
		break;

	case WM_PAINT:
		if (FlagSet(woStatus, WOS_MINIMIZED) && icon)
		{
			PAINTSTRUCT ps;
			HDC objectHDC = BeginPaint(screenID, &ps);
			RECT region;
			GetClientRect(screenID, &region);
			region.left = (region.right - GetSystemMetrics(SM_CXICON)) / 2;
			region.top = (region.bottom - GetSystemMetrics(SM_CYICON)) / 2;
			DrawIcon(objectHDC, region.left, region.top, icon);
			EndPaint(screenID, &ps);
		}
		else
			callFunction = TRUE;
		break;

	case WM_QUERYDRAGICON:
		if (FlagSet(woStatus, WOS_MINIMIZED) && icon)
			ccode = (EVENT_TYPE)icon;
		else
			callFunction = TRUE;
		break;

	case WM_SETFOCUS:
		if (!parent)
		{
			woStatus |= WOS_WINDOWS_ACTION;
			windowManager->Add(this);
			woStatus &= ~WOS_WINDOWS_ACTION;
		}
		if (current)
		{
			object = Current();
			if (FlagSet(object->woFlags, WOF_SUPPORT_OBJECT))
				UIW_WINDOW::Add((UI_WINDOW_OBJECT *)object->userObject);
			else if (screenID != object->screenID)
			{
				SetFocus(object->screenID);
				break;
			}
		}
		callFunction = TRUE;
		break;

	case WM_VSCROLL:
	case WM_HSCROLL:
		{
#if defined(WIN32)
		if (!event.message.lParam)
			break;
		NUMBERID tNumberID =
			GetWindowLong((HWND)event.message.lParam, GWL_ID);
#else
		if (!HIWORD(event.message.lParam))
			break;
		NUMBERID tNumberID =
			 GetWindowWord(HIWORD(event.message.lParam), GWW_ID);
#endif
		UI_WINDOW_OBJECT *object =
			(UI_WINDOW_OBJECT *)Information(GET_NUMBERID_OBJECT, VOIDP(&tNumberID));
		if (object)
			ccode = object->Event(event);
		}
		break;

	case WM_COMMAND:
		{
#if defined(WIN32)
		WORD tNumberID = LOWORD(event.message.wParam);
		LONG tScreenID = event.message.lParam;
#else
		WORD tNumberID = event.message.wParam;
		WORD tScreenID = (LOWORD(event.message.lParam));
#endif
		object = (UI_WINDOW_OBJECT *)Information(GET_NUMBERID_OBJECT, VOIDP(&tNumberID));
//		if (object)															// BUG.1619
		if (object && object != this)
		{
			if (tScreenID)
				ccode = object->Event(event);
			else
			{
				// object is a menu item.
				UI_EVENT aEvent(S_ADD_OBJECT);
				aEvent.data = object;
				object->parent->Event(aEvent);
				ccode = object->Event(UI_EVENT(L_SELECT, 0));
			}
		}
		callFunction = TRUE;
		}
		break;

	case WM_SYSCOMMAND:
		{
		callFunction = TRUE;
		WORD command = event.message.wParam;
		if (command < 0xF000)												// BUG.1680
		{
			object = (UI_WINDOW_OBJECT *)Information(GET_NUMBERID_OBJECT, &command);
			if (object)
				ccode = object->Event(UI_EVENT(L_SELECT));
		}
		else
		{
			command &= 0xFFF0;
			if (command == SC_RESTORE)										// BUG.1060
				woStatus &= ~(WOS_MINIMIZED | WOS_MAXIMIZED);
			else if (command == SC_MOVE)
			{
				if (FlagSet(woAdvancedFlags, WOAF_NO_MOVE))
					callFunction = FALSE;
			}
			else if (command == SC_SIZE)
			{
				if (FlagSet(woAdvancedFlags, WOAF_NO_SIZE))
					callFunction = FALSE;
			}
			else if (command == SC_MINIMIZE || command == SC_MAXIMIZE)		// BUG.A403
				restore = relative;
			else if (command == SC_CLOSE)
			{
				if (screenID == windowManager->screenID)
				{
		 			eventManager->Put(UI_EVENT(L_EXIT_FUNCTION, 0));
					callFunction = FALSE;
				}
				else if (FlagSet(woAdvancedFlags, WOAF_LOCKED))
					callFunction = FALSE;
				else if (FlagSet(woAdvancedFlags, WOAF_MODAL))
				{
					for (UI_WINDOW_OBJECT *tObject = Next(); tObject; tObject = tObject->Next())
					{
						EnableWindow(tObject->screenID, TRUE);
						if (FlagSet(tObject->woAdvancedFlags, WOAF_MODAL))
							break;
					}
				}
		}
		}
		}
		break;

	case WM_SYSCHAR:
	{
		char hotChar = event.message.wParam;								// BUG.1360
		if (!isalpha(hotChar) && !isdigit(hotChar))
		{
			callFunction = TRUE;
			break;
		}
	}
	// Fall through to WM_MENUCHAR.

	case WM_MENUCHAR:
		{
		callFunction = TRUE;
		for (object = First(); object && callFunction; object = object->Next())
			if (object->hotKey == HOT_KEY_SUB_WINDOW)
			{
				ccode = object->Event(event);
				if (ccode == 0x00010000L)
				{
					UIW_WINDOW::Add(object);
					callFunction = FALSE;
				}
			}
			else if (object->hotKey == event.message.wParam &&
				!FlagSet(object->woFlags, WOF_NON_SELECTABLE) &&
				!FlagSet(object->woAdvancedFlags, WOAF_NON_CURRENT))
			{
				UIW_WINDOW::Add(object);
				if (object->Inherited(ID_BUTTON) || object->Inherited(ID_ICON))
					object->Event(UI_EVENT(L_SELECT, 0));
				callFunction = FALSE;
				ccode = 0x00010000L;
				break;
			}
		}
		break;

//	case WM_MENUSELECT:														// BUG.1345
//		object = Current();
//		if (!object || (!object->Inherited(ID_PULL_DOWN_MENU) && !object->Inherited(ID_SYSTEM_BUTTON)))
//		{
//			for (object = (UI_WINDOW_OBJECT *)support.Last(); object; object = object->Previous())
//				if (object->Inherited(ID_PULL_DOWN_MENU) || object->Inherited(ID_SYSTEM_BUTTON))
//				{
//					object->userObject = current;
//					UIW_WINDOW::Add(object);
//					break;
//				}
//		}
//		object = Current();
//		if (LOWORD(event.message.lParam) == (WORD)(-1) && HIWORD(event.message.wParam) == 0 &&
//			object && object->Inherited(ID_PULL_DOWN_MENU) || object->Inherited(ID_SYSTEM_BUTTON))
//			UIW_WINDOW::Add((UI_WINDOW_OBJECT *)object->userObject);
//		else if (object)
//			object->Event(event);
//		break;

	case WM_MENUSELECT:														// BUG.1579.C
		object = Current();
		if (!object || (!object->Inherited(ID_PULL_DOWN_MENU) && !object->Inherited(ID_SYSTEM_BUTTON)))
		{
			for (object = (UI_WINDOW_OBJECT *)support.Last(); object; object = object->Previous())
				if (object->Inherited(ID_PULL_DOWN_MENU) || object->Inherited(ID_SYSTEM_BUTTON))
				{
					object->userObject = current;
					object->woStatus |= WOS_INTERNAL_ACTION;				// BUG.1345
					UIW_WINDOW::Add(object);
					object->woStatus &= ~WOS_INTERNAL_ACTION;
					break;
				}
		}
		object = Current();
		if (LOWORD(event.message.lParam) == (WORD)(-1) && HIWORD(event.message.wParam) == 0 &&
			object && object->Inherited(ID_PULL_DOWN_MENU) || object->Inherited(ID_SYSTEM_BUTTON))
			UIW_WINDOW::Add((UI_WINDOW_OBJECT *)object->userObject);
		else if (object)
			object->Event(event);
		break;

	case WM_SETCURSOR:														// ENHANCE.General
		{
		WORD hiTest = LOWORD(event.message.lParam);
		if (FlagSet(woAdvancedFlags, WOAF_NO_SIZE) &&
			(hiTest == HTLEFT || hiTest == HTTOP ||
			 hiTest == HTRIGHT || hiTest == HTBOTTOM ||
			 hiTest == HTTOPLEFT || hiTest == HTTOPRIGHT ||
			 hiTest == HTBOTTOMLEFT || hiTest == HTBOTTOMRIGHT))
			SetCursor(LoadCursor(0, IDC_ARROW));
		else
			callFunction = TRUE;
		}
		break;

	case WM_MOVE:
		if (!parent)
		{
			callFunction = TRUE;
			RECT winRegion;
			GetWindowRect(screenID, &winRegion);
			true.Assign(winRegion);

#if defined(ZIL_EDIT)														// BUG.Optimization
			if (FlagSet(woStatus, WOS_EDIT_MODE) && (true.left < 0 || true.top < 0))
			{
				int width = true.Width();
				int height = true.Height();
				if (true.left < 0)
					true.left = 0;
				if (true.top < 0)
					true.top = 0;
				true.right = true.left + width - 1;
				true.bottom = true.top + height - 1;
				SetWindowPos(screenID, screenID, true.left, true.top,
					width, height, SWP_NOZORDER);
			}
#endif
			relative = true;
		}
		break;

	case WM_SIZE:
		if (FlagSet(woStatus, WOS_INTERNAL_ACTION))
			break;
		callFunction = TRUE;
		woStatus &= ~(WOS_MINIMIZED | WOS_MAXIMIZED);
		if (event.message.wParam == SIZEICONIC)
			woStatus |= WOS_MINIMIZED;
		else if (event.message.wParam == SIZEFULLSCREEN)
			woStatus |= WOS_MAXIMIZED;
		if (!parent)
		{
			RECT rect;
			GetWindowRect(screenID, &rect);
			true.Assign(rect);
			relative = true;
		}
		else
		{
			true.right = true.left + LOWORD(event.message.lParam) - 1;
			true.bottom = true.top + HIWORD(event.message.lParam) - 1;
		}
		if (!parent && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
			ccode = DefFrameProc(screenID, mdiClient, event.message.message,
				event.message.wParam, event.message.lParam);
		else
			ccode = UI_WINDOW_OBJECT::Event(event);
		Event(UI_EVENT(S_SIZE, 0));
		if (FlagSet(woStatus, WOS_REDISPLAY))
		{
			SendMessage(screenID, WM_SETREDRAW, TRUE, 0);
			InvalidateRect(screenID, NULL, TRUE);
			UpdateWindow(screenID);
			woStatus &= ~WOS_REDISPLAY;
		}
		break;

	case WM_LBUTTONDOWN:
	case WM_NCLBUTTONDOWN:
		callFunction = TRUE;
		Information(HIDE_SUBWINDOW, NULL);	// See if a combo-box is active.
		if (message == WM_NCLBUTTONDOWN)
			break;
		// Continue to WM_LBUTTONDBLCLK.

#if defined(ZIL_EDIT)
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
		if (FlagSet(woStatus, WOS_EDIT_MODE))
		{
			for (object = First(); object; object = object->Next())
			{
				UI_REGION region = object->true;
				if (object->Inherited(ID_COMBO_BOX))
					region.bottom = region.top + display->cellHeight -
						display->preSpace - display->postSpace;
				if (region.Overlap(event.position))
				{
					UI_EVENT tEvent;
					tEvent = event;
					tEvent.position.column -= object->true.left;
					tEvent.position.line -= object->true.top;
					return (object->Event(tEvent));
				}
			}
		}
		callFunction = TRUE;
		break;
#endif

	default:
		callFunction = TRUE;
		break;
	}
	if (callFunction && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && !parent)
	{
		ccode = DefFrameProc(screenID, mdiClient, event.message.message,
			event.message.wParam, event.message.lParam);
		if (event.message.message == WM_NCDESTROY)
		{
			woStatus |= WOS_WINDOWS_ACTION;
			*windowManager - this;
			woStatus &= ~WOS_WINDOWS_ACTION;
		}
	}
	else if (callFunction)
		ccode = UI_WINDOW_OBJECT::Event(event);

	// Return the control code.
	return (ccode);
}

void UIW_WINDOW::RegionMax(UI_WINDOW_OBJECT *object)
{
	// Check to see if it is the top level object.
	if (object == this && (!parent || FlagSet(woAdvancedFlags, WOAF_TEMPORARY)))
	{
		true = clip = relative;
		clipList.Destroy();
		return;
	}

	// Find the appropriate region element.
	UI_REGION_ELEMENT *element = clipList.First();
	UI_REGION region;
	if (element)						// Get first available region.
		region = element->region;
	else								// Determine the update region.
	{
		RECT tRegion;
		GetClientRect(screenID, &tRegion); tRegion.right--; tRegion.bottom--;	// Fix Windows right/bottom "feature".
		region.Assign(tRegion);
		clipList.Add(new UI_REGION_ELEMENT(screenID, region));
	}
	int bottomRegion = FlagsSet(object->woFlags, WOF_NON_FIELD_REGION | WOF_JUSTIFY_RIGHT);
	for ( ; element; element = element->Next())
		if (!bottomRegion && region.top > element->region.top)
			region = element->region;
		else if (bottomRegion && region.bottom < element->region.bottom)
			region = element->region;

	// Compute the actual field region.
	object->true = region;
	if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
	{
		object->relative.right = object->relative.right - object->relative.left;
		object->relative.bottom = object->relative.bottom - object->relative.top;
		object->relative.left = object->relative.top = 0;
	}
	else
	{
		object->true.left = region.left + object->relative.left;
		object->true.top = region.top + object->relative.top;
		object->true.right = region.left + object->relative.right;
		object->true.bottom = region.top + object->relative.bottom;
	}
	object->clip = object->true;
}

UI_WINDOW_OBJECT *UIW_WINDOW::Subtract(UI_WINDOW_OBJECT *object)
{
	UI_WINDOW_OBJECT *nextObject = object ? object->Next() : NULL;

	// Remove the object from the list.
	if (!object)
		return (nextObject);
	else if (screenID && !FlagSet(woStatus, WOS_WINDOWS_ACTION))
		SendMessage(screenID, WM_SETREDRAW, FALSE, 0);
	if (UI_LIST::Index(object) != -1)
	{
		UI_EVENT event;
		if (FlagSet(woStatus, WOS_INTERNAL_ACTION))
			;
		else if (object->screenID != screenID)
		{
			woStatus |= WOS_INTERNAL_ACTION;
			if (parent || !FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))		// BUG.1233.C
				DestroyWindow(object->screenID);
			else
				SendMessage(mdiClient, WM_MDIDESTROY, (int)object->screenID, NULL);
			event.type = S_DEINITIALIZE;
			object->screenID = 0;
			object->Event(event);
			woStatus &= ~WOS_INTERNAL_ACTION;
		}
		else
		{
			woStatus |= WOS_INTERNAL_ACTION;
			event.type = S_SUBTRACT_OBJECT;
			event.data = object;
			Event(event);
			object->screenID = 0;											// BUG.1440
			woStatus &= ~WOS_INTERNAL_ACTION;
		}
//		if (object == current)	// Check for a current support object.
		if (object == current || (current && Current()->Inherited(ID_PULL_DOWN_MENU)
			&& object == Current()->userObject))							// BUG.1576
		{
/* START BLOCK COMMENT
**				current = NULL;
**				if (parent || !FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))		// BUG.1233.C
**				{
**					for (UI_WINDOW_OBJECT *tObject = First(); tObject; tObject = tObject->Next())
**						if (!FlagSet(tObject->woFlags, WOF_NON_SELECTABLE) &&	// BUG.General
**							!FlagSet(tObject->woAdvancedFlags, WOAF_NON_CURRENT))
**						{
**							if (object == current)
**							{
**								current = NULL;
**								Add(tObject);
**							}
**							else
**								Current()->userObject = tObject;
**							break;
**						}
**				}
**				else
**				{
**					for (UI_WINDOW_OBJECT *tObject = Last(); tObject; tObject = tObject->Previous())
**						if (!FlagSet(tObject->woFlags, WOF_NON_SELECTABLE) &&	// BUG.General
**							!FlagSet(tObject->woAdvancedFlags, WOAF_NON_CURRENT))
**						{
**							if (object == current)
**							{
**								current = NULL;
**								Add(tObject);
**							}
**							else
**								Current()->userObject = tObject;
**							break;
**						}
**				}
END BLOCK COMMENT */
			UI_WINDOW_OBJECT *tObject;
			if (parent || !FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))		// BUG.1233.C
				for (tObject = First();
					tObject && (tObject == object ||
						FlagSet(tObject->woFlags, WOF_NON_SELECTABLE) ||
						FlagSet(tObject->woAdvancedFlags, WOAF_NON_CURRENT));
					tObject = tObject->Next())
						;
			else
			{
#if defined(WIN32)
				HWND cWindowHwnd = (HWND)SendMessage(mdiClient, WM_MDIGETACTIVE, 0, 0);
#else
				HWND cWindowHwnd = 0x0000FFFFL & SendMessage(mdiClient, WM_MDIGETACTIVE, 0, 0);
#endif
				for (tObject = Last();
					tObject && tObject->screenID != cWindowHwnd;
					tObject = tObject->Previous())
						;
			}

			if (object != current)
				Current()->userObject = tObject;
			current = NULL;
			woStatus |= WOS_WINDOWS_ACTION;
			Add(tObject);
			woStatus &= ~WOS_WINDOWS_ACTION;
		}
		if (UI_LIST::Index(object) != -1)	// Make sure the object is still in the list.
			UI_LIST::Subtract(object);
	}
	else if (support.Index(object) != -1)
	{
		support.Subtract(object);
		if (object == current)	// Check for a current support object.
			current = First();
		if (object == vScroll)
			vScroll = NULL;
		else if (object == hScroll)
			hScroll = NULL;
		else if (object == title)
			title = NULL;
	}

	// Return a pointer to the next object.
	return (nextObject);
}


