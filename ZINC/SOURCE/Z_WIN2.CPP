//	Zinc Interface Library - Z_WIN2.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define INCL_WINBUTTONS			// OS/2 BS_USERBUTTON definition.
#define INCL_WINMENUS			// OS/2 OWNERITEM definition

#if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
#include <mem.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UI_WINDOW_OBJECT ---------------------------------------------------

UI_WINDOW_OBJECT::UI_WINDOW_OBJECT(int left, int top, int width, int height,
	WOF_FLAGS _woFlags, WOAF_FLAGS _woAdvancedFlags) : woFlags(_woFlags),
	woAdvancedFlags(_woAdvancedFlags), eventMapTable(NULL),
	hotKeyMapTable(NULL), paletteMapTable(NULL)
{
#ifdef ZIL_EDIT
	designerAdvancedFlags = 0;
#endif
	numberID = 0;
	stringID[0] = '\0';
	userFlags = UIF_NO_FLAGS;
	userStatus = UIS_NO_FLAGS;
	userObject = NULL;
	userFunction = NULL;
	userObjectName = NULL;
	userFunctionName = NULL;
	helpContext = NO_HELP_CONTEXT;
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);

	// Intialize the screen object information.
	true.left = relative.left = left;
	true.top = relative.top = top;

	true.right = relative.right = (left >= 0 && width > 0) ?
		left + width - 1 : width;
	true.bottom = relative.bottom = (top >= 0 && height > 0) ?
		(height == 1) ? top : top + height - 1 : height;
}

UI_WINDOW_OBJECT::~UI_WINDOW_OBJECT(void)
{
	if (userObjectName)
		delete userObjectName;
	if (userFunctionName)
		delete userFunctionName;
}

unsigned UI_WINDOW_OBJECT::HotKey(unsigned _hotKey)
{
	if (_hotKey)
		hotKey = _hotKey;
	return (hotKey);
}

unsigned UI_WINDOW_OBJECT::HotKey(char *text)
{
	hotKey = 0;
	if (text)
	{
		ui_strrepc(text, OLD_UNDERLINE_CHARACTER, UNDERLINE_CHARACTER);
		for (char *hotChar = strchr(text, UNDERLINE_CHARACTER); !hotKey && hotChar;
			hotChar = strchr(++hotChar, UNDERLINE_CHARACTER))
			if (hotChar[1] != UNDERLINE_CHARACTER)
			{
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2)
				hotKey = tolower(hotChar[1]);
#elif defined(ZIL_MOTIF)
				hotKey = hotChar[1];
#endif
				break;
			}
			else
				hotChar++;
	}
	return (hotKey);
}

int UI_WINDOW_OBJECT::NeedsUpdate(const UI_EVENT &, EVENT_TYPE)
{
	return (TRUE);
}

