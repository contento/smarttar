//	Zinc Interface Library - Z_BUTTON.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define OEMRESOURCE				// Windows button messages and flags.
#define INCL_WINBUTTONS			// OS/2 button messages and flags.
#define INCL_WINLISTBOXES		// OS/2 list messages and flags.
#define INCL_WINMENUS			// OS/2 menu messages and flags.
#include <ctype.h>
#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif
#if defined(ZIL_MOTIF)
#	include <Xm/ToggleB.h>
#endif

// ----- UIW_BUTTON ---------------------------------------------------------

UIW_BUTTON::UIW_BUTTON(int left, int top, int width, char *_text,
	BTF_FLAGS _btFlags, WOF_FLAGS _woFlags, USER_FUNCTION _userFunction,
	EVENT_TYPE _value, char *_bitmapName) :
	UI_WINDOW_OBJECT(left, top, width, 1, _woFlags, WOAF_NO_FLAGS),
	text(NULL), btFlags(_btFlags), value(_value), depth(2),
	btStatus(BTS_NO_STATUS), bitmapWidth(0), bitmapHeight(0),
	bitmapArray(NULL), colorBitmap(0), monoBitmap(0)
{
	// Initialize the button information.
#if defined(ZIL_MOTIF)
	pixmap = 0;
#endif
	bitmapName = ui_strdup(_bitmapName);
	userFunction = _userFunction;
	UIW_BUTTON::Information(INITIALIZE_CLASS, NULL);
	UIW_BUTTON::DataSet(_text);
}

