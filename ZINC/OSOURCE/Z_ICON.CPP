//	Zinc Interface Library - Z_ICON.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define INCL_WINSTATICS			// OS/2 static messages and flags.
#define INCL_WINLISTBOXES		// OS/2 list messages and flags.
#include <ctype.h>
#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_ICON -----------------------------------------------------------

UIW_ICON::UIW_ICON(int left, int top, char *_iconName, char *_title,
	ICF_FLAGS _icFlags, WOF_FLAGS _woFlags, USER_FUNCTION _userFunction) :
	UI_WINDOW_OBJECT(left, top, 0, 0, _woFlags, WOAF_NO_FLAGS),
	icFlags(_icFlags), iconWidth(32), iconHeight(32), iconArray(NULL), title(NULL)
{
	// Initialize the icon information.
#if defined(ZIL_MSDOS)
		icon.colorBitmap = NULL;
		icon.monoBitmap = NULL;
#else
		icon = 0;
#endif
	DataSet(_title);
	iconName = ui_strdup(_iconName);
	userFunction = _userFunction;
	UIW_ICON::Information(INITIALIZE_CLASS, NULL);
}

UIW_ICON::~UIW_ICON(void)
{
	if (title)
		delete title;
	if (iconName)
		delete iconName;
	if (iconArray)
		delete iconArray;

	// Free the icon handle.
#if defined(ZIL_MSDOS)
	if (icon.colorBitmap)
		delete icon.colorBitmap;
	if (icon.monoBitmap)
		delete icon.monoBitmap;
#elif defined(ZIL_MSWINDOWS)
	if (icon && FlagSet(woStatus, WOS_DESTROY_ICON))
		DestroyIcon(icon);
#elif defined(ZIL_MOTIF)
	if (icon && FlagSet(woStatus, WOS_DESTROY_ICON))
		XFreePixmap(display->xDisplay, icon);
#endif
}

char *UIW_ICON::DataGet(void)
{
 	return (title);
}

void UIW_ICON::DataSet(char *_title)
{
	// Reset the title and check for a hotkey.
	if (title && title != _title && !FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete title;
	if (title == _title || FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		title = _title;
	else
		title = ui_strdup(_title);
	hotKey = HotKey(title);

	// Redisplay the string.
	if (screenID)
	{
#if defined(ZIL_MSDOS)
		if (!display->isText && title)
		{
			titleRegion.bottom = true.bottom;
			titleRegion.top = titleRegion.bottom - display->TextHeight(title, screenID, font);
			int center = (true.left + true.right) /2;
			int width = display->TextWidth(title, screenID, font);
			titleRegion.left = center - width/2 ;
			titleRegion.right = center + width/2;
			true.left = Min(titleRegion.left, true.left);
			true.right = Max(titleRegion.right, true.right);
		}
		Event(UI_EVENT(S_REDISPLAY));
#elif defined(ZIL_MSWINDOWS)
		InvalidateRect(screenID, NULL, TRUE);
		SendMessage(screenID, WM_SETTEXT, 0, (LONG)title);
#elif defined(ZIL_OS2)
		WinSetWindowText(screenID, (PSZ)title);
#endif
	}
#if defined(ZIL_MOTIF)
		if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT) && shell)
			XtVaSetValues(shell, XmNiconName, title, NULL);
#endif
}

void *UIW_ICON::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
#if defined(ZIL_MSWINDOWS)
	// Initialize the standard windows icons.
	struct ICON_ITEM
	{
		char *name;
		LPSTR identification;
	} _iconTable[] = {
		{ "APPLICATION", 	(LPSTR)IDI_APPLICATION },
		{ "ASTERISK", 		(LPSTR)IDI_ASTERISK },
		{ "EXCLAMATION", 	(LPSTR)IDI_EXCLAMATION },
		{ "HAND", 			(LPSTR)IDI_HAND },
		{ "QUESTION", 		(LPSTR)IDI_QUESTION },
		{ 0, 0 } };
