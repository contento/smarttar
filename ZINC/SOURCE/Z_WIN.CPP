//	Zinc Interface Library - Z_WIN.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define USE_RAW_KEYS			// Zinc raw keys.
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

//#if defined(__hpux)
//SCREENID UI_WINDOW_MANAGER::currentScreenID;	// Fix for HP compiler bug.
#if defined(ZIL_MSDOS)
SCREENID UI_WINDOW_MANAGER::currentScreenID = ID_SCREEN;
#elif defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(ZIL_MOTIF)
SCREENID UI_WINDOW_MANAGER::taskID = NULL;									// BUG.2316
#endif

#if defined(ZIL_MSWINDOWS)
static UI_WINDOW_MANAGER *_object = NULL;
#if defined(WIN32)
static int _offset = GWL_USERDATA;
#	define DS_FIXUP
#else
static int _offset = 0;
#	define DS_FIXUP _export
#endif

LRESULT CALLBACK DS_FIXUP TaskJumpProcedure(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	UI_WINDOW_MANAGER *object = (UI_WINDOW_MANAGER *)GetWindowLong(hWnd, _offset);
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}

LRESULT CALLBACK DS_FIXUP TaskInitProcedure(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	_object->Information(SET_TASKID, &hWnd);
	SetWindowLong(hWnd, _offset, (LONG)_object);
	SetWindowLong(hWnd, GWL_WNDPROC, (LONG)TaskJumpProcedure);
	UI_EVENT event(E_MSWINDOWS, hWnd, wMsg, wParam, lParam);
	return ((LRESULT)_object->Event(event));
}
#endif

// ----- UI_WINDOW_MANAGER --------------------------------------------------

UI_WINDOW_MANAGER::UI_WINDOW_MANAGER(UI_DISPLAY *_display, UI_EVENT_MANAGER *_eventManager,
	EVENT_TYPE (*_exitFunction)(UI_DISPLAY *display, UI_EVENT_MANAGER *eventManager, UI_WINDOW_MANAGER *windowManager)) :
	UIW_WINDOW(0, 0, _display->columns, _display->lines),
	exitFunction(_exitFunction)
{
#if defined(ZIL_LINKBUG)
	extern void z_win2_dummy(void);		// Bug fix for broken linkers.
	z_win2_dummy();
	extern void z_event_dummy(void);	// Bug fix for broken linkers.
	z_event_dummy();
	extern void z_pnorm_dummy(void);	// Bug fix for broken linkers.
	z_pnorm_dummy();
	extern void z_hotkey_dummy(void);	// Bug fix for broken linkers.
	z_hotkey_dummy();
#if defined(ZIL_PERSISTENCE)
	extern void z_jump_dummy(void);		// Bug fix for broken linkers.
	z_jump_dummy();
#endif
#endif

	// Initialize the window manager information.
	windowID[0] = ID_WINDOW_MANAGER;
	windowID[1] = ID_WINDOW;

	if (!_display->isText)
		woStatus |= WOS_GRAPHICS;
	screenID = ID_SCREEN;
	display = _display;
	eventManager = _eventManager;
	windowManager = this;

#if defined(ZIL_MSDOS)
	currentScreenID = ID_SCREEN;
#elif defined(ZIL_MSWINDOWS)
	// Set up the task name.
	WNDCLASS wc;
	wc.style = 0;
	wc.lpfnWndProc = TaskInitProcedure;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(UI_WINDOW_OBJECT *);
	wc.hInstance = display->hInstance;
	wc.hIcon = 0;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = (LPSTR) "ZINC_TASK_MANAGER";
	RegisterClass(&wc);

	char title[256];
	GetModuleFileName(display->hInstance, title, 256);
	char *taskName = title + ui_strlen(title);
	while (taskName != title && *taskName != '\\' && *taskName != ':')
		taskName--;
	taskName++;
	_object = this;
	CreateWindowEx(WS_EX_NOPARENTNOTIFY, "ZINC_TASK_MANAGER", taskName,
		0, -32767, -32767, 0, 0, NULL, NULL, display->hInstance, 0);
	ShowWindow(taskID, SW_SHOWNOACTIVATE);
	UpdateWindow(taskID);
#	if !defined(WIN32)
	// Initialize the class registration module.
	extern int ZincClassInitialize(HANDLE hInstance);
	ZincClassInitialize(display->hInstance);
#	endif
#elif defined(ZIL_OS2)
	taskID = 0;
#elif defined(ZIL_MOTIF)
	screenID = shell = taskID = display->topShell;
#endif
}