void *UI_WINDOW_OBJECT::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_WINDOW_OBJECT;
	switch (request)
	{
	case INITIALIZE_CLASS:
		// Set the object identification and initial settings.
		searchID = windowID[0] = windowID[1] = windowID[2] = windowID[3] =
//			windowID[4] = windowID[5] = ID_WINDOW_OBJECT;					// Bug.General
			windowID[4] = ID_WINDOW_OBJECT;
		screenID = 0;
		parent = NULL;
		lastPalette = NULL;

		hotKey = 0;
		woStatus = defaultStatus;
		font = FNT_DIALOG_FONT;
		extern UI_EVENT_MAP *_eventMapTable;
		if (!eventMapTable)
			eventMapTable = _eventMapTable;
		extern UI_EVENT_MAP *_hotKeyMapTable;
		if (!hotKeyMapTable)
			hotKeyMapTable = _hotKeyMapTable;
		extern UI_PALETTE_MAP *_normalPaletteMapTable;
		if (!paletteMapTable)
			paletteMapTable = _normalPaletteMapTable;
		if (FlagSet(woFlags, WOF_INVALID))
			woStatus |= WOS_INVALID;
		if (FlagSet(woFlags, WOF_UNANSWERED))
			woStatus |= WOS_UNANSWERED;

		// Set the environment specific flags.
#if defined(ZIL_MSWINDOWS)
		dwStyle = 0;
		defaultCallback = NULL;
#elif defined(ZIL_OS2)
		flStyle = 0;
		flFlag = 0;
		defaultCallback = NULL;
#elif defined(ZIL_MOTIF)
		shell = 0;
#endif
		// Continue to CHANGED_FLAGS.

	case CHANGED_FLAGS:
		// Set the environment specific flags.
		woStatus &= ~WOS_REDISPLAY;
#if defined(ZIL_MSDOS)
		if (screenID)
			woStatus |= WOS_REDISPLAY;
#elif defined(ZIL_MSWINDOWS)
		if (screenID)
		{
			dwStyle = parent ? WS_CHILD | WS_VISIBLE : WS_OVERLAPPED;
			if (!parent || screenID != parent->screenID)
			{
				woStatus |= WOS_INTERNAL_ACTION;                                // BUG.1551
				DestroyWindow(screenID);
				UI_EVENT event(S_DEINITIALIZE, 0);
				Event(event);
				woStatus |= WOS_REDISPLAY;
				woStatus &= ~WOS_INTERNAL_ACTION;
			}
		}
		else
			dwStyle = 0;
		if (FlagSet(woFlags, WOF_BORDER))
			dwStyle |= WS_BORDER;
		if (FlagSet(woFlags, WOF_NON_SELECTABLE))
			dwStyle |= WS_DISABLED;
#elif defined(ZIL_OS2)
		flStyle = WS_VISIBLE;
		if (screenID && (!parent || screenID != parent->screenID))
		{
			woStatus |= WOS_INTERNAL_ACTION;                                // BUG.????
			WinDestroyWindow(screenID);
			UI_EVENT event(S_DEINITIALIZE, 0);
			Event(event);
			woStatus |= WOS_REDISPLAY;
			woStatus &= ~WOS_INTERNAL_ACTION;
		}
//		if (FlagSet(woFlags, WOF_NON_SELECTABLE))
//			flStyle |= WS_DISABLED;											// BUG.1780
#elif defined(ZIL_MOTIF)
		if (screenID)
		{
			XtDestroyWidget(TopWidget());
			UI_EVENT event(S_DEINITIALIZE, 0);
			Event(event);
			woStatus |= WOS_REDISPLAY;
		}
#endif
		break;

	case GET_NUMBERID_OBJECT:
		data = (numberID == *(NUMBERID *)data) ? this : NULL;
		break;

	case GET_STRINGID_OBJECT:
		data = ui_stricmp(stringID, (char *)data) == 0 ? this : NULL;
		break;

	case GET_FLAGS:
	case SET_FLAGS:
	case CLEAR_FLAGS:
		if (objectID && objectID != ID_WINDOW_OBJECT)
			return (NULL);
		else if (request == GET_FLAGS && !data)
			data = &woFlags;
		else if (request == GET_FLAGS)
			*(WOF_FLAGS *)data = woFlags;
		else if (request == SET_FLAGS)
			woFlags |= *(WOF_FLAGS *)data;
		else
			woFlags &= ~(*(WOF_FLAGS *)data);
		break;

	case GET_STATUS:
	case SET_STATUS:
	case CLEAR_STATUS:
		if (objectID && objectID != ID_WINDOW_OBJECT)
			return (NULL);
		else if (request == GET_STATUS && !data)
			data = &woStatus;
		else if (request == GET_STATUS)
			*(WOS_STATUS *)data = woStatus;
		else if (request == SET_STATUS)
			woStatus |= *(WOS_STATUS *)data;
		else if (request == CLEAR_STATUS)
			woStatus &= ~(*(WOS_STATUS *)data);
		break;

	case CHANGED_STATUS:
#if defined(ZIL_MSWINDOWS)
		if (parent && parent->Inherited(ID_LIST))
			SendMessage(screenID, LB_SETSEL, FlagSet(woStatus, WOS_SELECTED), ListIndex());
#endif
		Event(UI_EVENT(S_REDISPLAY, 0));
		break;

#if defined(ZIL_MSWINDOWS)
	case GET_DWSTYLE:		*(DWORD *)data = dwStyle;			break;
	case SET_DWSTYLE:		dwStyle |= *(DWORD *)data;			break;
	case CLEAR_DWSTYLE:		dwStyle &= ~(*(DWORD *)data);		break;
#elif defined(ZIL_OS2)
	case GET_FLSTYLE:		*(ULONG *)data = flStyle;			break;
	case SET_FLSTYLE:		flStyle |= *(ULONG *)data;			break;
	case CLEAR_FLSTYLE:		flStyle &= ~(*(ULONG *)data);		break;

	case GET_FLFLAG:		*(ULONG *)data = flFlag;			break;
	case SET_FLFLAG:		flFlag |= *(ULONG *)data;			break;
	case CLEAR_FLFLAG:		flFlag &= ~(*(ULONG *)data);		break;
#endif

#if defined(ZIL_PERSISTENCE)
	case PRINT_INFORMATION:
		if (data && stringID[0] != '\0' && numberID < 0xFF00)
		{
			ui_strupr(stringID);
			UI_STORAGE_OBJECT *hppEntry = (UI_STORAGE_OBJECT *)data;
			char buffer[128];
			::sprintf(buffer, "const USHORT %-32s = 0x%04X;", stringID, numberID);
			hppEntry->Store(buffer);
		}
		break;
#endif

#if defined(ZIL_PERSISTENCE)
	case PRINT_USER_FUNCTION:
		if (userFunctionName && *userFunctionName)
		{
			UI_STORAGE_OBJECT *cppEntry = (UI_STORAGE_OBJECT *)data;
			cppEntry->Store(userFunctionName);
		}
		break;
#endif

	default:
		data = NULL;
		break;
	}

	// Return the information.
	return (data);
}

