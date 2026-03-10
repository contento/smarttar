//	Zinc Interface Library - Z_BORDER.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_BORDER ---------------------------------------------------------

UIW_BORDER::UIW_BORDER(BDF_FLAGS _bdFlags) :
	UI_WINDOW_OBJECT(0, 0, 0, 0, WOF_SUPPORT_OBJECT | WOF_NON_FIELD_REGION,
	WOAF_OUTSIDE_REGION | WOAF_NON_CURRENT), bdFlags(_bdFlags)
{
#if defined(ZIL_LINKBUG)
	extern void z_border_dummy(void);	// Bug fix for broken linkers.
	z_border_dummy();
#endif

	// Initialize the border information.
	UIW_BORDER::Information(INITIALIZE_CLASS, NULL);
}

UIW_BORDER::~UIW_BORDER(void)
{
	// No border information needs to be destroyed.
}

int UIW_BORDER::DataGet(void)
{
	// Get the border width from the native environment.
#if defined(ZIL_MSDOS)
	// The width is always accurate.
#elif defined(ZIL_MSWINDOWS)
	width = GetSystemMetrics(SM_CXFRAME);
#elif defined(ZIL_OS2)
	width = WinQuerySysValue(HWND_DESKTOP, SV_CXSIZEBORDER);
#endif

	// Return the default border width.
	return (width);
}

void UIW_BORDER::DataSet(int newWidth)
{
	// Reset the default border width.
	width = (newWidth > 0) ? newWidth : 1;

	// Set the border width in the native environment.
#if defined(ZIL_MSDOS)
	if (eventManager)
		eventManager->Put(UI_EVENT(S_REDISPLAY));
#elif defined(ZIL_MSWINDOWS)
	// Windows does not allow the programmer to set the border width.
#elif defined(ZIL_OS2)
	WinSetSysValue(HWND_DESKTOP, SV_CXSIZEBORDER, (LONG)newWidth);
	WinSetSysValue(HWND_DESKTOP, SV_CYSIZEBORDER, (LONG)newWidth);
#endif
}

void *UIW_BORDER::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_BORDER;
	switch (request)
	{
	case INITIALIZE_CLASS:
		searchID = windowID[0] = ID_BORDER;
		numberID = NUMID_BORDER;
		strcpy(stringID, "NUMID_BORDER");
		woAdvancedFlags |= WOAF_NON_CURRENT;
		break;

#if defined(ZIL_MSWINDOWS)
	case CHANGED_FLAGS:
		parent->Information(SET_DWSTYLE, &dwStyle);
		break;
#elif defined(ZIL_OS2)
	case CHANGED_FLAGS:
		parent->Information(SET_FLFLAG, &flFlag);
		break;
#endif

	default:
		data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_PERSISTENCE)
UIW_BORDER::UIW_BORDER(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	UI_WINDOW_OBJECT(0, 0, 0, 0, WOF_SUPPORT_OBJECT | WOF_NON_FIELD_REGION,
	WOAF_OUTSIDE_REGION | WOAF_NON_CURRENT)
{
	// Initialize the border information.
	UIW_BORDER::Load(name, directory, file);
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
	UIW_BORDER::Information(INITIALIZE_CLASS, NULL);
}

void UIW_BORDER::Load(const char *, UI_STORAGE *, UI_STORAGE_OBJECT *)
{
	// No border information needs to be loaded.
}

void UIW_BORDER::Store(const char *, UI_STORAGE *, UI_STORAGE_OBJECT *)
{
	// Flag the object as being used.
	if (objectTable)
	{
		for (int i = 0; objectTable[i].data; i++)
			if (objectTable[i].value == searchID)
			{
				objectTable[i].flags = TRUE;
				break;
			}
	}
}
#endif


