//	Zinc Interface Library - Z_COMBO.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define INCL_WINLISTBOXES		// OS/2 list messages and flags.
#define INCL_WINENTRYFIELDS		// OS/2
#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#elif defined(ZIL_MOTIF)
#	include <Xm/Label.h>
#	include <Xm/RowColumn.h>
#endif

// ----- UIW_COMBO_BOX ------------------------------------------------------

UIW_COMBO_BOX::UIW_COMBO_BOX(int left, int top, int width, int height,
	int (*_compareFunction)(void *element1, void *element2),
	WNF_FLAGS _wnFlags, WOF_FLAGS _woFlags, WOAF_FLAGS _woAdvancedFlags) :
//	UIW_WINDOW(left, top, width, height, _woFlags, _woAdvancedFlags),		// BUG.1493
	UIW_WINDOW(left, top, width, 1, _woFlags, _woAdvancedFlags),
	list(0, 0, width, height, _compareFunction, _wnFlags | WNF_AUTO_SELECT,
//	WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY), text(NULL)				// BUG.1563
	_woFlags | WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY), text(NULL)
{
	// Initialize the combo-box information.
	wnFlags = _wnFlags | WNF_AUTO_SELECT;
	UIW_COMBO_BOX::Information(INITIALIZE_CLASS, NULL);
}

UIW_COMBO_BOX::UIW_COMBO_BOX(int left, int top, int width, int height,
	int (*_compareFunction)(void *element1, void *element2), 
	WOF_FLAGS flagSetting, UI_ITEM *item) :
//	UIW_WINDOW(left, top, width, height, WOF_NO_FLAGS, WOAF_NO_FLAGS),		// BUG.1493
	UIW_WINDOW(left, top, width, 1, WOF_NO_FLAGS, WOAF_NO_FLAGS),
	list(0, 0, width, height, _compareFunction, WNF_NO_WRAP | WNF_AUTO_SELECT,
	WOF_BORDER, WOAF_TEMPORARY | WOAF_NO_DESTROY), text(NULL)
{
	// Initialize the combo-box information.
	wnFlags |= WNF_NO_WRAP | WNF_AUTO_SELECT;
	UIW_COMBO_BOX::Information(INITIALIZE_CLASS, NULL);

	// Add the string items.
	for (int i = 0; item[i].text; i++)
	{
		UIW_STRING *string = new UIW_STRING(0, 0, width, item[i].text, -1,
			item[i].flags, WOF_BORDER | WOF_AUTO_CLEAR,
			(USER_FUNCTION)item[i].data);
		if (FlagSet(flagSetting, item[i].value))
			string->woStatus |= WOS_SELECTED;
		list.Add(string);
	}
}

UIW_COMBO_BOX::~UIW_COMBO_BOX(void)
{
	if (text)
		delete text;
}

void UIW_COMBO_BOX::Destroy(void)
{
	// Destroy the list elements (low level).
	list.Destroy();
}

void *UIW_COMBO_BOX::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_COMBO_BOX;
	switch (request)
	{
	case INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = windowID[0] = list.windowID[0] = ID_COMBO_BOX;
		windowID[1] = list.windowID[1] = ID_VT_LIST;
		windowID[2] = list.windowID[2] = ID_LIST;
		windowID[3] = list.windowID[3] = ID_WINDOW;
		font = FNT_DIALOG_FONT;
		list.parent = this;
		list.wnFlags |= WNF_AUTO_SELECT;

#if defined(ZIL_MSDOS)
		// Add the combo fields.
		button = new UIW_COMBO_BUTTON;
		UIW_WINDOW::Add(button);
		string = new UIW_COMBO_STRING;
		UIW_WINDOW::Add(string);
		if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN) || FlagSet(woFlags, WOF_VIEW_ONLY))
			string->woFlags |= WOF_VIEW_ONLY;
#endif
		// Continue to CHANGED_FLAGS.

	case CHANGED_FLAGS:
		// Check the flag settings.
		if (request == CHANGED_FLAGS)
			UIW_WINDOW::Information(CHANGED_FLAGS, data, ID_COMBO_BOX);
		if (compareFunction)
			list.compareFunction = compareFunction;
		if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
		{
			woStatus |= WOS_OWNERDRAW;
			list.woStatus |= WOS_OWNERDRAW;
		}
		else
		{
			woStatus &= ~WOS_OWNERDRAW;
			list.woStatus &= ~WOS_OWNERDRAW;
		}

		// Check the environment specific flag settings.