EVENT_TYPE UI_WINDOW_OBJECT::LogicalEvent(const UI_EVENT &event, OBJECTID currentID)
{
	// See if there is a matching identification.
	if (!currentID)
		currentID = windowID[0];
	OBJECTID id[5];
	for (int i = 0; i < 5; i++)
		if (currentID == windowID[i])
			break;
	id[0] = currentID;
	for (int j = 1; j < 5; j++)
		id[j] = (i < 4) ? windowID[++i] : ID_WINDOW_OBJECT;

	return (UI_EVENT_MAP::MapEvent(eventMapTable, event, id[0], id[1], id[2], id[3], id[4]));
}

UI_PALETTE *UI_WINDOW_OBJECT::LogicalPalette(LOGICAL_EVENT logicalEvent, OBJECTID currentID)
{
	// Determine the logical palette.
	LOGICAL_PALETTE logicalPalette = PM_ANY;
	if (FlagSet(woFlags, WOF_NON_SELECTABLE))
		logicalPalette = PM_NON_SELECTABLE;
	else if (logicalEvent == S_CURRENT && FlagSet(woStatus, WOS_CURRENT))
		logicalPalette = PM_CURRENT;
	else if (logicalEvent == S_CURRENT || logicalEvent == S_DISPLAY_ACTIVE ||
		logicalEvent == S_NON_CURRENT ||
		(parent && FlagSet(parent->woStatus, WOS_CURRENT)))
		logicalPalette = PM_ACTIVE;
	if (FlagSet(woStatus, WOS_SELECTED))
		logicalPalette |= PM_SELECTED;
	if (currentID == ID_HOT_KEY)
	{
		logicalPalette |= PM_HOT_KEY;
		currentID = 0;
	}

	// See if there is a matching identification.
	if (!currentID)
		currentID = windowID[0];
	OBJECTID id[5];
	for (int i = 0; i < 5; i++)
		if (currentID == windowID[i])
			break;
	id[0] = currentID;
	for (int j = 1; j < 5; j++)
		id[j] = (i < 4) ? windowID[++i] : ID_WINDOW_OBJECT;

	return (UI_PALETTE_MAP::MapPalette(paletteMapTable, logicalPalette, id[0], id[1], id[2], id[3], id[4]));
}

NUMBERID UI_WINDOW_OBJECT::NumberID(NUMBERID _numberID)
{
	if (_numberID)
		numberID = _numberID;
	return (numberID);
}