UIW_BUTTON::~UIW_BUTTON(void)
{
	if (text && !FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete text;
	if (bitmapName)
		delete bitmapName;
	if (bitmapArray && !FlagSet(btFlags, BTF_STATIC_BITMAPARRAY))
		delete bitmapArray;

	// Free the color and monochrome bitmap handles.
#if defined(ZIL_MSDOS)
	if (colorBitmap)
		delete colorBitmap;
	if (monoBitmap)
		delete monoBitmap;
#elif defined(ZIL_MSWINDOWS)
	if (colorBitmap)
		DeleteObject(colorBitmap);
	if (monoBitmap)
		DeleteObject(monoBitmap);
#elif defined(ZIL_OS2)
	if (colorBitmap)
		GpiDeleteBitmap(colorBitmap);
	if (monoBitmap)
		GpiDeleteBitmap(monoBitmap);
#elif defined(ZIL_MOTIF)
	if (pixmap)
		XFreePixmap(display->xDisplay, pixmap);
#endif
}

char *UIW_BUTTON::DataGet(int stripText)
{
	// Check for normal DataGet.
	if (!text || !stripText)
		return (text);

	// Strip the button text information.
	static char sText[128];
	int offset = 0;
	while (text[offset] == ' ')							// Check for beginning spaces.
		offset++;
	strcpy(sText, &text[offset]);
	offset = strlen(sText) - 1;

	while (offset >= 0 && sText[offset] == ' ')			// Check for trailing spaces.
		offset--;
	sText[offset + 1] = '\0';

	char *hotChar = sText;								// Check for underline or '&' character.
	while ((hotChar = strchr(hotChar, '&')) != NULL)
	{
		memcpy(hotChar, hotChar + 1, ui_strlen(hotChar));
		if (*hotChar == '&')
			++hotChar;
	}
	return (sText);
}

void UIW_BUTTON::DataSet(char *_text)
{
	// Reset the string and check for a hot key.
	if (_text)
	{
		if (text && text != _text && !FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
			delete text;
		if (text == _text || FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
			text = _text;
		else
			text = ui_strdup(_text);
		hotKey = HotKey(text);
	}

	// Redisplay the string.
	if (screenID)
	{
#if defined(ZIL_MSDOS)
		Event(UI_EVENT(S_REDISPLAY));
#elif defined(ZIL_MSWINDOWS)
		if (parent->Inherited(ID_COMBO_BOX))
			SendMessage(screenID, CB_SETITEMDATA, ListIndex(),
				FlagSet(parent->woStatus, WOS_OWNERDRAW) ? (LONG)this : (LONG)text);
		else if (parent->Inherited(ID_LIST))
			SendMessage(screenID, LB_SETITEMDATA, ListIndex(),
				FlagSet(parent->woStatus, WOS_OWNERDRAW) ? (LONG)this : (LONG)text);
		else if (parent->Inherited(ID_MENU))								// BUG.1253
		{
			WORD wFlags = MF_STRING;
			if (FlagSet(woFlags, WOF_NON_SELECTABLE))
				wFlags |= MF_GRAYED;
			MNIF_FLAGS mniFlags = NULL;
			Information(GET_FLAGS, &mniFlags, ID_POP_UP_ITEM);
			if (FlagSet(mniFlags, MNIF_CHECK_MARK) && FlagSet(woStatus, WOS_SELECTED))
				wFlags |= MF_CHECKED;
			ModifyMenu(screenID, ListIndex(), MF_BYPOSITION | wFlags, numberID, text);
			parent->Event(UI_EVENT(S_REDISPLAY, 0));
		}
		else
		{
			InvalidateRect(screenID, NULL, TRUE);
			if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON) && screenID)
				SendMessage(screenID, BM_SETCHECK, FlagSet(woStatus, WOS_SELECTED), 0);
			SendMessage(screenID, WM_SETTEXT, 0, (LONG)text);
		}
#elif defined(ZIL_OS2)
		if (parent->Inherited(ID_VT_LIST))
			WinSendMsg(screenID, LM_SETITEMTEXT, (MPARAM)ListIndex(), (MPARAM)text);
		else if (parent->Inherited(ID_MENU))								// BUG.2268
		{
			WinSendMsg(parent->screenID, MM_SETITEMTEXT, (MPARAM)numberID, (MPARAM)text);
			parent->Event(UI_EVENT(S_REDISPLAY, 0));
		}
		else
		{
			WinSetWindowText(screenID, (PSZ)text);
			Event(UI_EVENT(S_REDISPLAY));
		}
#elif defined(ZIL_MOTIF)
		XmString labelText = XmStringCreateLtoR(text ? text : "", 
			XmSTRING_DEFAULT_CHARSET);
		nargs = 0;
		XtSetArg(args[nargs], XmNlabelString, labelText); nargs++;
		XtSetValues(screenID, args, nargs);
		if (true.bottom > true.top + 1)
			XtSetArg(args[nargs], XmNheight, true.bottom - true.top + 1),
				nargs++;
		if (true.right > true.left + 1)
			XtSetArg(args[nargs], XmNwidth, true.right - true.left + 1),
				nargs++;
		if (labelText)
			XmStringFree(labelText);
		Event(UI_EVENT(S_REDISPLAY));
		if (FlagSet(btFlags, BTF_RADIO_BUTTON) ||
			FlagSet(btFlags, BTF_CHECK_BOX))
			XmToggleButtonSetState(screenID,	
				FlagSet(woStatus, WOS_SELECTED) ? 1 : 0, FALSE);
#endif
	}
}

void *UIW_BUTTON::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_BUTTON;
	switch (request)
	{
	case INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = windowID[0] = ID_BUTTON;
		hotKey = HotKey(text);

		// See if the bitmap is system specified.
#if defined(ZIL_MSWINDOWS)
		if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON))
		{
			colorBitmap = LoadBitmap(0, (LPSTR)OBM_CHECKBOXES);
			BITMAP bm;
			GetObject(colorBitmap, sizeof(BITMAP), (LPSTR)&bm);
			bitmapHeight = bm.bmHeight / 3;
			bitmapWidth = bm.bmWidth / 4;
		}
		else if (bitmapName && !colorBitmap)
		{
			char tempName[32];
#if defined(WIN32)
			strcpy(tempName, "!");
#else
			strcpy(tempName, "_");
#endif
			strcat(tempName, bitmapName);
			colorBitmap = LoadBitmap(display->hInstance, bitmapName);
			monoBitmap = LoadBitmap(display->hInstance, tempName);
			if (colorBitmap)
			{
				BITMAP bm;
				GetObject(colorBitmap, sizeof(BITMAP), (LPSTR)&bm);
				bitmapHeight = bm.bmHeight;
				bitmapWidth = bm.bmWidth;
			}
		}
#endif

		// Read the bitmap from the system file.
