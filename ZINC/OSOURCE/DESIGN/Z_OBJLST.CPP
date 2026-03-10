//	Zinc Interface Library Designer - Z_LIST.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define INCL_WINLISTBOXES		// OS/2 list messages and flags.
#define USE_RAW_KEYS
#define UIF_CONTROL			OBJECT_LIST
#include <stdio.h>
#include <string.h>
#include "ui_dsn.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

#if defined(ZIL_MSWINDOWS)
static UI_EVENT_MAP _eventTable[] =
{
	{ ID_WINDOW_OBJECT, 	L_UP,				WM_KEYDOWN, 	GRAY_UP_ARROW },
	{ ID_WINDOW_OBJECT, 	L_UP,				WM_KEYDOWN, 	WHITE_UP_ARROW },
	{ ID_WINDOW_OBJECT, 	L_DOWN,				WM_KEYDOWN, 	GRAY_DOWN_ARROW },
	{ ID_WINDOW_OBJECT, 	L_DOWN,				WM_KEYDOWN, 	WHITE_DOWN_ARROW },
	{ ID_WINDOW_OBJECT, 	L_LEFT,				WM_KEYDOWN, 	GRAY_LEFT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_LEFT,				WM_KEYDOWN, 	WHITE_LEFT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,			WM_KEYDOWN, 	GRAY_RIGHT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,			WM_KEYDOWN, 	WHITE_RIGHT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_PREVIOUS,			WM_CHAR, 		BACKTAB },
	{ ID_WINDOW_OBJECT, 	L_NEXT,				WM_CHAR, 		TAB },
	{ ID_WINDOW_OBJECT, 	L_SELECT,			WM_CHAR, 		ENTER },
	{ ID_WINDOW_OBJECT,		L_HELP,				WM_KEYUP, 		F1 },
	{ ID_WINDOW_OBJECT, 	L_HELP,				WM_KEYUP, 		ALT_F1 },

	{ ID_LIST, 				L_SELECT,			WM_CHAR,		' ' },
	{ ID_LIST,	 			D_DELETE_OBJECT,	WM_KEYDOWN,		CTRL_WHITE_DELETE },
	{ ID_LIST, 				D_DELETE_OBJECT,	WM_KEYDOWN,		CTRL_GRAY_DELETE },
	{ ID_LIST,	 			D_DRAG_UP,			WM_KEYDOWN, 	CTRL_WHITE_UP_ARROW },
	{ ID_LIST, 				D_DRAG_UP,			WM_KEYDOWN, 	CTRL_GRAY_UP_ARROW },
	{ ID_LIST,	 			D_DRAG_DOWN,		WM_KEYDOWN, 	CTRL_WHITE_DOWN_ARROW },
	{ ID_LIST, 				D_DRAG_DOWN,		WM_KEYDOWN, 	CTRL_GRAY_DOWN_ARROW },
	// End of array.
	{ ID_END, 0, 0, 0 }
};
#elif defined(ZIL_OS2)
static UI_EVENT_MAP _eventTable[] =
{
	{ ID_WINDOW_OBJECT, 	L_EXIT,				WM_QUIT, 				0 },

	{ ID_WINDOW_OBJECT, 	L_BEGIN_SELECT,		WM_BUTTON1DOWN, 		0 },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_SELECT,		WM_BUTTON1DBLCLK, 		0 },
	{ ID_WINDOW_OBJECT, 	L_END_SELECT,		WM_BUTTON1UP, 			0 },

 	{ ID_LIST, 				L_SELECT,			WM_CHAR, 	SPACE,					KC_KEYUP },
	{ ID_LIST,	 			D_DELETE_OBJECT,	WM_CHAR,	CTRL_WHITE_DELETE,		0 },
	{ ID_LIST, 				D_DELETE_OBJECT,	WM_CHAR,	CTRL_GRAY_DELETE,		0 },
	{ ID_LIST,	 			D_DRAG_UP,			WM_CHAR, 	CTRL_WHITE_UP_ARROW,	0 },
	{ ID_LIST, 				D_DRAG_UP,			WM_CHAR, 	CTRL_GRAY_UP_ARROW,		0 },
	{ ID_LIST,	 			D_DRAG_DOWN,		WM_CHAR, 	CTRL_WHITE_DOWN_ARROW,	0 },
	{ ID_LIST, 				D_DRAG_DOWN,		WM_CHAR, 	CTRL_GRAY_DOWN_ARROW,	0 },

	// End of array.
	{ ID_END, 0, 0, 0 }
};
#elif defined(ZIL_MOTIF)
static UI_EVENT_MAP _eventTable[] =
{
	{ ID_WINDOW_OBJECT,		L_BEGIN_SELECT,		ButtonPress,	M_LEFT | M_LEFT_CHANGE },
	{ ID_WINDOW_OBJECT,		L_CONTINUE_SELECT,	MotionNotify,	M_LEFT },
	{ ID_WINDOW_OBJECT,		L_END_SELECT,		ButtonRelease,	M_LEFT_CHANGE },
	{ ID_WINDOW_OBJECT,		L_BEGIN_ESCAPE,		ButtonPress,	M_RIGHT | M_RIGHT_CHANGE },
	{ ID_WINDOW_OBJECT,		L_CONTINUE_ESCAPE,	MotionNotify,	M_RIGHT },
	{ ID_WINDOW_OBJECT,		L_END_ESCAPE,		ButtonRelease,	M_RIGHT_CHANGE },
	{ ID_WINDOW_OBJECT,		L_SELECT,			KeyPress,		XK_Return,
		0 },
	{ ID_WINDOW_OBJECT,		L_SELECT,			KeyPress,		XK_KP_Enter,
		0 },
	{ ID_WINDOW_OBJECT, 	L_CANCEL,			KeyPress, 		XK_Escape },
	{ ID_WINDOW_OBJECT,		S_SIZE,				ConfigureNotify,0 },
	{ ID_WINDOW_OBJECT,		L_HELP,				KeyPress, 		XK_F1 },
	{ ID_WINDOW_OBJECT,		L_HELP,				KeyPress, 		XK_F1, Mod1Mask },
#if defined(__hpux)
	{ ID_WINDOW_OBJECT, 	L_UP,				KeyPress, 		osfXK_Up },
	{ ID_WINDOW_OBJECT, 	L_DOWN,				KeyPress, 		osfXK_Down },
	{ ID_WINDOW_OBJECT, 	L_LEFT,				KeyPress, 		osfXK_Left },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,			KeyPress, 		osfXK_Right },
	{ ID_WINDOW_OBJECT, 	L_PGUP,				KeyPress, 		osfXK_PageUp },
	{ ID_WINDOW_OBJECT, 	L_PGDN,				KeyPress, 		osfXK_PageDown },
