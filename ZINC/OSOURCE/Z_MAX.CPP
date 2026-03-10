//	Zinc Interface Library - Z_MAX.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_MAXIMIZE_BUTTON ------------------------------------------------

UIW_MAXIMIZE_BUTTON::UIW_MAXIMIZE_BUTTON(void) :
	UIW_BUTTON(0, 0, 0, NULL, BTF_SEND_MESSAGE | BTF_NO_TOGGLE | BTF_STATIC_BITMAPARRAY,
	WOF_BORDER | WOF_JUSTIFY_CENTER | WOF_SUPPORT_OBJECT | WOF_NON_FIELD_REGION,
	NULL, L_MAXIMIZE)
{
	// Initialize the maximize button information.
	UIW_MAXIMIZE_BUTTON::Information(INITIALIZE_CLASS, NULL);
}

UIW_MAXIMIZE_BUTTON::~UIW_MAXIMIZE_BUTTON(void)
{
	// Restore the maximize button information.
	text = NULL;			// text is a static string.
	bitmapArray = NULL;		// bitmapArray is a static array.
}

void *UIW_MAXIMIZE_BUTTON::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_MAXIMIZE_BUTTON;
	switch (request)
	{
	case INITIALIZE_CLASS:
		// Set the object identification and initial button settings.
		searchID = windowID[0] = ID_MAXIMIZE_BUTTON;
		windowID[1] = ID_BUTTON;
		numberID = NUMID_MAXIMIZE;
		strcpy(stringID, "NUMID_MAXIMIZE");
		woAdvancedFlags |= WOAF_NON_CURRENT;
		btFlags |= BTF_SEND_MESSAGE | BTF_NO_TOGGLE;
		hotKey = HOT_KEY_MAXIMIZE;
		depth = 1;
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
		data = UIW_BUTTON::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_PERSISTENCE)
UIW_MAXIMIZE_BUTTON::UIW_MAXIMIZE_BUTTON(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	UIW_BUTTON(0, 0, 0, NULL, BTF_SEND_MESSAGE,
	WOF_BORDER | WOF_JUSTIFY_CENTER | WOF_SUPPORT_OBJECT | WOF_NON_FIELD_REGION,
	NULL, L_MAXIMIZE)
{
	// Initialize the maximize button information.
	UIW_MAXIMIZE_BUTTON::Load(name, directory, file);
	UIW_MAXIMIZE_BUTTON::Information(INITIALIZE_CLASS, NULL);
}

void UIW_MAXIMIZE_BUTTON::Load(const char *, UI_STORAGE *, UI_STORAGE_OBJECT *)
{
}

void UIW_MAXIMIZE_BUTTON::Store(const char *, UI_STORAGE *, UI_STORAGE_OBJECT *)
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


