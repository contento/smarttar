//	Zinc Interface Library - W_WIN2.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stdio.h>
#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif


#if !defined(NO_ZIL_APP)
// Bug fix for linkers that don't look for WinMain in the .LIBs.
extern "C" int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow);
static FARPROC linkAddr = (FARPROC) VOIDF(WinMain);
#endif

// ----- MAKEPROC_ELEMENT ---------------------------------------------------

class MAKEPROC_ELEMENT : public UI_ELEMENT
{
public:
	HANDLE hInstance;
	FARPROC procInstance;
	char name[32];

	MAKEPROC_ELEMENT(char *_name, FARPROC _procInstance, HANDLE _hInstance) :
		procInstance(_procInstance), hInstance(_hInstance) { strcpy(name, _name); }
	~MAKEPROC_ELEMENT(void)
		{ FreeProcInstance(procInstance); UnregisterClass(name, hInstance); }
	MAKEPROC_ELEMENT *Next(void) { return((MAKEPROC_ELEMENT *)next); }
	MAKEPROC_ELEMENT *Previous(void) { return((MAKEPROC_ELEMENT *)previous); }
};

class MAKEPROC_LIST : public UI_LIST
{
public:
	MAKEPROC_ELEMENT *Current(void) { return((MAKEPROC_ELEMENT *)current); }
	MAKEPROC_ELEMENT *First(void) { return((MAKEPROC_ELEMENT *)first); }
	MAKEPROC_ELEMENT *Last(void) { return((MAKEPROC_ELEMENT *)last); }
};

static MAKEPROC_LIST _makeprocList;

int ZincClassInitialize(HANDLE)
{
	// Clean up the procedure instance list.
	_makeprocList.Destroy();
	return (TRUE);
}

int ZincClassRestore(HANDLE hInstance)
{
	// Clean up the procedure instance list.
	_makeprocList.Destroy();
	return (TRUE);
}

int ZincClassAdd(char *name, FARPROC farproc, HANDLE hInstance)
{
	// Make sure the element does not exist.
	for (MAKEPROC_ELEMENT *element = _makeprocList.First(); element; element = element->Next())
		if (element->hInstance == hInstance &&
			element->procInstance || farproc &&
			ui_stricmp(element->name, name) == 0)
			return (FALSE);

	// Add a procedure element
	_makeprocList.Add(new MAKEPROC_ELEMENT(name, farproc, hInstance));
	return (TRUE);
}

// ----- UI_WINDOW_OBJECT ---------------------------------------------------

// ----- Definition for the Windows callback functions -----
#ifdef __ZTC__
#	if __ZTC__ >= 0x0310
typedef WNDPROC DEFWINPROC;
#	else
typedef LONG (FAR PASCAL *DEFWINPROC)(HWND, unsigned, WORD, LONG);
//typedef long (FAR PASCAL *DEFWINPROC)();	// For early versions of Zortech.
#	endif
#endif
#ifdef _MSC_VER
typedef LONG (FAR PASCAL *DEFWINPROC)(HWND, UINT, UINT, LONG);
#endif
#if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
#if __BORLANDC__ >= 0x0410		// Set by Borland C++ version 3.1
typedef LONG (FAR PASCAL *DEFWINPROC)(HWND, UINT, UINT, LONG);
#else
typedef LONG (FAR PASCAL *DEFWINPROC)(HWND, WORD, WORD, LONG);
#endif
#endif

static int _offset = -1;
static WORD _controlID = 0;
static UI_WINDOW_OBJECT *_object = NULL;
static FARPROC _jumpInstance = NULL;

long FAR PASCAL _export ObjectInitProcedure(HWND hWnd, WORD wMsg, WORD wParam, LONG lParam)
{
	_object->screenID = hWnd;
	SetWindowLong(hWnd, _offset, (LONG)_object);
	SetWindowLong(hWnd, GWL_WNDPROC, (LONG)_jumpInstance);
	if (_object->parent)
		SetWindowWord(hWnd, GWW_ID, _controlID);
	_object = NULL;
	UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)GetWindowLong(hWnd, _offset);
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}

static int _objectOffset = -1;
static FARPROC _objectCallback = (FARPROC)DefWindowProc;
long FAR PASCAL _export ObjectJumpProcedure(HWND hWnd, WORD wMsg, WORD wParam, LONG lParam)
{
	UIW_PROMPT *object = (UIW_PROMPT *)GetWindowLong(hWnd, _objectOffset);
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}
static FARPROC _objectJumpInstance = (FARPROC)ObjectJumpProcedure;

EVENT_TYPE UI_WINDOW_OBJECT::DrawBorder(SCREENID screenID, UI_REGION &region,
	int fillRegion, EVENT_TYPE ccode)
{
	display->VirtualGet(screenID, region);

	// Draw the outer rectangle.
	if (Inherited(ID_BUTTON))
	{
		UI_PALETTE *outline = LogicalPalette(ccode, ID_OUTLINE);
		display->Line(screenID, region.left + 1, region.top, region.right - 1, region.top, outline);
		display->Line(screenID, region.left + 1, region.bottom, region.right - 1, region.bottom, outline);
		display->Line(screenID, region.left, region.top + 1, region.left, region.bottom - 1, outline);
		display->Line(screenID, region.right, region.top + 1, region.right, region.bottom - 1, outline);
	}
	else
		display->Rectangle(screenID, region, lastPalette, 1, fillRegion);
	--region;

	display->VirtualPut(screenID);
	return (TRUE);
}