#endif

	// Switch on the request.
	if (!objectID) objectID = ID_ICON;
	switch (request)
	{
	case INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = windowID[0] = ID_ICON;
		if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
			woFlags |= WOF_SUPPORT_OBJECT;
		font = FNT_SMALL_FONT;
		hotKey = HotKey(title);

		// Read in the icon if so specified.
#if defined(ZIL_MSWINDOWS)
		if (!icon && iconName)
		{
			for (int i = 0; _iconTable[i].name && !icon; i++)
				if (ui_stricmp(iconName, _iconTable[i].name) == 0)
					icon = LoadIcon(0, _iconTable[i].identification);
			if (!icon)
				icon = LoadIcon(display->hInstance, iconName);
		}
#endif

		// Read the bitmap from the system file.
#if defined(ZIL_STORAGE)
		if (iconName && !iconArray &&
			defaultStorage && !defaultStorage->storageError)
		{
			defaultStorage->ChDir("~UI_ICON");
			UI_STORAGE_OBJECT iFile(*defaultStorage, iconName, ID_ICON_IMAGE, UIS_READ);
			if (!iFile.objectError)
			{
				short _value; iFile.Load(&_value); iconWidth = _value;
				iFile.Load(&_value); iconHeight = _value;
				iconArray = new UCHAR[iconWidth * iconHeight];
				iFile.Load(iconArray, sizeof(UCHAR), iconWidth * iconHeight);
			}
		}
#endif
		// Continue to CHANGED_FLAGS.

	case CHANGED_FLAGS:
		// Check the icon and base class flag settings.
		if (request == CHANGED_FLAGS && !FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
			UI_WINDOW_OBJECT::Information(CHANGED_FLAGS, data, ID_ICON);

		// Check the environment specific flag settings.
		woStatus |= WOS_OWNERDRAW;
#if defined(ZIL_MSWINDOWS) || defined(ZIL_OS2)
		icStatus = 0;
#endif

		// See if the field needs to be re-computed.
		if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
		{
			if (request == CHANGED_FLAGS)
				parent->Information(SET_ICON, &icon);
		}
		else if (objectID == ID_ICON && FlagSet(woStatus, WOS_REDISPLAY))
		{
			UI_EVENT event(S_INITIALIZE, 0);
			Event(event);
			event.type = S_CREATE;
			Event(event);
		}
		break;

	case GET_TEXT:
		// Get the icon text.
		if (!data)
			return (title);
		*(char **)data = title;
		break;

	case COPY_TEXT:
		// Set the icon text.
		data = strcpy((char *)data, title);
		break;

	case SET_TEXT:
		// Set the icon text.
		DataSet((char *)data);
		break;

	case GET_FLAGS:
	case SET_FLAGS:
	case CLEAR_FLAGS:
		// Get, set or clear the icon flag settings.
		if (objectID && objectID != ID_ICON)
			data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		else if (request == GET_FLAGS && !data)
			data = &icFlags;
		else if (request == GET_FLAGS)
			*(ICF_FLAGS *)data = icFlags;
		else if (request == SET_FLAGS)
			icFlags |= *(ICF_FLAGS *)data;
		else
			icFlags &= ~(*(ICF_FLAGS *)data);
		break;

	case GET_ICON_ARRAY:
		// See if the array needs to be converted.
		if (!iconArray)
			display->IconHandleToArray(screenID, icon, &iconWidth,
				&iconHeight, &iconArray);

		// Get the bitmap array.
		if (!data)
			return (iconArray);
		*(UCHAR **)data = iconArray;
		break;

	case SET_ICON_ARRAY:
		// Set the bitmap array.
		if (iconArray)
			delete iconArray;
		if (data)
		{
			iconArray = new UCHAR[iconWidth * iconHeight];
			memcpy(iconArray, (UCHAR *)data, iconWidth * iconHeight);
		}
		else
			iconArray = NULL;

		// See if the array needs to be converted.
#if defined(ZIL_MSDOS)
		if (icon.colorBitmap)
		{
			delete icon.colorBitmap;
			icon.colorBitmap = NULL;
		}
		if (icon.monoBitmap)
		{
			delete icon.monoBitmap;
			icon.monoBitmap = NULL;
		}
#elif defined(ZIL_MSWINDOWS)
		if (icon && FlagSet(woStatus, WOS_DESTROY_ICON))
			DestroyIcon(icon);
		icon = 0;
#elif defined(ZIL_OS2)
		icon = 0;															// BUG.???? --- Not destroying icon!
#elif defined(ZIL_MOTIF)
		if (icon && FlagSet(woStatus, WOS_DESTROY_ICON))
			XFreePixmap(display->xDisplay, icon);
		icon = 0;
#endif
		break;

	default:
		data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_PERSISTENCE)
UIW_ICON::UIW_ICON(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	UI_WINDOW_OBJECT(0, 0, 15, 1, WOF_NO_FLAGS, WOAF_NO_FLAGS),
	iconWidth(32), iconHeight(32), iconArray(NULL)
{
	// Initialize the icon information.
#if defined(ZIL_MSDOS)
		icon.colorBitmap = NULL;
		icon.monoBitmap = NULL;
#else
		icon = 0;
#endif
	UIW_ICON::Load(name, directory, file);
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
	UIW_ICON::Information(INITIALIZE_CLASS, NULL);
}

void UIW_ICON::Load(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Load the icon information.
	UI_WINDOW_OBJECT::Load(name, directory, file);
	file->Load(&icFlags);
	file->Load(&title);
	file->Load(&iconName);

	// Read the bitmap information.
	if (iconName)
	{
		directory->ChDir("~UI_ICON");
		UI_STORAGE_OBJECT iFile(*directory, iconName, ID_ICON_IMAGE, UIS_READ);
		if (!iFile.objectError)
		{
			short _value; iFile.Load(&_value); iconWidth = _value;
			iFile.Load(&_value); iconHeight = _value;
			iconArray = new UCHAR[iconWidth * iconHeight];
			iFile.Load(iconArray, sizeof(UCHAR), iconWidth * iconHeight);

			// Convert the icon array to a handle if possible.
#if defined(ZIL_MSDOS)
			if (iconArray && !icon.colorBitmap)
			{
				display->IconArrayToHandle(screenID, iconWidth, iconHeight,
					iconArray, NULL, &icon);
				if (icon.colorBitmap)
				{
					delete iconArray;
					iconArray = NULL;
				}
			}
#elif !defined(ZIL_OS2) && !defined(ZIL_MOTIF)
			if (iconArray && !icon)
			{
				display->IconArrayToHandle(screenID, iconWidth, iconHeight,
					iconArray, NULL, &icon);
				if (icon)
				{
					delete iconArray;
					iconArray = NULL;
				}
			}
#endif
		}
	}
}

void UIW_ICON::Store(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Store the icon information.
	UI_WINDOW_OBJECT::Store(name, directory, file);
	file->Store(icFlags);
	file->Store(title);
	file->Store(iconName);

	// See if the bitmap needs to be converted.
#if defined(ZIL_MSDOS)
	if (iconName && icon.colorBitmap && !iconArray)
#else
	if (iconName && icon && !iconArray)
#endif
		display->IconHandleToArray(screenID, icon, &iconWidth, &iconHeight,
			&iconArray);

	// Store the bitmap information.
	if (iconName && iconArray)
	{
		if (directory->ChDir("~UI_ICON"))
		{
			directory->MkDir("~UI_ICON");
			directory->ChDir("~UI_ICON");
		}
		UI_STORAGE_OBJECT iFile(*directory, iconName, ID_ICON_IMAGE, UIS_CREATE | UIS_READWRITE);
		if (!iFile.objectError)
		{
			short _value = iconWidth; iFile.Store(_value);
			_value = iconHeight; iFile.Store(_value);
			iFile.Store(iconArray, sizeof(UCHAR), iconWidth * iconHeight);
		}
	}
}
#endif


