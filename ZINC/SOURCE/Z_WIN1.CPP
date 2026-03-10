//	Zinc Interface Library - Z_WIN1.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define INCL_WINMENUS			// OS/2 menu messages and flags.
#define INCL_WINLISTBOXES		// OS/2 list box messages and flags.
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif
#if defined(ZIL_MOTIF)
#	include <Xm/ToggleB.h>
#endif

// ----- UIW_WINDOW ---------------------------------------------------------

UIW_WINDOW::UIW_WINDOW(int left, int top, int width, int height,
	WOF_FLAGS _woFlags, WOAF_FLAGS _woAdvancedFlags,
	UI_HELP_CONTEXT _helpContext, UI_WINDOW_OBJECT *minObject) :
	UI_WINDOW_OBJECT(left, top, width, height, _woFlags, _woAdvancedFlags),
	UI_LIST(), support(), clipList(), wnFlags(WNF_NO_FLAGS)
{
	// Initialize the window information.
	compareFunctionName = NULL;
	helpContext = _helpContext;
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
	UIW_WINDOW::Information(INITIALIZE_CLASS, NULL);
	if (minObject)
	{
		minObject->woFlags |= WOF_SUPPORT_OBJECT;
		UIW_WINDOW::Add(minObject);
	}
}

UIW_WINDOW::~UIW_WINDOW(void)
{
	if (compareFunctionName)
		delete compareFunctionName;
}

void UIW_WINDOW::CheckSelection(void)
{
	// Check for proper single-select status.
	if (!current || !FlagSet(Current()->woStatus, WOS_SELECTED) ||
		FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
		return;

#if defined(ZIL_OS2)
	if (screenID && Inherited(ID_LIST))
	{
		WinSendMsg(screenID, LM_SELECTITEM, (MPARAM)LIT_NONE, (MPARAM)FALSE);
		WinSendMsg(screenID, LM_SELECTITEM, (MPARAM)Index(current), (MPARAM)TRUE);
	}
#endif

	// Make sure non-current items are not selected.
	int position = 0;
	for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next(), position++)
	{
		if (object != current && FlagSet(object->woStatus, WOS_SELECTED))
		{
			object->woStatus &= ~WOS_SELECTED;
#if defined(ZIL_MSDOS) || defined(ZIL_OS2)
			object->Event(UI_EVENT(S_REDISPLAY));
#elif defined(ZIL_MSWINDOWS)
			if (screenID && object->Inherited(ID_POP_UP_ITEM))
  				CheckMenuItem(screenID, position, MF_BYPOSITION | MF_UNCHECKED);
			else if (screenID && Inherited(ID_LIST))						// BUG.1346
				SendMessage(screenID, LB_SETSEL, 0, position);
			else if (screenID == object->screenID)
				;
			else if (object->screenID && object->Inherited(ID_BUTTON))
				SendMessage(object->screenID, BM_SETCHECK, 0, 0);
#elif defined(ZIL_MOTIF)
			if (object->Inherited(ID_BUTTON))
			{
				BTF_FLAGS btFlags;
				object->Information(GET_FLAGS, &btFlags, ID_BUTTON);
				if (FlagSet(btFlags, BTF_RADIO_BUTTON) ||
					FlagSet(btFlags, BTF_CHECK_BOX))
					XmToggleButtonSetState(object->screenID,	
						FlagSet(object->woStatus, WOS_SELECTED) ? 1 : 0, FALSE);
			}
#endif
		}
	}
}

void UIW_WINDOW::Destroy(void)
{
	UI_WINDOW_OBJECT *object;

	// Remove the window objects from the system.
	if (screenID)
	{
#if defined(ZIL_MSWINDOWS)
		SendMessage(screenID, WM_SETREDRAW, FALSE, 0);
		for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
			if (object->screenID)
				DestroyWindow(object->screenID);
#elif defined(ZIL_OS2)
		for (object = First(); object; object = object->Next())
			if (object->screenID && object->screenID != screenID)
				WinDestroyWindow(object->screenID);
#elif defined(ZIL_MOTIF)
		for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
			if (object->screenID)
				XtDestroyWidget(object->TopWidget());
#endif
	}

	// Destroy the list elements.
	UI_EVENT event(S_DEINITIALIZE);
	for (object = First(); object; object = object->Next())
	{
		object->screenID = 0;
		object->Event(event);
	}
	UI_LIST::Destroy();
}