#endif
	{ ID_WINDOW_OBJECT, 	L_UP,				KeyPress, 		XK_Up },
	{ ID_WINDOW_OBJECT, 	L_DOWN,				KeyPress, 		XK_Down },
	{ ID_WINDOW_OBJECT, 	L_LEFT,				KeyPress, 		XK_Left },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,			KeyPress, 		XK_Right },
	{ ID_WINDOW_OBJECT, 	L_PGUP,				KeyPress, 		XK_Prior },
	{ ID_WINDOW_OBJECT, 	L_PGDN,				KeyPress, 		XK_Next },

	{ ID_BUTTON, 			L_SELECT,			KeyPress, 		XK_space },



#if defined(__hpux)
	{ ID_LIST,	 			D_DELETE_OBJECT,	KeyPress,	osfXK_Delete, ControlMask },
	{ ID_LIST,	 			D_DELETE_OBJECT,	KeyPress,	osfXK_BackSpace, ControlMask },
	{ ID_LIST,	 			D_DRAG_UP,			KeyPress, 	osfXK_Up, ControlMask },
	{ ID_LIST,	 			D_DRAG_DOWN,		KeyPress, 	osfXK_Down, ControlMask },
#endif
	{ ID_LIST,	 			D_DELETE_OBJECT,	KeyPress,	XK_Delete, ControlMask },
	{ ID_LIST,	 			D_DELETE_OBJECT,	KeyPress,	XK_BackSpace, ControlMask },
	{ ID_LIST,	 			D_DRAG_UP,			KeyPress, 	XK_Up, ControlMask },
	{ ID_LIST,	 			D_DRAG_DOWN,		KeyPress, 	XK_Down, ControlMask },

	// End of array.
	{ ID_END, 0, 0, 0 }
};
#elif defined(ZIL_MSDOS)
static UI_EVENT_MAP _eventTable[] =
{
	{ ID_WINDOW_OBJECT, 	L_SELECT,			E_KEY, 		ENTER },
	{ ID_WINDOW_OBJECT, 	L_SELECT,			E_KEY, 		GRAY_ENTER },
	{ ID_WINDOW_OBJECT, 	L_CANCEL,			E_KEY, 		ESCAPE },
	{ ID_WINDOW_OBJECT, 	L_VIEW,				E_MOUSE, 	0 },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_SELECT,		E_MOUSE, 	M_LEFT | M_LEFT_CHANGE },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_SELECT,		E_MOUSE, 	M_LEFT | M_RIGHT | M_LEFT_CHANGE | M_RIGHT_CHANGE },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_SELECT,	E_MOUSE, 	M_LEFT },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_SELECT,	E_MOUSE, 	M_LEFT | M_RIGHT },
	{ ID_WINDOW_OBJECT, 	L_END_SELECT,		E_MOUSE, 	M_LEFT_CHANGE },
	{ ID_WINDOW_OBJECT, 	L_END_SELECT,		E_MOUSE, 	M_RIGHT | M_LEFT_CHANGE },
	{ ID_WINDOW_OBJECT, 	L_UP,				E_KEY, 		GRAY_UP_ARROW },
	{ ID_WINDOW_OBJECT, 	L_UP,				E_KEY, 		WHITE_UP_ARROW },
	{ ID_WINDOW_OBJECT, 	L_DOWN,				E_KEY, 		GRAY_DOWN_ARROW },
	{ ID_WINDOW_OBJECT, 	L_DOWN,				E_KEY, 		WHITE_DOWN_ARROW },
	{ ID_WINDOW_OBJECT, 	L_LEFT,				E_KEY, 		GRAY_LEFT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_LEFT,				E_KEY, 		WHITE_LEFT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,			E_KEY, 		GRAY_RIGHT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,			E_KEY, 		WHITE_RIGHT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_PREVIOUS,			E_KEY, 		BACKTAB },
	{ ID_WINDOW_OBJECT, 	L_PREVIOUS,			E_KEY, 		SHIFT_F6 },
	{ ID_WINDOW_OBJECT, 	L_NEXT,				E_KEY, 		TAB },
	{ ID_WINDOW_OBJECT, 	L_NEXT,				E_KEY, 		F6 },
	{ ID_WINDOW_OBJECT,		L_HELP,				E_KEY, 		F1 },
	{ ID_WINDOW_OBJECT, 	L_HELP,				E_KEY, 		ALT_F1 },

	{ ID_WINDOW, 			L_PGUP,				E_KEY, 		WHITE_PGUP },
	{ ID_WINDOW, 			L_PGUP,				E_KEY, 		GRAY_PGUP },
	{ ID_WINDOW, 			L_PGDN,				E_KEY, 		WHITE_PGDN },
	{ ID_WINDOW, 			L_PGDN,				E_KEY, 		GRAY_PGDN },
	{ ID_WINDOW,			L_TOP,				E_KEY,	 	WHITE_HOME},
	{ ID_WINDOW,			L_TOP,				E_KEY,	 	GRAY_HOME},
	{ ID_WINDOW,			L_BOTTOM,			E_KEY,	 	WHITE_END},
	{ ID_WINDOW,			L_BOTTOM,			E_KEY,	 	GRAY_END},

	{ ID_LIST, 				L_SELECT,			E_KEY, 		SPACE },
	{ ID_LIST,	 			D_DELETE_OBJECT,	E_KEY, 		CTRL_WHITE_DELETE },
	{ ID_LIST, 				D_DELETE_OBJECT,	E_KEY, 		CTRL_GRAY_DELETE },
	{ ID_LIST, 				D_DELETE_OBJECT,	E_KEY, 		ALT_D },
	{ ID_LIST,	 			D_DRAG_UP,			E_KEY, 		CTRL_WHITE_UP_ARROW },
	{ ID_LIST, 				D_DRAG_UP,			E_KEY, 		CTRL_GRAY_UP_ARROW },
	{ ID_LIST, 				D_DRAG_UP,			E_KEY, 		ALT_A },
	{ ID_LIST,	 			D_DRAG_DOWN,		E_KEY, 		CTRL_WHITE_DOWN_ARROW },
	{ ID_LIST, 				D_DRAG_DOWN,		E_KEY, 		CTRL_GRAY_DOWN_ARROW },
	{ ID_LIST, 				D_DRAG_DOWN,		E_KEY, 		ALT_Z },
	// End of array.
	{ ID_END, 0, 0, 0 }
};
#endif