#if defined(ZIL_STORAGE)
		if (!colorBitmap && bitmapName && !bitmapArray &&
			defaultStorage && !defaultStorage->storageError)
		{
			defaultStorage->ChDir("~UI_BITMAP");
			UI_STORAGE_OBJECT bFile(*defaultStorage, bitmapName, ID_BITMAP_IMAGE, UIS_READ);
			if (!bFile.objectError)
			{
				short _value; bFile.Load(&_value); bitmapWidth = _value;
				bFile.Load(&_value); bitmapHeight = _value;
				bitmapArray = new UCHAR[bitmapWidth * bitmapHeight];
				bFile.Load(bitmapArray, sizeof(UCHAR), bitmapWidth * bitmapHeight);
			}
		}
#endif
		// Continue to CHANGED_FLAGS.

	case CHANGED_FLAGS:
		// Check the object and base class flag settings.
		if (request == CHANGED_FLAGS)
			UI_WINDOW_OBJECT::Information(CHANGED_FLAGS, data, ID_BUTTON);
		if (FlagSet(btFlags, BTF_SEND_MESSAGE) && !userFunction)
			userFunction = UIW_BUTTON::Message;
		if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON))
		{
			woFlags &= ~WOF_JUSTIFY_CENTER;									// BUG.1276
			btFlags &= ~(BTF_AUTO_SIZE | BTF_NO_TOGGLE);
			btFlags |= BTF_NO_3D;
		}
		depth = FlagSet(btFlags, BTF_NO_3D) ? 0 : 2;

		// Check the environment specific flag settings.
#if defined(ZIL_MSWINDOWS)
		if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
			dwStyle |= BS_LEFTTEXT;
		if (FlagSet(btFlags, BTF_CHECK_BOX))
			dwStyle |= BS_CHECKBOX;
		else if (FlagSet(btFlags, BTF_RADIO_BUTTON))
			dwStyle |= BS_AUTORADIOBUTTON;
//		else if (colorBitmap || bitmapArray)								// BUG.1558
		else if (FlagSet(btFlags, BTF_NO_3D) || colorBitmap || bitmapArray ||
			!FlagSet(woFlags, WOF_JUSTIFY_CENTER))							// BUG.1579.B
		{
			dwStyle |= BS_OWNERDRAW;
			woStatus |= WOS_OWNERDRAW;
		}
#elif defined(ZIL_OS2)
		if (FlagSet(btFlags, BTF_CHECK_BOX))
//			flStyle |= BS_AUTOCHECKBOX;										// BUG.1769
			flStyle |= BS_CHECKBOX;
		else if (FlagSet(btFlags, BTF_RADIO_BUTTON))
//			flStyle |= BS_AUTORADIOBUTTON;
			flStyle |= BS_RADIOBUTTON;
		else if (colorBitmap || bitmapArray)
		{
			flStyle |= BS_USERBUTTON;
			woStatus |= WOS_OWNERDRAW;
		}
		else
			flStyle |= BS_PUSHBUTTON;