EVENT_TYPE UI_WINDOW_OBJECT::DrawItem(const UI_EVENT &, EVENT_TYPE )
{
	return (FALSE);
}

EVENT_TYPE UI_WINDOW_OBJECT::DrawShadow(SCREENID screenID, UI_REGION &region,
	int depth, int fillRegion, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
	UI_DISPLAY *display = this->display;
#endif

	display->VirtualGet(screenID, region);

	// Draw the outer shadow.
	UI_PALETTE *outline = LogicalPalette(ccode, ID_OUTLINE);
	int delta = (region == true) ? 1 : 0;
	display->Line(screenID, region.left + delta, region.top, region.right - delta, region.top, outline);
	display->Line(screenID, region.left + delta, region.bottom, region.right - delta, region.bottom, outline);
	display->Line(screenID, region.left, region.top + delta, region.left, region.bottom - delta, outline);
	display->Line(screenID, region.right, region.top + delta, region.right, region.bottom - delta, outline);
	--region;

	// Draw the inner shadow.
	if (depth > 0)
	{
		UI_PALETTE *lightShadow = LogicalPalette(ccode, ID_WHITE_SHADOW);
		UI_PALETTE *darkShadow = LogicalPalette(ccode, ID_DARK_SHADOW);
		for (int i = 0; i < depth; i++)
		{
			display->Line(screenID, region.left, region.top + 1, region.left,
				region.bottom - 1, lightShadow, 1, FALSE);
			display->Line(screenID, region.left, region.top, region.right,
				region.top, lightShadow, 1, FALSE);
			display->Line(screenID, region.right, region.top + 1,
				region.right, region.bottom, darkShadow, 1, FALSE);
			display->Line(screenID, region.left, region.bottom,
				region.right - 1, region.bottom, darkShadow, 1, FALSE);
			--region;
		}
	}
	else if (depth < 0)
	{
		UI_PALETTE *darkShadow = LogicalPalette(ccode, ID_DARK_SHADOW);
		display->Line(screenID, region.left, region.top + 1, region.left,
			region.bottom - 1, darkShadow, 1, FALSE);
		display->Line(screenID, region.left, region.top, region.right,
			region.top, darkShadow, 1, FALSE);
		region.left++;
		region.top++;
	}
	if (fillRegion)
		display->Rectangle(screenID, region, NULL, 0, TRUE);

	display->VirtualPut(screenID);
	return (TRUE);
}

EVENT_TYPE UI_WINDOW_OBJECT::DrawText(SCREENID screenID, UI_REGION &region,
	const char *text, UI_PALETTE *palette, int fillRegion, EVENT_TYPE )
{
#if defined(ZIL_OPTIMIZE)
	UI_DISPLAY *display = this->display;
#endif

	display->VirtualGet(screenID, region);

	// Make sure there is a valid string.
	if (text && text[0])													// BUG.1435
	{
		// Fill the region (if so specified).
		if (fillRegion)
			display->Rectangle(screenID, region, palette, 0, TRUE);

		// Draw the text.
		int textWidth = display->TextWidth(text, screenID, font);
		int textHeight = display->TextHeight(text, screenID, font);
		if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
			region.left = region.right - textWidth;
		else if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
			region.left = region.left + (region.Width() - textWidth) / 2;
		region.top += (region.Height() - textHeight) / 2;

		display->Text(screenID, region.left, region.top, text, palette, -1,	// BUG.1470
			FALSE, FALSE, NULL, font);
		region.bottom = region.top + textHeight - 1;
		region.right = region.left + textWidth - 1;
	}

	display->VirtualPut(screenID);
	return (TRUE);
}