/* START BLOCK COMMENT // Only Persistent constructor needed.
**	OBJECT_LIST::OBJECT_LIST(int left, int top, int width, int height) :
**		UIW_VT_LIST(left, top, width, height, NULL, (FlagSet(_config->options,
**			OPTION_MENU_BITMAPS) ? WNF_BITMAP_CHILDREN : WNF_NO_FLAGS) |
**			WNF_NO_WRAP | WNF_SELECT_MULTIPLE)
**	{
**		searchID = windowID[0] = ID_OBJECT_LIST;
**		windowID[1] = ID_VT_LIST;
**		windowID[2] = ID_LIST;
**		windowID[3] = ID_WINDOW;
**		eventMapTable = _eventTable;
**		orderChanged = FALSE;
**	}
END BLOCK COMMENT */

EVENT_TYPE OBJECT_LIST::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = LogicalEvent(event, ID_LIST);
	switch (ccode)
	{
	case D_DRAG_UP:
	case D_DRAG_DOWN:
		{
		if (!First())
			break;
#if defined(ZIL_MSWINDOWS)
		if (SendMessage(screenID, LB_GETCURSEL, 0, 0) == LB_ERR)
			return (UIW_VT_LIST::Event(event));	// No items are selected yet.
#endif
		UI_WINDOW_OBJECT *dragObject = Current() ? Current() : First();
		UI_WINDOW_OBJECT *referenceObject;

		if (ccode == D_DRAG_UP && dragObject->Previous())
		{
			referenceObject = dragObject->Previous();
			UI_LIST::Subtract(dragObject);
			UI_LIST::Add(referenceObject, dragObject);
		}
		else if (ccode == D_DRAG_DOWN && dragObject->Next())
		{
			referenceObject = dragObject->Next();
			UI_LIST::Subtract(dragObject);
			UI_LIST::Add(referenceObject->Next(), dragObject);
		}
		else
			break;

		orderChanged = TRUE;
		UI_REGION tRegion;
		UI_REGION rRegion;
		if (ccode == D_DRAG_UP)
		{
			tRegion = dragObject->true;
			rRegion = dragObject->relative;
			dragObject->true = dragObject->Next()->true;
			dragObject->relative = dragObject->Next()->relative;
			dragObject->Next()->true = tRegion;
			dragObject->Next()->relative = rRegion;
			dragObject->woStatus &= ~WOS_CURRENT;
			dragObject->Next()->woStatus |= WOS_CURRENT;
		}
		else // D_DRAG_DOWN
		{
			tRegion = dragObject->true;
			rRegion = dragObject->relative;
			dragObject->true = dragObject->Previous()->true;
			dragObject->relative = dragObject->Previous()->relative;
			dragObject->Previous()->true = tRegion;
			dragObject->Previous()->relative = rRegion;
			dragObject->woStatus &= ~WOS_CURRENT;
			dragObject->Previous()->woStatus |= WOS_CURRENT;
		}

#if defined(ZIL_MSWINDOWS)
		SendMessage(screenID, LB_SETITEMDATA, UI_LIST::Index(referenceObject), (LONG)referenceObject);
		SendMessage(screenID, LB_SETITEMDATA, UI_LIST::Index(dragObject), (LONG)dragObject);
		UIW_VT_LIST::Event(event);
#elif defined(ZIL_OS2)
		WinSendMsg(screenID, LM_SETITEMHANDLE, (MPARAM)UI_LIST::Index(referenceObject), referenceObject);
		WinSendMsg(screenID, LM_SETITEMHANDLE, (MPARAM)UI_LIST::Index(dragObject), dragObject);

		// Scroll the list.
		current = dragObject;
		int topIndex = LOWORD(WinSendMsg(screenID, LM_QUERYTOPINDEX, (MPARAM)0, (MPARAM)0));
		if (ccode == D_DRAG_UP && topIndex >= Index(dragObject))
			WinSendMsg(screenID, LM_SETTOPINDEX, (MPARAM)(short)(topIndex - 1), (MPARAM)0);
		else if (ccode == D_DRAG_DOWN && topIndex < Index(dragObject) - 1)
			WinSendMsg(screenID, LM_SETTOPINDEX, (MPARAM)(short)(topIndex + 1), (MPARAM)0);

		// Make the drag object current.
		WinSendMsg(screenID, LM_SELECTITEM, (MPARAM)Index(dragObject), (MPARAM)TRUE);
		WinSendMsg(screenID, LM_SELECTITEM, (MPARAM)Index(dragObject), (MPARAM)FALSE);
		UI_WINDOW_OBJECT::Event(UI_EVENT(S_REDISPLAY, 0));
#elif defined(ZIL_MSDOS) || defined(ZIL_MOTIF)
		// Force the vertical list to handle scrolling.
		if (ccode == D_DRAG_UP)
		{
			current = dragObject->Next();		
			UIW_VT_LIST::Event(UI_EVENT(L_UP));
		}
		else // D_DRAG_DOWN
		{
			current = dragObject->Previous();
			UIW_VT_LIST::Event(UI_EVENT(L_DOWN));
		}
		UIW_VT_LIST::Event(UI_EVENT(S_CURRENT, 0, true));
#if defined(ZIL_MOTIF)
		Information(CHANGED_FLAGS, NULL);
		Add(dragObject);
#endif
#endif
		}
		break;

	case D_DELETE_OBJECT:
		if (current)
		{
			UI_EVENT event;
			UI_WINDOW_OBJECT *deleteObject = Current();
			UI_WINDOW_OBJECT *addObject = deleteObject->Next() ?
				deleteObject->Next() : deleteObject->Previous();
			Subtract(deleteObject);
			deleteList.Add(deleteObject);
			if (addObject)
			{
				event.type = S_ADD_OBJECT;
				event.data = addObject;
				UIW_VT_LIST::Event(event);
			}
			event.type = S_REDISPLAY;
			UIW_VT_LIST::Event(event);
		}
		break;

	default:
		ccode = UIW_VT_LIST::Event(event);
	}
	return ccode;
}