UI_WINDOW_MANAGER::~UI_WINDOW_MANAGER(void)
{
	UI_WINDOW_OBJECT *object;

	// Remove all objects from the window manager.
	for (object = First(); object; object = First())
	{
		UI_LIST::Subtract(object);
		if (object->screenID)
		{
#if defined(ZIL_MSWINDOWS)
			object->woStatus |= WOS_WINDOWS_ACTION;
			DestroyWindow(object->screenID);
#elif defined(ZIL_OS2)
			WinDestroyWindow(object->screenID);
#endif
		}
		object->Event(UI_EVENT(S_DEINITIALIZE, 0));
		if (!FlagSet(object->woAdvancedFlags, WOAF_NO_DESTROY))
			delete object;
	}

#if defined(ZIL_MSDOS)
	if (pasteBuffer)
		delete pasteBuffer;
	pasteBuffer = NULL;
	pasteLength = 0;
//#elif defined(ZIL_MSWINDOWS) && !defined(WIN32)
#elif defined(ZIL_MSWINDOWS)
	if (taskID)
		DestroyWindow(taskID);
#	if !defined(WIN32)
	extern int ZincClassRestore(HANDLE hInstance);
	ZincClassRestore(display->hInstance);
	PostQuitMessage(0);
#	endif
#endif

	windowManager = NULL;
	eventManager = NULL;
	display = NULL;
}

void UI_WINDOW_MANAGER::Center(UI_WINDOW_OBJECT *object)
{
	// Compute the actual screen size according to the window status.
	long columns = display->columns;
	long lines = display->lines;
	if (FlagSet(object->woStatus, WOS_GRAPHICS))
		;
	else if (FlagSet(object->woFlags, WOF_MINICELL))
	{
		columns = columns * display->miniNumeratorX / display->miniDenominatorX;
		lines = lines * display->miniNumeratorY / display->miniDenominatorY;
	}
	else
	{
		columns /= display->cellWidth;
		lines /= display->cellHeight;
	}

	// Compute the center location on the screen.
	int width = object->relative.Width();
	if (width > columns)													// BUG.1634
		width = columns;
	int height = object->relative.Height();
	if (height > lines)														// BUG.1634
		height = lines;

	// Update the window according to the center position.
	object->relative.left = (int)(columns - width) / 2;
	object->relative.top = (int)(lines - height) / 2;
	object->relative.right = object->relative.left + width - 1;
	object->relative.bottom = object->relative.top + height - 1;
	object->true = object->relative;

	// Re-display the window if it already is present on the screen.
	if (Index(object) != -1)
		UI_WINDOW_MANAGER::Add(object);
}

void *UI_WINDOW_MANAGER::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_WINDOW_MANAGER;
	switch (request)
	{
	case GET_NUMBERID_OBJECT:
	case GET_STRINGID_OBJECT:
		{
		void *match = NULL;
		for (UI_WINDOW_OBJECT *object = First(); object && !match; object = object->Next())
			match = object->UI_WINDOW_OBJECT::Information(request, data, objectID);
		data = match;
		}
		break;

#if defined(ZIL_MSWINDOWS)
	case SET_TASKID:
		taskID = *(SCREENID*)data;
		break;

	case SET_TEXT:
		SendMessage(taskID, WM_SETTEXT, 0, (LONG)data);
		break;

	case GET_TEXT:
		static char text[128];
		SendMessage(taskID, WM_GETTEXT, 128, (LONG)text);
		*(char **)data = text;
		break;

	case COPY_TEXT:
		SendMessage(taskID, WM_GETTEXT, 128, (LONG)data);
		break;
#elif defined(ZIL_OS2)
#endif

	default:
		return (NULL);
	}

	// Return the information.
	return (data);
}