EVENT_TYPE UI_WINDOW_OBJECT::Event(const UI_EVENT &event)
{
	static initializedTime = FALSE;
	static UI_TIME lastTime;
	if (!initializedTime)
	{
		lastTime.Import();
		initializedTime = TRUE;
	}
	UI_WINDOW_OBJECT *object;

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW_OBJECT);
	switch (ccode)
	{
	case S_INITIALIZE:
		if (!numberID && parent)
		{
			for (object = parent; object->parent; object = object->parent)
				;
			numberID = object->NumberID() ? object->NumberID() : 1;
			object->NumberID(numberID + 1);
			if (stringID[0] == '\0')
				sprintf(stringID, "FIELD_%d", numberID);
		}
		dwStyle |= parent ? WS_CHILD | WS_VISIBLE : WS_OVERLAPPED;
		RegionConvert(relative, (parent && !FlagSet(woFlags, WOF_NON_FIELD_REGION)) ? FALSE : TRUE);
		break;

	case S_DEINITIALIZE:
		screenID = 0;
		break;

	case S_REGISTER_OBJECT:
		RegisterObject("UI_WINDOW_OBJECT", "STATIC", &_objectOffset,
			&_objectJumpInstance, &_objectCallback);
		break;

	case S_SIZE:
	case S_CREATE:
		if (FlagSet(woStatus, WOS_INTERNAL_ACTION))
			break;
//		else if (ccode == S_CREATE && screenID &&							// BUG.General
//			!FlagSet(woFlags, WOF_SUPPORT_OBJECT) &&
//			(!parent || screenID != parent->screenID))
//		{
//			DestroyWindow(screenID);
//			Event(UI_EVENT(S_DEINITIALIZE, 0));
//		}
		else if (parent)
		{
			parent->RegionMax(this);
			if (parent->searchID == ID_GROUP)
			{
				true.top -= display->cellHeight / 4;
				true.bottom -= display->cellHeight / 4;
			}
		}
		else
			RegionMax(this);
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
		// Check the object status.
		if (FlagSet(woStatus, WOS_INTERNAL_ACTION))
		{
			lastPalette = LogicalPalette(ccode);
			break;
		}
		else if (ccode == S_CURRENT)
			woStatus |= WOS_CURRENT;
		else
			woStatus &= ~WOS_CURRENT;
		// Continue to S_DISPLAY_ACTIVE.
	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		lastPalette = LogicalPalette(ccode);
		if (FlagSet(woStatus, WOS_WINDOWS_ACTION) || !screenID || 
			(parent && screenID == parent->screenID))							// BUG.General
			;
		else if (ccode == S_CURRENT)
		{
			// Make sure the base window is current on the screen.
			for (UI_WINDOW_OBJECT *root = this; root->parent; root = root->parent)
				if (FlagSet(root->woAdvancedFlags, WOAF_TEMPORARY))
					break;
			if (root != windowManager->First())
				break;
			// Activate the current object.
			if (!parent)
			{
				woStatus |= WOS_WINDOWS_ACTION;
				SendMessage(screenID, WM_ACTIVATE, 1, 0);
				if (FlagSet(woFlags, WOF_AUTO_CLEAR))
					SendMessage(screenID, EM_SETSEL, 0, 0x7FFF0000L);
				SetFocus(screenID);
				woStatus &= ~WOS_WINDOWS_ACTION;
			}
			else if (FlagSet(woFlags, WOF_AUTO_CLEAR))						// BUG.1398
				SendMessage(screenID, EM_SETSEL, 0, 0x7FFF0000L);
		}
		else if (ccode == S_DISPLAY_ACTIVE || ccode == S_DISPLAY_INACTIVE)
		{
			SendMessage(screenID, WM_SETREDRAW, TRUE, 0);
			InvalidateRect(screenID, NULL, FALSE);
			ShowWindow(screenID, SW_SHOWNA);
			UpdateWindow(screenID);
		}

		// Call the associated user or validate function.
		if (FlagSet(woStatus, WOS_INTERNAL_ACTION) ||
			(ccode != S_CURRENT && ccode != S_NON_CURRENT))
			break;

		ccode = UserFunction(event, ccode);
		break;

	case S_ADD_OBJECT:
	case S_SUBTRACT_OBJECT:
		if (FlagSet(woStatus, WOS_WINDOWS_ACTION))
		{
			UI_EVENT event(ccode, 0);
			event.data = this;
			object = parent ? parent : windowManager;
			object->woStatus |= WOS_WINDOWS_ACTION;
			object->Event(event);
			object->woStatus &= ~WOS_WINDOWS_ACTION;
		}
		else if (parent)
			parent->Event(event);
		break;

	case S_REDISPLAY:
		if (screenID)
		{
			WOS_STATUS _woStatus = woStatus;
			woStatus |= WOS_REDISPLAY | WOS_INTERNAL_ACTION;	// Don't call user-function.
			SendMessage(screenID, WM_SETREDRAW, TRUE, 0);
			InvalidateRect(screenID, NULL, TRUE);
			ShowWindow(screenID, SW_SHOWNA);
			UpdateWindow(screenID);
			if (!FlagSet(_woStatus, WOS_INTERNAL_ACTION))
				woStatus &= ~WOS_INTERNAL_ACTION;
		}
		else if (parent)
			return (parent->Event(event));
		break;

	case L_HELP:
		if (parent && helpContext == NO_HELP_CONTEXT)
			return (S_UNKNOWN);
		else if (helpSystem)
			helpSystem->DisplayHelp(windowManager, helpContext);
		break;

	case L_END_SELECT:
	case L_SELECT:
		ccode = UserFunction(event, L_SELECT);
//		if (parent->Inherited(ID_LIST))										// BUG.1584
		if (parent && parent->Inherited(ID_LIST))							// BUG.????
			SendMessage(screenID, LB_SETSEL, FlagSet(woStatus, WOS_SELECTED), ListIndex());
		break;

	case L_PREVIOUS:
	case L_NEXT:
		if (event.type == E_MSWINDOWS)
			eventManager->Put(UI_EVENT(ccode, 0));
		else
			ccode = S_UNKNOWN;
		break;

	case E_KEY:
		return (CallWindowProc((FARPROC)defaultCallback, screenID, WM_CHAR, event.key.value, 0));

	default:
		ccode = S_UNKNOWN;
	}
	if (ccode != S_UNKNOWN || event.type != E_MSWINDOWS)
		return (ccode);
	else if (!windowManager)
		return (CallWindowProc((FARPROC)defaultCallback, screenID,
			event.message.message, event.message.wParam, event.message.lParam));
		
	// Switch on the windows message.
	ccode = TRUE;
	EVENT_TYPE winCode = event.type;
 	HWND hwnd = screenID;
	WORD message = event.message.message;
	WORD wParam = event.message.wParam;
	LONG lParam = event.message.lParam;
	switch (message)
	{
	case WM_NCDESTROY:
		ccode = CallWindowProc((FARPROC)defaultCallback, hwnd, message, wParam, lParam);
		if (!parent && !FlagSet(woStatus, WOS_INTERNAL_ACTION))				// BUG.1551
		{
			woStatus |= WOS_WINDOWS_ACTION;
			*windowManager - this;
			woStatus &= ~WOS_WINDOWS_ACTION;
		}
		screenID = 0;
		break;

	case WM_CLOSE:															// BUG.1582
		if (!parent && FlagSet(woAdvancedFlags, WOAF_TEMPORARY))				// BUG.????
			eventManager->Put(UI_EVENT(S_CLOSE_TEMPORARY));
		else if (!parent)
			eventManager->Put(UI_EVENT(S_CLOSE));
		else
			eventManager->Put(UI_EVENT(S_CLOSE + S_MDICHILD_EVENT));		// BUG.1576
		break;

	case WM_MEASUREITEM:
		{
		MEASUREITEMSTRUCT *item = (MEASUREITEMSTRUCT *)lParam;
		item->itemWidth = relative.Width();
		item->itemHeight = relative.Height();
		}
		break;

	case WM_PAINT:
		// See if the object draws its information.
		if (FlagSet(woStatus, WOS_OWNERDRAW))
		{
			winCode = FlagSet(woStatus, WOS_CURRENT) ? S_CURRENT : S_DISPLAY_ACTIVE;
			ccode = DrawItem(event, winCode);
		}
		else
			ccode = FALSE;
		if (!ccode)
			ccode = CallWindowProc((FARPROC)defaultCallback, hwnd, message, wParam, lParam);
		break;

	case WM_DRAWITEM:
		{
		DRAWITEMSTRUCT *draw = (DRAWITEMSTRUCT *)lParam;

		// Check the object's current status.
		if (parent)															// BUG.1346
		{
			WNF_FLAGS wnFlags = WNF_NO_FLAGS;
			parent->Information(GET_FLAGS, &wnFlags, ID_WINDOW);
			if (FlagsSet(wnFlags, WNF_AUTO_SELECT) && !FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
			{
				if (FlagSet(draw->itemState, ODS_SELECTED))
					woStatus |= WOS_SELECTED;
				else
					woStatus &= ~WOS_SELECTED;
			}
		}
		if (FlagSet(draw->itemState, ODS_FOCUS))
			woStatus |= WOS_CURRENT;
		else
			woStatus &= ~WOS_CURRENT;

		// Recalculate the object's region.
		UI_MSWINDOWS_DISPLAY::hDC = draw->hDC;
		if (parent->Inherited(ID_LIST))
		{
//			int width = relative.Width() - 1;								// BUG.???
			int height = relative.Height() - 1;
			true.left = draw->rcItem.left;
			true.top = draw->rcItem.top;
//			true.right = draw->rcItem.right;
			true.right = draw->rcItem.right - 1;
			true.bottom = true.top + height;
//			display->VirtualGet(ID_DIRECT, 0, 0, width, height);
			display->VirtualGet(ID_DIRECT, 0, 0, true.Width(), height);
		}
		else
			display->VirtualGet(ID_DIRECT, true);

		winCode = FlagSet(woStatus, WOS_CURRENT) ? S_CURRENT : S_DISPLAY_ACTIVE;
		ccode = DrawItem(event, winCode);
		display->VirtualPut(ID_DIRECT);
		UI_MSWINDOWS_DISPLAY::hDC = 0;

		// Check for invalid display object.
		if (!ccode)
			ccode = CallWindowProc((FARPROC)defaultCallback, hwnd, message, wParam, lParam);
		}
		break;

	case WM_NCLBUTTONDBLCLK:
	case WM_NCLBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
		if (!FlagSet(woStatus, WOS_EDIT_MODE))
		{
			if (!FlagSet(woFlags, WOF_NON_SELECTABLE))
				ccode = CallWindowProc((FARPROC)defaultCallback, hwnd, message, wParam, lParam);
			break;
		}
#if defined(ZIL_EDIT)
		{
		UI_TIME currentTime;
		UI_EVENT dEvent;
		UI_EVENT mEvent;
		mEvent = event;

		RECT rect;
		GetWindowRect(hwnd, &rect);
		if (message == WM_NCLBUTTONDBLCLK || message == WM_NCLBUTTONDOWN)
		{
			mEvent.position.column -= rect.left;
			mEvent.position.line -= rect.top;
		}
		else
		{
			POINT offset = { rect.left, rect.top };
			ScreenToClient(hwnd, &offset);
			mEvent.position.column -= offset.x;
			mEvent.position.line -= offset.y;
		}

		long elapsedTime = currentTime - lastTime;							// BUG.1228
		if (Abs(elapsedTime) < doubleClickRate)
		{
			dEvent.type = D_EDIT_OBJECT;
			dEvent.rawCode = searchID;
			dEvent.data = this;
			eventManager->Put(dEvent);
			break;
		}
		if (parent)
		{
			if (hwnd && GetFocus() != hwnd)
				SetFocus(hwnd);
			Modify(mEvent);
		}
		else
			ccode = CallWindowProc((FARPROC)defaultCallback, hwnd, message, wParam, lParam);

		// Set the new current object.
		dEvent.type = D_SET_OBJECT;
		dEvent.rawCode = searchID;
		dEvent.data = this;
		eventManager->Put(dEvent);
		dEvent.type = D_SET_POSITION;
		dEvent.rawCode = M_LEFT | M_LEFT_CHANGE;
		dEvent.position = mEvent.position;
		eventManager->Put(dEvent);
		lastTime = currentTime;
		}
#endif
		break;

#if defined(ZIL_EDIT)
	case WM_NCRBUTTONDBLCLK:
	case WM_NCRBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
		if (FlagSet(woStatus, WOS_EDIT_MODE))
		{
			UI_EVENT dEvent;
			dEvent.type = D_SET_POSITION;
			dEvent.rawCode = M_RIGHT | M_RIGHT_CHANGE;
			dEvent.position = event.position;
			eventManager->Put(dEvent);
			break;
		}
		else
			ccode = CallWindowProc((FARPROC)defaultCallback, hwnd, message, wParam, lParam);
		break;
#endif

	case WM_MOUSEACTIVATE:
//		if (FlagSet(woFlags, WOF_NON_SELECTABLE))							// BUG.1405
		if (FlagSet(woFlags, WOF_NON_SELECTABLE) || FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
			break;
		else if (!FlagSet(woStatus, WOS_WINDOWS_ACTION | WOS_CURRENT))
		{
			woStatus |= WOS_WINDOWS_ACTION;
			UI_WINDOW_OBJECT::Event(UI_EVENT(S_ADD_OBJECT, 0));
			woStatus &= ~WOS_WINDOWS_ACTION;
		}
		ccode = CallWindowProc((FARPROC)defaultCallback, hwnd, message, wParam, lParam);
		break;

#if defined(ZIL_EDIT)
	case WM_SETCURSOR:
		if (FlagSet(woStatus, WOS_EDIT_MODE) && parent)
		{
			SetCursor(LoadCursor(0, IDC_ARROW));
			ccode = TRUE;
		}
		else
			ccode = CallWindowProc((FARPROC)defaultCallback, hwnd, message, wParam, lParam);
		break;
#endif

	case WM_SETFOCUS:
		if (!FlagSet(woFlags, WOF_NON_SELECTABLE))
			ccode = CallWindowProc((FARPROC)defaultCallback, hwnd, message, wParam, lParam);
		break;

	case WM_SYSCHAR:
		if (parent && !Inherited(ID_WINDOW))
			ccode = parent->Event(event);
		else
			ccode = CallWindowProc((FARPROC)defaultCallback, hwnd, message, wParam, lParam);
		break;

	case WM_KEYUP:
		if (event.message.wParam == 0x1B)									// BUG.1445
		{
			eventManager->Put(UI_EVENT(S_CLOSE_TEMPORARY));
			break;
		}
		// else fall through to WM_SYSKEYUP.

	case WM_SYSKEYUP:
		if (helpSystem && UI_EVENT_MAP::MapEvent(eventMapTable, event, ID_WINDOW_MANAGER) == L_HELP)
		{
			helpSystem->DisplayHelp(windowManager, NO_HELP_CONTEXT);
			break;
		}
		ccode = CallWindowProc((FARPROC)defaultCallback, hwnd, message, wParam, lParam);
		break;

	default:
		ccode = CallWindowProc((FARPROC)defaultCallback, hwnd, message, wParam, lParam);
		break;
	}

	// Return the control code.
	return (ccode);
}

void UI_WINDOW_OBJECT::RegisterObject(char *name, char *baseName,
	int *offset, FARPROC *procInstance, FARPROC *defProcInstance,
	char *title, HMENU menu)
{
	// Make sure the field needs to be registered.
	if (screenID || FlagSet(woStatus, WOS_INTERNAL_ACTION))
		return;

	// Set up the object init procedure.
	static FARPROC _initInstance = NULL;
	if (!_initInstance)
		_initInstance = MakeProcInstance((FARPROC)ObjectInitProcedure, display->hInstance);

	// Register the windows class object.
	extern int ZincClassAdd(char *name, FARPROC farproc, HANDLE hInstance);
	if (*offset == -1 && !baseName)
	{
		WNDCLASS wc;
		*offset = (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && parent) ?
			sizeof(LOCALHANDLE) : 0;
		if (*defProcInstance && FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && parent)
			*defProcInstance = (FARPROC)DefMDIChildProc;
		else if (*defProcInstance)
			*defProcInstance = (FARPROC)DefWindowProc;
		else
			*defProcInstance = NULL;
		*procInstance = MakeProcInstance(*procInstance, display->hInstance);
		wc.style = 0;
		wc.lpfnWndProc = (DEFWINPROC)_initInstance;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = *offset + sizeof(UI_WINDOW_OBJECT *);
		wc.hInstance = display->hInstance;
		wc.hIcon = 0;
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		if (Inherited(ID_TOOL_BAR))
			wc.hbrBackground = COLOR_BTNFACE + 1;
		else if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT) && !parent)
			wc.hbrBackground = COLOR_APPWORKSPACE + 1;
		else
			wc.hbrBackground = COLOR_WINDOW + 1;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = name;
		RegisterClass(&wc);
		ZincClassAdd(name, *procInstance, wc.hInstance);
	}
	else if (*offset == -1)
	{
		WNDCLASS wc;
		GetClassInfo(0, baseName, &wc);
		*offset = wc.cbWndExtra;
		*defProcInstance = (FARPROC)wc.lpfnWndProc;
		*procInstance = MakeProcInstance(*procInstance, display->hInstance);
		wc.lpfnWndProc = (DEFWINPROC)_initInstance;
		wc.cbWndExtra += sizeof(UI_WINDOW_OBJECT *);
		wc.hInstance = display->hInstance;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = name;
		RegisterClass(&wc);
		ZincClassAdd(name, *procInstance, wc.hInstance);
	}

	// Get the class registration information and seed the init procedure.
	if (*defProcInstance)
		defaultCallback = *defProcInstance;
	_object = this;

	_offset = *offset;
	_controlID = numberID;
	_jumpInstance = *procInstance;

	// Create the object.
	woStatus |= WOS_INTERNAL_ACTION;
	DWORD exFlags = WS_EX_NOPARENTNOTIFY;
	if (FlagSet(woAdvancedFlags, WOAF_DIALOG_OBJECT))
		exFlags |= WS_EX_DLGMODALFRAME;
	if (!parent)