EVENT_TYPE OBJECT_LIST::SelectChild(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	if (ccode == L_DOUBLE_CLICK || object->parent->LogicalEvent(event) == L_SELECT)	// BUG.????
	{											
		char frameName[32];
		OBJECTID searchID = ((UI_WINDOW_OBJECT *)object->userObject)->windowID[0];
		if (searchID == ID_WINDOW_OBJECT)
			searchID = ID_DERIVE_OBJECT;
		sprintf(frameName, "UIF_%d", searchID);
		if (display->isText)
			strcat(frameName, "_TEXT");
		defaultStorage->ChDir("~UIW_WINDOW");
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		if (defaultStorage->FindFirstObject(frameName))
			(void *)&(*windowManager + new UIW_FRAME_WINDOW(frameName, (UI_WINDOW_OBJECT *)object->userObject, object));
		else
			eventManager->DeviceState(E_MOUSE, DM_VIEW);
	}
	return ccode;
}

void OBJECT_LIST::GetObjects(UIW_WINDOW *window)
{
	UI_WINDOW_OBJECT *object;
	for (object = (UI_WINDOW_OBJECT *)window->support.First(); object; object = object->Next())
	{
		if (object->Inherited(ID_PULL_DOWN_MENU))
		{
			object->userObject = userObject;
			char bitmapName[32];
			sprintf(bitmapName, "Z_%d", object->SearchID());
 			char *text = object->StringID();
			if (!*text)
				text = "New Item";
#if defined(__WATCOMC__)
			char *bName;
			if (FlagSet(_config->options, OPTION_MENU_BITMAPS))
				bName = bitmapName;
			else
				bName = NULL;
			UIW_BUTTON *button = new UIW_BUTTON(0, 0, 28, text,
				BTF_NO_3D | BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_DOUBLE_CLICK, WOF_NO_FLAGS,
				OBJECT_LIST::SelectChild, 0, bName);
#else
			UIW_BUTTON *button = new UIW_BUTTON(0, 0, 28, text,
				BTF_NO_3D | BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_DOUBLE_CLICK, WOF_NO_FLAGS,
				OBJECT_LIST::SelectChild, 0, (FlagSet(_config->options, OPTION_MENU_BITMAPS)) ?
				bitmapName : NULL);
#endif
			button->userObject = object;
			(void *)&(*this + button);
		}
	}
	for (object = window->First(); object; object = object->Next())
	{
		object->userObject = userObject;
		char bitmapName[32];
		OBJECTID id = object->windowID[0];
		if (id >= 3000)
			id = 18;
		sprintf(bitmapName, "Z_%d", id);
		char *text = object->StringID();
		if (!*text)
			text = "New Item";
#if defined(__WATCOMC__)
		char *bName;
		if (FlagSet(_config->options, OPTION_MENU_BITMAPS))
			bName = bitmapName;
		else
			bName = NULL;
		UIW_BUTTON *button = new UIW_BUTTON(0, 0, 28, text,
			BTF_NO_3D | BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_DOUBLE_CLICK, WOF_NO_FLAGS,
			OBJECT_LIST::SelectChild, 0, bName);
#else
		UIW_BUTTON *button = new UIW_BUTTON(0, 0, 28, text,
			BTF_NO_3D | BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_DOUBLE_CLICK, WOF_NO_FLAGS,
			OBJECT_LIST::SelectChild, 0, (FlagSet(_config->options, OPTION_MENU_BITMAPS)) ?
			bitmapName : NULL);
#endif
		button->userObject = object;
		(void *)&(*this + button);
	}
}