#endif

		// See if the field needs to be re-computed.
		if (objectID == ID_BUTTON && FlagSet(woStatus, WOS_REDISPLAY))
		{
			UI_EVENT event(S_INITIALIZE, 0);
			Event(event);
			event.type = S_CREATE;
			Event(event);
		}
		break;

	case GET_TEXT:
		// Get the button text.
		if (!data)
			return (DataGet());
		*(char **)data = DataGet();
		break;

	case COPY_TEXT:
		// Set the button text.
		data = strcpy((char *)data, DataGet());
		break;

	case SET_TEXT:
		// Set the button text.
		DataSet((char *)data);
		break;

	case GET_FLAGS:
	case SET_FLAGS:
	case CLEAR_FLAGS:
		// Get, set or clear the button flag settings.
		if (objectID && objectID != ID_BUTTON)
			data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		else if (request == GET_FLAGS && !data)
			data = &btFlags;
		else if (request == GET_FLAGS)
			*(BTF_FLAGS *)data = btFlags;
		else if (request == SET_FLAGS)
			btFlags |= *(BTF_FLAGS *)data;
		else
			btFlags &= ~(*(BTF_FLAGS *)data);
		break;

	case GET_STATUS:
	case SET_STATUS:
	case CLEAR_STATUS:
		// Get, set or clear the button status settings.
		if (objectID && objectID != ID_BUTTON)
			data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		else if (request == GET_STATUS && !data)
			data = &btStatus;
		else if (request == GET_STATUS)
			*(BTS_STATUS *)data = btStatus;
		else if (request == SET_STATUS)
			btStatus |= *(BTS_STATUS *)data;
		else if (request == CLEAR_STATUS)
			btStatus &= ~(*(BTS_STATUS *)data);
		break;

	case CHANGED_STATUS:
		DataSet(NULL);	// Redraw the item contents.
		break;

	case GET_VALUE:
		// Get the button value.
		*(EVENT_TYPE *)data = value;
		break;

	case SET_VALUE:
		// Reset the button value.
		value = *(EVENT_TYPE *)data;
		break;

	case GET_BITMAP_WIDTH:
		// Get the bitmap width.
		*(int *)data = bitmapWidth;
		break;

	case SET_BITMAP_WIDTH:
		// Set the bitmap width.
		bitmapWidth = *(int *)data;
		break;

	case GET_BITMAP_HEIGHT:
		// Get the bitmap height.
		*(int *)data = bitmapHeight;
		break;

	case SET_BITMAP_HEIGHT:
		// Set the bitmap height.
		bitmapHeight = *(int *)data;
		break;

	case GET_BITMAP_ARRAY:
		// See if the array needs to be converted.
		if (!bitmapArray && display)
			display->BitmapHandleToArray(screenID, colorBitmap, monoBitmap,
				&bitmapWidth, &bitmapHeight, &bitmapArray);

		// Get the bitmap array.
		if (!data)
			return (bitmapArray);
		*(UCHAR **)data = bitmapArray;
		break;

	case SET_BITMAP_ARRAY:
		// Set the bitmap array.
		if (bitmapArray && !FlagSet(btFlags, BTF_STATIC_BITMAPARRAY))
			delete bitmapArray;
		if (data)
		{
			bitmapArray = new UCHAR[bitmapWidth * bitmapHeight];
			memcpy(bitmapArray, (UCHAR *)data, bitmapWidth * bitmapHeight);
		}
		else
			bitmapArray = NULL;

		// See if the array needs to be converted.
#if defined(ZIL_MSDOS)
		if (colorBitmap)
		{
			delete colorBitmap;
			colorBitmap = NULL;
		}
		if (monoBitmap)
		{
			delete monoBitmap;
			monoBitmap = NULL;
		}
#elif defined(ZIL_MSWINDOWS)
		if (colorBitmap)
			DeleteObject(colorBitmap);
		if (monoBitmap)
			DeleteObject(monoBitmap);
		if (bitmapArray)
		{
			dwStyle = (dwStyle & 0xFFFFFF00L) | BS_OWNERDRAW;
			if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
				dwStyle |= BS_LEFTTEXT;
		}
		else
		{
			dwStyle = (dwStyle & 0xFFFFFF00L);
			if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
				dwStyle |= BS_LEFTTEXT;
		}
#elif defined(ZIL_OS2)
		if (colorBitmap)
			GpiDeleteBitmap(colorBitmap);
		if (monoBitmap)
			GpiDeleteBitmap(monoBitmap);
#elif defined(ZIL_MOTIF)
	if (pixmap)
		XFreePixmap(display->xDisplay, pixmap);
#endif
		colorBitmap = monoBitmap = 0;
		break;

	default:
		data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