//		screenID = CreateWindowEx(exFlags, name, title, dwStyle,			// BUG.task
//			true.left, true.top, true.Width(), true.Height(),
//			0, menu, display->hInstance, NULL);
		screenID = CreateWindowEx(exFlags, name, title, dwStyle,
			true.left, true.top, true.Width(), true.Height(),
			windowManager->taskID, menu, display->hInstance, NULL);
	else if (parent->Inherited(ID_COMBO_BOX))
	{
		screenID = parent->screenID;										// BUG.1345
		DWORD index = SendMessage(screenID, CB_INSERTSTRING,
			(!next || !Next()->screenID) ? -1 : ListIndex(),
			FlagSet(parent->woStatus, WOS_OWNERDRAW) ? (LONG)this : (LONG)title);
		if (FlagSet(woStatus, WOS_SELECTED))
			SendMessage(screenID, CB_SETCURSEL, LOWORD(index), 0);
	}
	else if (parent->Inherited(ID_LIST))
	{
		screenID = parent->screenID;										// BUG.1345
		DWORD index = SendMessage(screenID, LB_INSERTSTRING,
			(!next || !Next()->screenID) ? -1 : ListIndex(),
			FlagSet(parent->woStatus, WOS_OWNERDRAW) ? (LONG)this : (LONG)title);
		if (FlagSet(woStatus, WOS_SELECTED))
			SendMessage(screenID, LB_SETSEL, TRUE, index);
	}
	else if (FlagSet(woAdvancedFlags, WOAF_MDI_OBJECT))
	{
		SCREENID parentMDI;
		parent->Information(GET_MDIHANDLE, &parentMDI);
		MDICREATESTRUCT mdiCreate;
		mdiCreate.szClass = name;
		mdiCreate.szTitle = title;
		mdiCreate.hOwner = display->hInstance;
		mdiCreate.style = dwStyle;
		mdiCreate.x = true.left;
		mdiCreate.y = true.top;
		mdiCreate.cx = true.right - true.left + 1;
		mdiCreate.cy = true.bottom - true.top + 1;
		mdiCreate.lParam = 0;
		screenID = LOWORD(SendMessage(parentMDI, WM_MDICREATE, 0, (LONG)&mdiCreate));
	}
	else
		screenID = CreateWindowEx(exFlags, name, title, dwStyle,
			true.left, true.top, true.Width(), true.Height(),
			parent->screenID, menu, display->hInstance, NULL);
	if (font != FNT_SYSTEM_FONT)
		SendMessage(screenID, WM_SETFONT, UI_MSWINDOWS_DISPLAY::fontTable[font], FALSE);
	woStatus &= ~WOS_INTERNAL_ACTION;
}