void OBJECT_LIST::SaveObjects(UIW_WINDOW *window)
{
	if (orderChanged)
	{
		while (window->First())
			(void *)&(*window - window->First());
		for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
		{
			UI_WINDOW_OBJECT *addObject = (UI_WINDOW_OBJECT *)object->userObject;
			(void *)&(*window + addObject);
			if (!addObject->NumberID())
			{
				UI_WINDOW_OBJECT *root = (UI_WINDOW_OBJECT *)addObject->userObject;
				if (!root)
					errorSystem->Beep();
				addObject->NumberID(root->NumberID() ? root->NumberID() : 1);
				root->NumberID(addObject->numberID + 1);
				if (addObject->stringID[0] == '\0')
					sprintf(addObject->stringID, "FIELD_%d", addObject->NumberID());
			}
		}
		orderChanged = FALSE;
	}
	for (UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)deleteList.First();
		object; object = object->Next())
	{
		(void *)&(*window - (UI_WINDOW_OBJECT *)object->userObject);
		delete object->userObject;
	}
}

void OBJECT_LIST::NewObject(UI_WINDOW_OBJECT *object)
{
	object->userObject = userObject;
	orderChanged = TRUE;
	char bitmapName[13];
	sprintf(bitmapName, "Z_%d", object->SearchID());
#if defined(__WATCOMC__)
	char *bName;
	if (FlagSet(_config->options, OPTION_MENU_BITMAPS))
		bName = bitmapName;
	else
		bName = NULL;
	UIW_BUTTON *button = new UIW_BUTTON(0, 0, 28, "New Item",
		BTF_DOUBLE_CLICK | BTF_NO_3D | BTF_NO_TOGGLE | BTF_AUTO_SIZE,
		WOF_NO_FLAGS, OBJECT_LIST::SelectChild, 0, bName);
#else
	UIW_BUTTON *button = new UIW_BUTTON(0, 0, 28, "New Item",
		BTF_DOUBLE_CLICK | BTF_NO_3D | BTF_NO_TOGGLE | BTF_AUTO_SIZE,
		WOF_NO_FLAGS, OBJECT_LIST::SelectChild, 0,
		(FlagSet(_config->options, OPTION_MENU_BITMAPS)) ? bitmapName : NULL);
#endif
	button->userObject = object;
	(void *)&(*this + button);
	UIW_VT_LIST::Event(UI_EVENT(S_REDISPLAY, 0));
}


OBJECT_LIST::OBJECT_LIST(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	UIW_VT_LIST(name, directory, file)
{
	searchID = windowID[0] = ID_OBJECT_LIST;
	windowID[1] = ID_VT_LIST;
	windowID[2] = ID_LIST;
	windowID[3] = ID_WINDOW;
	eventMapTable = _eventTable;	
	orderChanged = FALSE;
}