#if defined(ZIL_MSDOS)
		woFlags &= ~WOF_BORDER;
#elif defined(ZIL_MSWINDOWS)
		if (compareFunction)
			dwStyle |= CBS_SORT;
		if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
			dwStyle |= CBS_DROPDOWNLIST | CBS_OWNERDRAWVARIABLE;
		else if (FlagSet(woFlags, WOF_VIEW_ONLY))
			dwStyle |= CBS_DROPDOWNLIST;
		else
			dwStyle |= CBS_DROPDOWN | CBS_AUTOHSCROLL;
#elif defined(ZIL_OS2)
		if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
		{
			flFlag |= FCF_BORDER;
			list.Information(CHANGED_FLAGS, NULL);
		}
		else if (FlagSet(woFlags, WOF_VIEW_ONLY))
			flStyle |= CBS_DROPDOWNLIST;
		else
			flStyle |= CBS_DROPDOWN;
#endif

		// See if the field needs to be re-computed.
		if (objectID == ID_COMBO_BOX && FlagSet(woStatus, WOS_REDISPLAY))
		{
			UI_EVENT event(S_INITIALIZE, 0);
			Event(event);
			event.type = S_CREATE;
			Event(event);
		}
		break;

	case GET_TEXT:
	case COPY_TEXT:
		if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN) || request == GET_BITMAP_ARRAY)
			return (list.Information(request, data, objectID));

		if (!text)
			text = new char[256];
#if defined(ZIL_MSDOS)
		string->Information(COPY_TEXT, text, objectID);
#elif defined(ZIL_MSWINDOWS)
		if (screenID)
			SendMessage(screenID, WM_GETTEXT, 255, (LONG)text);
#elif defined(ZIL_OS2)
		if (screenID)
			WinQueryWindowText(screenID, 255, (PSZ)text);
#elif defined(ZIL_MOTIF)
		if (screenID)
		{
			XmString xms;
			XtVaGetValues(XmOptionButtonGadget(screenID),
				XmNlabelString, &xms, NULL);
			char *tmpText;
			XmStringGetLtoR(xms, XmSTRING_DEFAULT_CHARSET, &tmpText);
			strcpy(text, tmpText);
			XmStringFree(xms);
		}
#endif
		if (!data)
			return (text);
		if (request == GET_TEXT)
			*(char **)data = text;
		else
			data = strcpy((char *)data, text);
		break;

	case SET_TEXT:
	case SET_BITMAP_ARRAY:
		if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN) || request == SET_BITMAP_ARRAY)
			return (list.Information(request, data, objectID));

		if (!text)
			text = new char[256];
		strcpy(text, data ? (char *)data : "");
#if defined(ZIL_MSDOS)
		string->Information(request, data, objectID);
#elif defined(ZIL_MSWINDOWS)
		if (screenID)
		{
			InvalidateRect(screenID, NULL, TRUE);
			SendMessage(screenID, WM_SETTEXT, 0, (LONG)text);
		}
#elif defined(ZIL_OS2)
		if (screenID)
			WinSetWindowText(screenID, (PSZ)text);
#elif defined(ZIL_MOTIF)
		if (screenID)
		{
			XmString labelText =
				XmStringCreateLtoR(text, XmSTRING_DEFAULT_CHARSET);
			XtVaSetValues(XmOptionButtonGadget(screenID), XmNlabelString,
				labelText, NULL);
			if (labelText)
				XmStringFree(labelText);
		}
#endif
		break;