void UI_WINDOW_OBJECT::Modify(const UI_EVENT &event)
{
	// Make sure we can move the object.
	if (!parent || FlagSet(woFlags, WOF_NON_FIELD_REGION) ||
		(event.type == L_SIZE && Inherited(ID_ICON)))
		return;

	// Determine move / size operation.
	RECT sizeBoundary = { 5, 5, relative.right - relative.left - 5, relative.bottom - relative.top - 5 };
	RAW_CODE sizeFlags = 0;
	EVENT_TYPE operation = L_SIZE;
	if (event.type == L_MOVE || 
		(event.position.column >= sizeBoundary.left && event.position.line >= sizeBoundary.top &&
		event.position.column <= sizeBoundary.right && event.position.line <= sizeBoundary.bottom))
	{
		sizeFlags = M_LEFT_CHANGE | M_TOP_CHANGE | M_RIGHT_CHANGE | M_BOTTOM_CHANGE;
		operation = L_MOVE;
	}
	if (event.position.column < sizeBoundary.left)
		sizeFlags |= M_LEFT_CHANGE;
	else if (event.position.column > sizeBoundary.right)
		sizeFlags |= M_RIGHT_CHANGE;
	if (event.position.line < sizeBoundary.top)
		sizeFlags |= M_TOP_CHANGE;
	else if (event.position.line > sizeBoundary.bottom)
		sizeFlags |= M_BOTTOM_CHANGE;

	// Determine the minimum height and width of the object.
	int minHeight = display->cellHeight;
	if (parent && searchID != ID_WINDOW)
		minHeight -= (display->preSpace + display->postSpace);
	else
		minHeight *= 2;
	int minWidth = 2 * display->cellWidth;
	if (Inherited(ID_WINDOW) && !Inherited(ID_SCROLL_BAR)
		&& !Inherited(ID_LIST) && !Inherited(ID_GROUP))
		minWidth *= 9;

	// Determine the absolute region.
	UI_REGION absolute;
	if (parent)
	{
		RECT rect;
		GetClientRect(parent->screenID, &rect);
		absolute.Assign(rect);
	}
	else
	{
		absolute.left = absolute.top = 0;
		absolute.right = display->columns - 1;
		absolute.bottom = display->lines - 1;
	}

	// Adjust size flags if necessary.
	if (operation != L_MOVE)
	{
		if (Inherited(ID_BUTTON))
		{
			BTF_FLAGS btFlags;
			Information(GET_FLAGS, &btFlags, ID_BUTTON);
//			if (FlagSet(btFlags, BTF_AUTO_SIZE))							// BUG.GENERAL
			if (FlagSet(btFlags, BTF_AUTO_SIZE | BTF_RADIO_BUTTON | BTF_CHECK_BOX))
				sizeFlags &= ~(M_TOP_CHANGE | M_BOTTOM_CHANGE);
		}
//		else if (!Inherited(ID_WINDOW) || Inherited(ID_COMBO_BOX))			// BUG.7079
		else if ((!Inherited(ID_WINDOW) || Inherited(ID_COMBO_BOX)) && searchID < 3000)
			sizeFlags &= ~(M_TOP_CHANGE | M_BOTTOM_CHANGE);
		else if (Inherited(ID_SCROLL_BAR))
		{
			SBF_FLAGS sbFlags = SBF_NO_FLAGS;
			Information(GET_FLAGS, &sbFlags, ID_SCROLL_BAR);
			if (!FlagSet(sbFlags, SBF_VERTICAL))
			{
				minHeight = relative.bottom - relative.top + 1;
				sizeFlags &= ~(M_TOP_CHANGE | M_BOTTOM_CHANGE);
			}
			if (!FlagSet(sbFlags, SBF_HORIZONTAL))
			{
				minWidth = relative.right - relative.left + 1;
				sizeFlags &= ~(M_LEFT_CHANGE | M_RIGHT_CHANGE);
			}
		}
	}
	if (!sizeFlags)
		return;

	// Convert to client coordinates.
	UI_POSITION origin = event.position;
	RECT rect;
	GetWindowRect(screenID, &rect);
	POINT offset = { rect.left, rect.top };
	ScreenToClient(screenID, &offset);
	origin.column += offset.x;
	origin.line += offset.y;

	RECT newRegion = {true.left, true.top, true.right, true.bottom };
	RECT oldRegion = newRegion;
	int xJump = display->cellWidth;
//	int yJump = display->cellWidth;											// BUG.7124
	int yJump = display->cellHeight;
	SetCapture(screenID);
	HDC hDC = GetDC(parent->screenID);
	DrawFocusRect(hDC, &newRegion);
	WORD message;
	EVENT_TYPE ccode = 0;
	do
	{
		UI_EVENT event;
		eventManager->Get(event);
		message = event.message.message;
		int deltaX = 0, deltaY = 0;
		int update = FALSE;													// BUG.7124
		if (message == WM_LBUTTONDOWN || message == WM_NCLBUTTONDOWN ||
			(message == WM_MOUSEMOVE && FlagSet(event.message.wParam, MK_LBUTTON)))
		{
			// Check the absolute region.
			if (true.left + event.position.column > absolute.right)
				event.position.column = absolute.right - true.left;
			if (true.top + event.position.line > absolute.bottom)
				event.position.line = absolute.bottom - true.top;
			deltaX = event.position.column - origin.column;
			deltaY = event.position.line - origin.line;
			update = TRUE;							   						// BUG.7124
		}
		else if (message >= WM_KEYFIRST && message <= WM_KEYLAST)
		{
			deltaX = newRegion.right - true.right;							// BUG.7124
			deltaY = newRegion.bottom - true.bottom;
			ccode = LogicalEvent(event, ID_WINDOW_OBJECT);
			switch (ccode)
			{
			case L_UP:
			case L_DOWN:
//				deltaY = (ccode == L_UP) ? deltaY - yJump : deltaY + yJump; // BUG.7124
//				if (newRegion.top + deltaY < absolute.top)
//					deltaY = absolute.top - newRegion.top;
//				else if (newRegion.bottom + deltaY > absolute.bottom)
//					deltaY = absolute.bottom - newRegion.bottom;
				if (ccode == L_DOWN)
					deltaY += yJump;
				else
					deltaY -= yJump;
				update = TRUE;
				break;

			case L_LEFT:
			case L_RIGHT:
//				deltaX = (ccode == L_LEFT) ? deltaX - xJump : deltaX + xJump;
//				if (newRegion.left + deltaX < absolute.left)				// BUG.7124
//					deltaX = absolute.left - newRegion.left;
//				else if (newRegion.right + deltaX > absolute.right)
//					deltaX = absolute.right - newRegion.right;
				if (ccode == L_RIGHT)
					deltaX += xJump;
				else
					deltaX -= xJump;
				update = TRUE;
				break;

			default:
				if (event.key.value == 0x000D)		// ENTER
					ccode = L_SELECT;
				else if (event.key.value == 0x001B)	// ESCAPE
					ccode = L_CANCEL;
				break;
			}
		}
		// Send all user messages to the window manager.
		if (event.type > 9999)
			windowManager->Event(event);

		// Update the new region.
//		if (deltaX || deltaY)												// BUG.7124
		if (update)
		{
#if defined(ZIL_EDIT)
			if (FlagSet(woStatus, WOS_EDIT_MODE))
			{
				// Check the absolute region.
				if (FlagSet(sizeFlags, M_LEFT_CHANGE))
				{
					if (true.left + deltaX < absolute.left)
						deltaX = absolute.left - true.left;
				}
				if (FlagSet(sizeFlags, M_TOP_CHANGE))
				{
					if (true.top + deltaY < absolute.top)
						deltaY = absolute.top - true.top;
				}
			}

			long miniNX = display->miniNumeratorX, miniDX = display->miniDenominatorX;
			long miniNY = display->miniNumeratorY, miniDY = display->miniDenominatorY;

			// Check for a cell boundary move or size.
			if (FlagSet(woStatus, WOS_EDIT_MODE) && FlagSet(woFlags, WOF_MINICELL))
			{
				long value = deltaX;
				int ceil = (value >= 0) ? (int)(miniDX - 1) : int(1 - miniDX);
				value = (value * miniDX) / (miniNX * display->cellWidth);
				value = (value * miniNX * display->cellWidth + ceil) / miniDX;
				deltaX = (int)value;

				value = deltaY;
				ceil = (value >= 0) ? (int)(miniDY - 1) : (int)(1 - miniDY);
				value = (value * miniDY) / (miniNY * display->cellHeight);
				value = (value * miniNY * display->cellHeight + ceil) / miniDY;
				deltaY = (int)value;
			}
			else if (FlagSet(woStatus, WOS_EDIT_MODE))
			{
				int value = (deltaX > 0) ? deltaX + display->cellWidth / 2 : deltaX - display->cellWidth / 2;
				deltaX = value / display->cellWidth * display->cellWidth;
				value = (deltaY > 0) ? deltaY + display->cellHeight / 2 : deltaY - display->cellHeight / 2;
				deltaY = value / display->cellHeight * display->cellHeight;
			}
#endif

			if (FlagSet(sizeFlags, M_LEFT_CHANGE))
				newRegion.left = true.left + deltaX;
			if (FlagSet(sizeFlags, M_TOP_CHANGE))
				newRegion.top = true.top + deltaY;
			if (FlagSet(sizeFlags, M_RIGHT_CHANGE))
				newRegion.right = true.right + deltaX;
			if (FlagSet(sizeFlags, M_BOTTOM_CHANGE))
				newRegion.bottom = true.bottom + deltaY;

 			// Check for minimum size.
			if (operation == L_SIZE)
			{
				if (newRegion.left + minWidth - 1 > newRegion.right)
				{
					newRegion.left = oldRegion.left;
					newRegion.right = oldRegion.right;
				}
				if (newRegion.top + minHeight - 1 > newRegion.bottom)
				{
					newRegion.top = oldRegion.top;
					newRegion.bottom = oldRegion.bottom;
				}
			}

			// Move sizing rectangle.
			if (newRegion.left != oldRegion.left || newRegion.top != oldRegion.top ||
				newRegion.right != oldRegion.right || newRegion.bottom != oldRegion.bottom)
			{
				DrawFocusRect(hDC, &oldRegion);
				DrawFocusRect(hDC, &newRegion);
				oldRegion = newRegion;
			}
		}
	} while (message != WM_LBUTTONUP && ccode != L_SELECT && ccode != L_CANCEL);
	DrawFocusRect(hDC, &newRegion);
	ReleaseDC(parent->screenID, hDC);
	ReleaseCapture();

	if (ccode == L_CANCEL)
		return;				// Do not change the object region.

	if (newRegion.left != true.left || newRegion.right != true.right ||
		newRegion.top != true.top || newRegion.bottom != true.bottom)
	{
		// Size and redisplay the object.
		relative.left += newRegion.left - true.left;
		relative.top += newRegion.top - true.top;
		relative.right += newRegion.right - true.right;
		relative.bottom += newRegion.bottom - true.bottom;
		RECT rect = { true.left - 1, true.top - 1, true.right + 2, true.bottom + 2 }; 
		UI_WINDOW_OBJECT *root = this;										// BUG.1636
		while (root->parent)
			root = root->parent;
		root->Event(UI_EVENT(S_SIZE, 0));
		Event(UI_EVENT(S_SIZE, 0));
		WOS_STATUS _woStatus = woStatus;
		woStatus |= WOS_INTERNAL_ACTION;	// Don't call user-function.
		InvalidateRect(parent->screenID, &rect, TRUE);
		SetWindowPos(screenID, screenID, true.left, true.top,
			true.right - true.left + 1, true.bottom - true.top + 1,
			SWP_NOZORDER);
		UpdateWindow(parent->screenID);
		if (!FlagSet(_woStatus, WOS_INTERNAL_ACTION))
			woStatus &= ~WOS_INTERNAL_ACTION;
	}
}