void UI_WINDOW_OBJECT::RegionConvert(UI_REGION &region, int absolute)
{
	// Check for a convertable region.
	if ((display->isText && !FlagSet(woFlags, WOF_MINICELL)) ||
		FlagSet(woStatus, WOS_GRAPHICS))
	{
		if (relative.right < relative.left && relative.right < 0)
			region.right += display->columns;
		if (relative.bottom < relative.top && relative.bottom < 0)
			region.bottom += display->lines;
		if (FlagSet(woStatus, WOS_GRAPHICS) && region.top == region.bottom)
		{
			if (Inherited(ID_BUTTON))
				region.top = region.bottom - display->cellHeight + display->preSpace + display->postSpace + 1;
			else
				region.bottom = region.top + display->cellHeight - display->preSpace - display->postSpace - 1;
		}
		return;
	}

	// Convert the coordinates.
	int cellWidth = display->cellWidth, cellHeight = display->cellHeight;
	if (FlagSet(woFlags, WOF_MINICELL))
	{
		long miniNX = display->miniNumeratorX, miniDX = display->miniDenominatorX;
		long miniNY = display->miniNumeratorY, miniDY = display->miniDenominatorY;
		if (display->isText)
		{
			int width = (int)(region.Width() * miniNX / miniDX) - 1;
			if (width < 0)
				width = 0;
			int height = (int)(region.Height() * miniNY / miniDY) - 1;
			if (height < 0)
				height = 0;
			region.left = (int)((miniNX * region.left) / miniDX);
			region.top = (int)((miniNY * region.top) / miniDY);
			region.right = region.left + width;
			region.bottom = region.top + height;
			woFlags &= ~WOF_MINICELL;
		}
		else
		{
/* START BLOCK COMMENT
**				int ceil = (region.left >= 0) ? (int)(miniDX - 1) : (int)(1 - miniDX);
**				region.left = (int)((miniNX * region.left * cellWidth + ceil) / miniDX);
**				ceil = (region.top >= 0) ? (int)(miniDY - 1) : (int)(1 - miniDY);
**				region.top = (int)((miniNY * region.top * cellHeight + ceil) / miniDY);
**				ceil = (region.right >= 0) ? (int)(miniDX - 1) : (int)(1 - miniDX);
**				region.right = (int)((miniNX * region.right * cellWidth + ceil) / miniDX);
**				ceil = (region.bottom >= 0) ? (int)(miniDY - 1) : (int)(1 - miniDY);
**				region.bottom = (int)((miniNY * region.bottom * cellHeight + ceil) / miniDY);
END BLOCK COMMENT */
			int width = (int)((region.Width() * miniNX * display->cellWidth) / miniDX);
			int height = (int)((region.Height() * miniNY * display->cellHeight) / miniDY);
			region.left = (int)((region.left * miniNX * display->cellWidth) / miniDX);
			region.top = (int)((region.top * miniNY * display->cellHeight) / miniDY);
			region.right = region.left + width - 1;
			region.bottom = region.top + height - 1;
		}
	}
	else
	{
		region.left = region.left * cellWidth;
		region.top = region.top * cellHeight;
		region.right = region.right * cellWidth + cellWidth - 1;
		region.bottom = region.bottom * cellHeight + cellHeight - 1;
	}

	// Check for absolute coordinates.
	if (!absolute && !FlagSet(woFlags, WOF_MINICELL))
	{
		region.top += display->preSpace;
		region.bottom -= display->postSpace;
	}

	if (!display->isText)
	{
		// Check for negative values.
		if (region.left < 0)
			region.left += display->columns;
		if (region.top < 0)
			region.top += display->lines;
		if (region.right < 0)
			region.right += display->columns;
		if (region.bottom < 0)
			region.bottom += display->lines;

		// Update the object status.
		woStatus |= WOS_GRAPHICS;
	}
}

void UI_WINDOW_OBJECT::RegionMax(UI_WINDOW_OBJECT *object)
{
	object->true = relative;
}

void UI_WINDOW_OBJECT::RegisterObject(char *className)
{
	// Call the object's event for actual registration.
	UI_EVENT event(S_REGISTER_OBJECT);
	event.data = className;
	Event(event);
}

char *UI_WINDOW_OBJECT::StringID(const char *_stringID)
{
	if (_stringID)
		strcpy(stringID, _stringID);
	return (stringID);
}