#if defined(ZIL_MSDOS)
	case RESET_SELECTION:
		if (FlagSet(list.woStatus, WOS_INTERNAL_ACTION))
			break;
		else if (!list.Current())
			string->DataSet("");
		else if (!FlagSet(wnFlags, WNF_BITMAP_CHILDREN) || display->isText)
			string->DataSet((char *)list.Current()->Information(GET_TEXT, NULL));
		else
		{
			UI_WINDOW_OBJECT *object = list.Current();
			string->userObject = object;
			UI_REGION _clip = object->clip;
			UI_REGION _true = object->true;
			SCREENID _screenID = object->screenID;
			WOF_FLAGS _woFlags = object->woFlags;
			WOS_STATUS _woStatus = object->woStatus;
			UI_WINDOW_OBJECT *_parent = object->parent;
			object->clip = string->clip;
			object->true = string->true;
			object->screenID = string->screenID;
			object->woFlags = (string->woFlags & ~(WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT)) |
				(_woFlags & (WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT));
			object->woStatus = string->woStatus;
//			object->parent = FlagSet(woStatus, WOS_CURRENT) ? this : object->parent;
			if (FlagSet(woStatus, WOS_CURRENT))
				object->parent = this;
			object->Event(UI_EVENT(S_REDISPLAY));
			object->clip = _clip;
			object->true = _true;
			object->screenID = _screenID;
			object->woFlags = _woFlags;
			object->woStatus = _woStatus;
			object->parent = _parent;
		}
		break;
#elif defined(ZIL_OS2)
	case RESET_SELECTION:
		if (screenID && list.screenID && FlagSet(list.flStyle, WS_VISIBLE))
		{
			if (data == this)
			{
				list.flStyle &= ~WS_VISIBLE;
				WinShowWindow(list.screenID, FALSE);
				WinInvalidateRect(screenID, NULL, TRUE);
				WinSetFocus(HWND_DESKTOP, screenID);
			}
			else
			{
				RECTL os2Region;
				os2Region.xLeft = os2Region.yBottom = 0;
				os2Region.yTop = true.bottom - true.top;
				os2Region.xRight = true.right - true.left - WinQuerySysValue(HWND_DESKTOP, SV_CXVSCROLL);
				WinInvalidateRect(screenID, &os2Region, TRUE);
			}
		}
		break;
#endif

	case DESTROY_LIST:
		Destroy();
		break;

	case GET_NUMBERID_OBJECT:
	case GET_STRINGID_OBJECT:
		{
		void *match = UI_WINDOW_OBJECT::Information(request, data, objectID);
		if (!match)
			match = list.Information(request, data, objectID);
		data = match;
		}
		break;

	case PRINT_INFORMATION:
	case PRINT_USER_FUNCTION:
		UI_WINDOW_OBJECT::Information(request, data, objectID);
		list.Information(request, data, objectID);
		break;

	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_PERSISTENCE)
UIW_COMBO_BOX::UIW_COMBO_BOX(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
//	UIW_WINDOW(0, 0, 20, 5, WOF_NO_FLAGS),									// BUG.1493
	UIW_WINDOW(0, 0, 20, 1, WOF_NO_FLAGS),
	list(0, 0, 20, 5, NULL, WNF_NO_WRAP | WNF_AUTO_SELECT, WOF_BORDER,
		WOAF_TEMPORARY | WOAF_NO_DESTROY),
	text(NULL)
{
	// Initialize the text information.
	Load(name, directory, file);
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
	UIW_WINDOW::Information(INITIALIZE_CLASS, NULL);
	UIW_COMBO_BOX::Information(INITIALIZE_CLASS, NULL);
}

void UIW_COMBO_BOX::Load(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Load the combo-box information.
	UI_WINDOW_OBJECT::Load(name, directory, file);
	list.Load(NULL, directory, file);
	relative.bottom -= list.relative.Height();								// BUG.1673
	wnFlags |= list.wnFlags;
}

void UIW_COMBO_BOX::Store(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Store the combo-box information.
//#if defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(ZIL_MOTIF)		// BUG.1493
//	UI_WINDOW_OBJECT::Store(name, directory, file);
//#elif defined(ZIL_MSDOS)
	UI_REGION region = relative;
	relative.bottom += list.relative.Height();								// BUG.1673
//	if (!display->isText && FlagSet(woStatus, WOS_GRAPHICS))
//		relative.bottom += 2 * display->preSpace + 2 * display->postSpace;
	UI_WINDOW_OBJECT::Store(name, directory, file);
	relative = region;
//#endif																	// BUG.1493
	list.Store(NULL, directory, file);
}
#endif