EVENT_TYPE UIW_BUTTON::Message(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	// Check for valid button.
	if (FlagSet(object->woStatus, WOS_EDIT_MODE) ||
		(ccode != L_SELECT && ccode != L_DOUBLE_CLICK))
		return (ccode);

	// Check for a double-click message.
	BTF_FLAGS btFlags;
	object->Information(GET_FLAGS, &btFlags, ID_BUTTON);
	if (FlagSet(btFlags, BTF_DOUBLE_CLICK) && ccode == L_SELECT &&
		object->LogicalEvent(event, object->SearchID()) != L_SELECT)
		return (ccode);

	// Process the button value as a system message.
	EVENT_TYPE command;
	object->Information(GET_VALUE, &command);
	event.type = command;
	event.rawCode = 0;
	event.data = object;
	if (command == L_RESTORE || command == L_MOVE || command == L_SIZE ||
		command == L_MINIMIZE || command == L_MAXIMIZE || command == S_CLOSE)
	{
		for (UI_WINDOW_OBJECT *tObject = object; tObject; tObject = tObject->parent)
			if (tObject->Inherited(ID_WINDOW) && !tObject->Inherited(ID_MENU))
			{
				event.data = tObject;
				break;
			}
	}
//	object->eventManager->Put(event);										// BUG.1609
	object->eventManager->Put(event, Q_BEGIN);

	// Return the control code.
	return (ccode);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_PERSISTENCE)
UIW_BUTTON::UIW_BUTTON(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	UI_WINDOW_OBJECT(0, 0, 15, 1, WOF_NO_FLAGS, WOAF_NO_FLAGS),
	btStatus(BTS_NO_STATUS), bitmapWidth(0), bitmapHeight(0),
	bitmapArray(NULL), colorBitmap(0), monoBitmap(0)
{
	// Initialize the button information.
#if defined(ZIL_MOTIF)
	pixmap = 0;
#endif
	UIW_BUTTON::Load(name, directory, file);
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);					// BUG.2167
	UIW_BUTTON::Information(INITIALIZE_CLASS, NULL);
	UIW_BUTTON::DataSet(text);
}

void UIW_BUTTON::Load(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Load the button information.
	UI_WINDOW_OBJECT::Load(name, directory, file);
	file->Load(&btFlags);
	short _value; file->Load(&_value); value = _value;
	file->Load(&_value); depth = _value;
	file->Load(&text);
	file->Load(&bitmapName);

	// Load the bitmap information.
	if (bitmapName)
	{
		directory->ChDir("~UI_BITMAP");
		UI_STORAGE_OBJECT bFile(*directory, bitmapName, ID_BITMAP_IMAGE, UIS_READ);
		if (!bFile.objectError)
		{
			short _value; bFile.Load(&_value); bitmapWidth = _value;
			bFile.Load(&_value); bitmapHeight = _value;
			bitmapArray = new UCHAR[bitmapWidth * bitmapHeight];
			bFile.Load(bitmapArray, sizeof(UCHAR), bitmapWidth * bitmapHeight);
			// Convert the bitmap array to a handle if possible.
#if !defined(ZIL_OS2) && !defined(ZIL_MOTIF)
			if (display)
			{
				display->BitmapArrayToHandle(screenID, bitmapWidth,
					bitmapHeight, bitmapArray, NULL, &colorBitmap,
					&monoBitmap);
				if (colorBitmap && bitmapArray && !FlagSet(btFlags, BTF_STATIC_BITMAPARRAY))
				{
					delete bitmapArray;
					bitmapArray = NULL;
				}
			}
#endif
		}
	}
}

void UIW_BUTTON::Store(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Store the button information.
	UI_WINDOW_OBJECT::Store(name, directory, file);
	file->Store(btFlags);
	short _value = (short)value; file->Store(_value);
	_value = depth; file->Store(_value);
	file->Store(text);
	file->Store(bitmapName);

	// See if the bitmap needs to be converted.
	if (bitmapName && colorBitmap && !bitmapArray)
		display->BitmapHandleToArray(screenID, colorBitmap, monoBitmap,
			&bitmapWidth, &bitmapHeight, &bitmapArray);

	// Store the bitmap information.
	if (bitmapName && bitmapArray)
	{
		if (directory->ChDir("~UI_BITMAP"))
		{
			directory->MkDir("~UI_BITMAP");
			directory->ChDir("~UI_BITMAP");
		}
		UI_STORAGE_OBJECT bFile(*directory, bitmapName, ID_BITMAP_IMAGE, UIS_CREATE | UIS_READWRITE);
		if (!bFile.objectError)
		{
			_value = bitmapWidth; bFile.Store(_value);
			_value = bitmapHeight; bFile.Store(_value);
			bFile.Store(bitmapArray, sizeof(UCHAR), bitmapWidth * bitmapHeight);
		}
	}
}
#endif