EVENT_TYPE UI_WINDOW_OBJECT::UserFunction(const UI_EVENT &event, EVENT_TYPE action)
{
	if (FlagSet(woStatus, WOS_INTERNAL_ACTION | WOS_EDIT_MODE))
		return (0);

	woStatus |= WOS_INTERNAL_ACTION;	// Prevent recursive calls.
	EVENT_TYPE ccode = action;
	if (ccode == L_SELECT && !FlagSet(woFlags, WOF_NON_SELECTABLE))
	{
		BTF_FLAGS btFlags = BTF_NO_FLAGS;
		Information(GET_FLAGS, &btFlags, ID_BUTTON);
		if (parent && !FlagSet(btFlags, BTF_NO_TOGGLE))
		{
			WNF_FLAGS wnFlags = WNF_NO_FLAGS;
			parent->Information(GET_FLAGS, &wnFlags, ID_WINDOW);
			if (FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
				woStatus ^= WOS_SELECTED;
			else
				woStatus |= WOS_SELECTED;
		}
	}

	if (userFunction)
	{
		UI_EVENT uEvent = event;
		ccode = (*userFunction)(this, uEvent, ccode);
	}
	else if (ccode != S_CURRENT)
		ccode = (Validate(TRUE) == 0) ? 0 : -1;
	woStatus &= ~WOS_INTERNAL_ACTION;
	if (parent && action == L_SELECT)
		parent->Information(CHECK_SELECTION, NULL);
	return (ccode);
}

int UI_WINDOW_OBJECT::Validate(int)
{
	return (0);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_PERSISTENCE)
UI_WINDOW_OBJECT::UI_WINDOW_OBJECT(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	eventMapTable(NULL), hotKeyMapTable(NULL), paletteMapTable(NULL)
{
	// Intialize the window object information.
	userFlags = UIF_NO_FLAGS;
	userStatus = UIS_NO_FLAGS;
	userObject = NULL;
	userFunction = NULL;
	helpContext = NO_HELP_CONTEXT;
	Load(name, directory, file);
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
}

#if defined(__HIGHC__)
void UI_WINDOW_OBJECT::Load(const char *dummy1, UI_STORAGE *dummy2, UI_STORAGE_OBJECT *file)
#else
void UI_WINDOW_OBJECT::Load(const char *, UI_STORAGE *, UI_STORAGE_OBJECT *file)
#endif
{
	short value;
#if defined(CONVERT_V200)
	struct EXPORT UI_SEARCH_INFO
	{
		// Fields described in UI_SEARCH_ELEMENT reference chapter.
		USHORT type, numberID;
		char stringID[32];
		long offset;
		USHORT size;
	} search;

	file->Load(&search, sizeof(search));
	numberID = search.numberID;
	strcpy(string, search.stringID);
#else
	file->Load(&numberID);
	file->Load(stringID, 32);
#endif
	file->Load(&woFlags);
//	file->Load(&woAdvancedFlags);											// BUG.1210B4B
#if defined (ZIL_EDIT)
	if (FlagSet(defaultStatus, WOS_EDIT_MODE))
		file->Load(&designerAdvancedFlags);
	else
		file->Load(&woAdvancedFlags);
#else
	file->Load(&woAdvancedFlags);
#endif
	file->Load(&value); relative.left = value;
	file->Load(&value); relative.top = value;
	file->Load(&value); relative.right = value;
	file->Load(&value); relative.bottom = value;
	true = relative;
	file->Load(&value); helpContext = value;
	file->Load(&userFlags);
	file->Load(&userStatus);
	file->Load(&userObjectName);
	if (userObjectName && userTable)
	{
		for (int i = 0; !userObject && userTable[i].data; i++)
			if (ui_stricmp(userTable[i].text, userObjectName) == 0)
				userObject = userTable[i].data;
	}
	file->Load(&userFunctionName);
	if (userFunctionName && userTable)
	{
		for (int i = 0; !userFunction && userTable[i].data; i++)
			if (ui_stricmp(userTable[i].text, userFunctionName) == 0)
				userFunction = (USER_FUNCTION)userTable[i].data;
	}
}

UI_WINDOW_OBJECT *UI_WINDOW_OBJECT::New(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	UI_WINDOW_OBJECT *object = NULL;

	// Check for a valid directory and file.
	int tempDirectory = FALSE;
	if (name && !file)
	{
		char pathName[128], fileName[32], objectName[32], objectPathName[128];
		UI_STORAGE::StripFullPath(name, pathName, fileName, objectName, objectPathName);
		if (!directory)
		{
			UI_STORAGE::AppendFullPath(pathName, pathName, fileName);
			UI_STORAGE::ChangeExtension(pathName, ZIL_EXT);
			directory = new UI_STORAGE(pathName, UIS_READ);
			if (directory->storageError)
			{
				delete directory;
				return (NULL);
			}
			tempDirectory = TRUE;
		}
		if (!file)
		{
			object = new UIW_WINDOW(name, directory, NULL);
			if (directory->storageError)
			{
				delete object;
				object = NULL;
			}
			if (tempDirectory)
				delete directory;
			return (object);
		}
	}

	// Read the object based on its identification.
	short searchID;
	file->Load(&searchID);
	for (int i = 0; objectTable[i].data; i++)
		if (objectTable[i].value == searchID)
		{
			NEW_FUNCTION newFunction = (NEW_FUNCTION)objectTable[i].data;
			object = newFunction(NULL, directory, file);
			object->searchID = searchID;
		}

	// Clean up the file and storage then return the object.
	if (tempDirectory)
		delete directory;
	if (object)
		return (object);

	// Error report for unknown object types.
	return (NULL);
}

#if defined(__HIGHC__)
void UI_WINDOW_OBJECT::Store(const char *dummy1, UI_STORAGE *dummy2, UI_STORAGE_OBJECT *file)
#else
void UI_WINDOW_OBJECT::Store(const char *, UI_STORAGE *, UI_STORAGE_OBJECT *file)
#endif
{
	// Mark the object as used.
	if (objectTable)
	{
		for (int i = 0; objectTable[i].data; i++)
			if (objectTable[i].value == searchID)
			{
				objectTable[i].flags = TRUE;
				break;
			}
	}

	// Store the object information.
	file->Store(numberID);
	file->Store(stringID);
	file->Store(woFlags);
//	file->Store(woAdvancedFlags);											// BUG.1210B4B
#if defined (ZIL_EDIT)
	if (FlagSet(woStatus, WOS_EDIT_MODE))
		file->Store(designerAdvancedFlags);
	else
		file->Store(woAdvancedFlags);
#else
	file->Store(woAdvancedFlags);
#endif
	UI_REGION region;
#if defined(ZIL_MOTIF)
		region = relative;
#else
	if (parent)
	{
		region = relative;
		if (FlagSet(woStatus, WOS_GRAPHICS) &&								// BUG.General
			!FlagSet(woFlags, WOF_NON_FIELD_REGION) && 
			!FlagSet(woFlags, WOF_MINICELL))
		{
			region.top -= display->preSpace;
			region.bottom += display->postSpace;
		}
	}
	else
		region = true;
#endif
	if (!display->isText && FlagSet(woStatus, WOS_GRAPHICS) && FlagSet(woFlags, WOF_MINICELL))
	{
		long miniNX = display->miniNumeratorX, miniDX = display->miniDenominatorX;
		long miniNY = display->miniNumeratorY, miniDY = display->miniDenominatorY;
/* START BLOCK COMMENT
**			region.left = (int)((miniDX * region.left) / (miniNX * display->cellWidth));
**			region.top = (int)((miniDY * region.top) / (miniNY * display->cellHeight));
**			region.right = (int)((miniDX * region.right) / (miniNX * display->cellWidth));
**			region.bottom = (int)((miniDY * region.bottom) / (miniNY * display->cellHeight));
END BLOCK COMMENT */
		int width = (int)((region.Width() * miniDX) / (miniNX * display->cellWidth));
		int height = (int)((region.Height() * miniDY) / (miniNY * display->cellHeight));
		region.left = (int)((region.left * miniDX) / (miniNX * display->cellWidth));
		region.bottom++;
		region.bottom = (int)((region.bottom * miniDY) / (miniNY * display->cellHeight));
		region.bottom--;
		region.right = region.left + width - 1;
		region.top = region.bottom - height + 1;
	}
	else if (!display->isText && FlagSet(woStatus, WOS_GRAPHICS))
	{
/* START BLOCK COMMENT														// BUG.General
**			int width = (region.right - region.left) / display->cellWidth;
**			int height = (region.bottom - region.top) / display->cellHeight;
**			region.left /= display->cellWidth;
**			region.bottom /= display->cellHeight;
**			region.right = region.left + width;
**			region.top = region.bottom - height;
END BLOCK COMMENT */
		int width = region.Width() / display->cellWidth;
		int height = region.Height() / display->cellHeight;
		region.left /= display->cellWidth;
		region.bottom++;
		region.bottom /= display->cellHeight;
		region.bottom--;
		region.right = region.left + width - 1;
		region.top = region.bottom - height + 1;
	}
	short value = region.left; file->Store(value);
	value = region.top; file->Store(value);
	value = region.right; file->Store(value);
	value = region.bottom; file->Store(value);
	value = helpContext; file->Store(value);
	file->Store(userFlags);
	file->Store(userStatus);
	if (userObject && !userObjectName && userTable)
	{
		for (int i = 0; !userObjectName && userTable[i].data; i++)
			if (userTable[i].data == userObject)
				userObjectName = ui_strdup(userTable[i].text);
	}
	file->Store(userObjectName);
	if (userFunction && !userFunctionName && userTable)
	{
		for (int i = 0; !userFunctionName && userTable[i].data; i++)
			if (userTable[i].data == (void *)userFunction)
				userFunctionName = ui_strdup(userTable[i].text);
	}
	file->Store(userFunctionName);
}
#endif