void *UIW_WINDOW::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	UI_WINDOW_OBJECT *object;
	if (!objectID) objectID = ID_WINDOW;
	switch (request)
	{
	case INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = windowID[0] = ID_WINDOW;
		font = FNT_SYSTEM_FONT;

#if defined(ZIL_MSDOS)
		vScroll = hScroll = icon = NULL;
#elif defined(ZIL_MSWINDOWS)
		hScroll = vScroll = title = NULL;
		title = NULL;
		mdiClient = menu = icon = 0;
		mdiCallback = NULL;
#elif defined(ZIL_OS2)
		title = NULL;
#elif defined(ZIL_MOTIF)
		supportDecorations = 0;
		vScroll = hScroll = NULL;
#endif
		// Continue to CHANGED_FLAGS.

	case CHANGED_FLAGS:
		// Check the object and base class flag settings.
		if (request == CHANGED_FLAGS)
			UI_WINDOW_OBJECT::Information(CHANGED_FLAGS, data, ID_WINDOW);
		if (FlagSet(wnFlags, WNF_AUTO_SORT))
			compareFunction = UIW_WINDOW::StringCompare;

		// Check the environment specific flag settings.
#if defined(ZIL_OS2)
		if (FlagSet(woFlags, WOF_BORDER))
			flFlag |= FCF_BORDER;
#elif defined(ZIL_MOTIF)
		supportDecorations = 0;
#endif

		// See if the window needs to be re-computed.
		if (objectID == ID_WINDOW && FlagSet(woStatus, WOS_REDISPLAY))
		{
			UI_EVENT event(S_INITIALIZE, 0);
			Event(event);
			event.type = S_CREATE;
			Event(event);
#if defined(ZIL_OS2)
			if (windowManager->First() == this)								// BUG.????
				WinSetActiveWindow(HWND_DESKTOP, screenID);
#endif
		}
		break;

	case DESTROY_LIST:
		Destroy();
		break;

	case CHECK_SELECTION:
		CheckSelection();
		break;

	case GET_NUMBERID_OBJECT:
	case GET_STRINGID_OBJECT:
		{
		void *match = UI_WINDOW_OBJECT::Information(request, data, objectID);
		for (object = (UI_WINDOW_OBJECT *)support.First(); object && !match; object = object->Next())
			match = object->Information(request, data, objectID);
		for (object = First(); object && !match; object = object->Next())
			match = object->Information(request, data, objectID);
		data = match;
		}
		break;

	case SET_TEXT:
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			if (object->Inherited(ID_TITLE) || object->Inherited(ID_ICON))
				object->Information(request, data, objectID);
		break;

	case GET_TEXT:
	case COPY_TEXT:
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			if (object->Inherited(ID_TITLE) || object->Inherited(ID_ICON))
				return object->Information(request, data, objectID);
		break;

	case GET_FLAGS:
	case SET_FLAGS:
	case CLEAR_FLAGS:
		// Get, set or clear the flag settings.
		if (objectID && objectID != ID_WINDOW)
			data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		else if (request == GET_FLAGS && !data)
			data = &wnFlags;
		else if (request == GET_FLAGS)
			*(WNF_FLAGS *)data = wnFlags;
		else if (request == SET_FLAGS)
			wnFlags |= *(WNF_FLAGS *)data;
		else
			wnFlags &= ~(*(WNF_FLAGS *)data);
		break;

	case GET_CLIPREGION:
		if (clipList.First())
			*(UI_REGION *)data = clipList.First()->region;
		else
			*(UI_REGION *)data = true;
		break;

	case SET_VSCROLL:	vScroll = (UI_WINDOW_OBJECT *)data;		break;
	case SET_HSCROLL:	hScroll = (UI_WINDOW_OBJECT *)data;		break;
#if defined(ZIL_MSDOS)
	case SET_ICON:		icon = (UI_WINDOW_OBJECT *)data;		break;
#elif defined(ZIL_MSWINDOWS)
	case GET_MDIHANDLE:	*(SCREENID *)data = mdiClient;			break;
	case GET_MENU:		*(HMENU *)data = menu;					break;

	case SET_TITLE:		title = (UI_WINDOW_OBJECT *)data;		break;
	case SET_MENU:		menu = *(HMENU *)data;					break;
	case SET_ICON:		icon = *(HICON *)data;					break;

	case HIDE_SUBWINDOW:
		if (!current)
			break;
		else if (Current()->Inherited(ID_COMBO_BOX))
			SendMessage(Current()->screenID, CB_SHOWDROPDOWN, FALSE, 0);
		else
			Current()->Information(request, data, objectID);
		break;

#elif defined(ZIL_OS2)
	case SET_TITLE:		title = (UI_WINDOW_OBJECT *)data;		break;
#elif defined(ZIL_MOTIF)
	case GET_DECORATIONS:	*(long *)data = supportDecorations;	break;
	case SET_DECORATIONS:	supportDecorations |= *(long *)data;break;
#endif

#if defined(ZIL_PERSISTENCE)
	case PRINT_INFORMATION:
	case PRINT_USER_FUNCTION:
	case PRINT_COMPARE_FUNCTION:
		if (request == PRINT_COMPARE_FUNCTION && compareFunctionName)
		{
			UI_STORAGE_OBJECT *cppEntry = (UI_STORAGE_OBJECT *)data;
			cppEntry->Store(compareFunctionName);
		}
		else if (request != PRINT_COMPARE_FUNCTION)
			UI_WINDOW_OBJECT::Information(request, data, objectID);
		for (object = First(); object; object = object->Next())
			object->Information(request, data, objectID);
		break;
#endif

	default:
		data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

int UIW_WINDOW::StringCompare(void *object1, void *object2)
{
	char *string1, *string2;
	((UI_WINDOW_OBJECT *)object1)->Information(GET_TEXT, &string1);
	if (!string1)
		return (-1);
	((UI_WINDOW_OBJECT *)object2)->Information(GET_TEXT, &string2);
	if (!string2)
		return (1);
	return (strcmp(string1, string2));
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_PERSISTENCE)
UIW_WINDOW::UIW_WINDOW(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	UI_WINDOW_OBJECT(0, 0, 40, 10, WOF_NO_FLAGS, WOAF_NO_FLAGS),
	UI_LIST(), support(), clipList()
{
	// Initialize the window information.
	if (name && !file)
	{
		long miniNumeratorX = display->miniNumeratorX;
		long miniDenominatorX = display->miniDenominatorX;
		long miniNumeratorY = display->miniNumeratorY;
		long miniDenominatorY = display->miniDenominatorY;
		Load(name, directory, file);
		UIW_WINDOW::Information(INITIALIZE_CLASS, NULL);
//		if (FlagSet(woFlags, WOF_MINICELL))									// BUG.1047
//			Event(UI_EVENT(S_INITIALIZE, 0));
		display->miniNumeratorX = miniNumeratorX;
		display->miniDenominatorX = miniDenominatorX;
		display->miniNumeratorY = miniNumeratorY;
		display->miniDenominatorY = miniDenominatorY;
	}
	else
	{
		Load(name, directory, file);
		UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
		UIW_WINDOW::Information(INITIALIZE_CLASS, NULL);
	}
}

void UIW_WINDOW::Load(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Check for a valid directory and file.
	int tempDirectory = FALSE, tempFile = FALSE;
	if (name && !file)
	{
		char pathName[128], fileName[32], objectName[32], objectPathName[128];
		UI_STORAGE::StripFullPath(name, pathName, fileName, objectName, objectPathName);
		if (!directory)
		{
			tempDirectory = TRUE;
			UI_STORAGE::AppendFullPath(pathName, pathName, fileName);
			UI_STORAGE::ChangeExtension(pathName, ZIL_EXT);
			directory = new UI_STORAGE(pathName, UIS_READ);
			if (directory->storageError)
			{
				UIW_WINDOW::Error(WOS_READ_ERROR, "File %s was not found.", fileName);
				goto LOAD_ERROR;
			}
		}
		if (!file)
		{
			tempFile = TRUE;
			if (objectPathName[0] != '\0' && !directory->ChDir(objectPathName))
				;
			else
				directory->ChDir("~UIW_WINDOW");
			if (objectName[0] == '\0')
				strcpy(objectName, fileName);
			file = new UI_STORAGE_OBJECT(*directory, objectName, ID_WINDOW, UIS_READ);
			if (file->objectError)
			{
				Error(WOS_READ_ERROR, "Object %s was not found.", objectName);
				goto LOAD_ERROR;
			}
			file->Load(&display->miniNumeratorX);	// miniNumeratorX
			file->Load(&display->miniDenominatorX);	// miniDenominatorX
			file->Load(&display->miniNumeratorY);	// miniNumeratorY
			file->Load(&display->miniDenominatorY);	// miniDenominatorY
		}
	}

	// Load the window information.
	UI_WINDOW_OBJECT::Load(name, directory, file);

	// Load the object information.
	{
 	short noOfObjects;
	file->Load(&noOfObjects);
	for (int i = 0; i < noOfObjects; i++)
	{
		UI_WINDOW_OBJECT *object = UI_WINDOW_OBJECT::New(NULL, directory, file);
		if (!object)
		{
			UIW_WINDOW::Error(WOS_READ_ERROR, "Unknown window object.");
			goto LOAD_ERROR;
		}
		Add(object);
	}
	file->Load(&wnFlags);
	file->Load(&compareFunctionName);
	if (compareFunctionName && userTable)
	{
		for (int i = 0; !compareFunction && userTable[i].data; i++)
			if (ui_stricmp(userTable[i].text, compareFunctionName) == 0)
				compareFunction = (COMPARE_FUNCTION)userTable[i].data;
	}
	}

LOAD_ERROR:
	// Clean up the file and storage.
	if (tempFile)
		delete file;
	if (tempDirectory)
		delete directory;
}

void UIW_WINDOW::Store(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Check for a valid directory and file.
	int tempDirectory = FALSE, tempFile = FALSE;
	if (name && !file)
	{
		char pathName[128], fileName[32], objectName[32], objectPathName[128];
		UI_STORAGE::StripFullPath(name, pathName, fileName, objectName, objectPathName);
		if (!directory)
		{
			UI_STORAGE::AppendFullPath(pathName, pathName, fileName);
			UI_STORAGE::ChangeExtension(pathName, ZIL_EXT);
			directory = new UI_STORAGE(pathName, UIS_OPENCREATE | UIS_READWRITE);
			if (directory->storageError)
			{
				delete directory;
				return;
			}
			tempDirectory = TRUE;
		}
		if (!file)
		{
			if (objectPathName[0] != '\0' && !directory->ChDir(objectPathName))
				;
			else if (directory->ChDir("~UIW_WINDOW"))
			{
				directory->MkDir("~UIW_WINDOW");
				directory->ChDir("~UIW_WINDOW");
			}
			if (objectName[0] == '\0')
				strcpy(objectName, fileName);
			StringID(objectName);
			file = new UI_STORAGE_OBJECT(*directory, objectName, ID_WINDOW, UIS_CREATE | UIS_READWRITE);
			file->Store(display->miniNumeratorX);	// miniNumeratorX
			file->Store(display->miniDenominatorX);	// miniDenominatorX
			file->Store(display->miniNumeratorY);	// miniNumeratorY
			file->Store(display->miniDenominatorY);	// miniDenominatorY
			tempFile = TRUE;
			// Reset the object table status.
			if (objectTable)
			{
				for (int i = 0; objectTable[i].data; i++)
					objectTable[i].flags = FALSE;
			}
		}
	}

	// Store the object information.
	UI_WINDOW_OBJECT::Store(name, directory, file);
	short noOfObjects = support.Count() + UI_LIST::Count();
	file->Store(noOfObjects);
	for (UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
	{
		file->Store(object->searchID);
		object->Store(NULL, directory, file);
	}
	for (object = First(); object; object = object->Next())
	{
		file->Store(object->searchID);
		object->Store(NULL, directory, file);
	}
	file->Store(wnFlags);
	file->Store(compareFunctionName);

	// Write out the header information.
	if (!parent && stringID[0] != '\0')
	{
		if (directory->ChDir("~UI_HPP"))
		{
			directory->MkDir("~UI_HPP");
			directory->ChDir("~UI_HPP");
		}
		UI_STORAGE_OBJECT hppEntry(*directory, stringID, ID_WINDOW, UIS_CREATE | UIS_READWRITE);
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			object->Information(PRINT_INFORMATION, &hppEntry);
		for (object = First(); object; object = object->Next())
			object->Information(PRINT_INFORMATION, &hppEntry);
		hppEntry.Store("");	// End of HPP string entries.
	}

	// Clean up the file and storage.
	if (tempFile)
	{
		delete file;

		// Create the CPP entries.
		if (directory->ChDir("~UI_CPP"))
		{
			directory->MkDir("~UI_CPP");
			directory->ChDir("~UI_CPP");
		}
		if (directory->ChDir(stringID))
		{
			directory->MkDir(stringID);
			directory->ChDir(stringID);
		}

		if (objectTable)
		{
			OBJECTID objectID;
			UI_STORAGE_OBJECT cppEntry(*directory, "OBJECTID", 0, UIS_CREATE | UIS_READWRITE);
			for (int i = 0; objectTable[i].data; i++)
				if (objectTable[i].flags)
				{
					objectID = (OBJECTID)objectTable[i].value;
					cppEntry.Store(objectID);
				}
				objectID = 0; cppEntry.Store(objectID);	// End of CPP object entries.
		}

		UI_STORAGE_OBJECT userEntry(*directory, "USER_FUNCTION", 0, UIS_CREATE | UIS_READWRITE);
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			object->Information(PRINT_USER_FUNCTION, &userEntry);
		for (object = First(); object; object = object->Next())
			object->Information(PRINT_USER_FUNCTION, &userEntry);
		userEntry.Store("");	// End of CPP user-function entries.

		UI_STORAGE_OBJECT compareEntry(*directory, "COMPARE_FUNCTION", 0, UIS_CREATE | UIS_READWRITE);
		for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			object->Information(PRINT_COMPARE_FUNCTION, &compareEntry);
		for (object = First(); object; object = object->Next())
			object->Information(PRINT_COMPARE_FUNCTION, &compareEntry);
		compareEntry.Store("");	// End of CPP compare-function entries.
	}
	if (tempDirectory)
	{
		directory->Save();
		delete directory;
	}
}
#endif

// ----- ERROR_HANDLING -----------------------------------------------------

void UIW_WINDOW::Error(UIS_STATUS status, const char *format, ...)
{
#if defined(ZIL_DEBUG)
	char *message = new char[256];
	va_list arguments;
   	va_start(arguments, format);
	UI_INTERNATIONAL::vsprintf(message, format, arguments);
	va_end(arguments);

	// Display the error message before we abort.
#if defined(ZIL_MSDOS)
	if (display)
		delete display;
	printf(message);
#elif defined(ZIL_MSWINDOWS)
	MessageBox(0, message, NULL, MB_OK);
#elif defined(ZIL_OS2)
	WinMessageBox(HWND_DESKTOP, windowManager->screenID, message, NULL, 0,
		MB_ICONEXCLAMATION | MB_OK);
#elif defined(ZIL_MOTIF)
	format = format;
	printf(message);
#endif

	delete message;
	abort();
#endif

	// Flag parents with the error.
	for (UI_WINDOW_OBJECT *object = this; object; object = object->parent)
		object->woStatus |= status;
}


