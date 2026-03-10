//	Zinc Interface Library - Z_TBAR.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_TOOL_BAR -------------------------------------------------------

UIW_TOOL_BAR::UIW_TOOL_BAR(int left, int top, int width, int height,
	WNF_FLAGS _wnFlags, WOF_FLAGS _woFlags, WOAF_FLAGS _woAdvancedFlags) :
	UIW_WINDOW(left, top, width, height, _woFlags, _woAdvancedFlags)
{
	// Initialize the menu information.
	wnFlags |= _wnFlags;
	UIW_TOOL_BAR::Information(INITIALIZE_CLASS, NULL);
}

UIW_TOOL_BAR::~UIW_TOOL_BAR(void)
{
}

void *UIW_TOOL_BAR::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_TOOL_BAR;
	switch (request)
	{
	case INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = windowID[0] = ID_TOOL_BAR;
		windowID[1] = ID_WINDOW;
		if (!hotKey)														// BUG.1709
			hotKey = HOT_KEY_SUB_WINDOW;
		// Continue to CHANGED_FLAGS.

	case CHANGED_FLAGS:
		// Check the object and base class flag settings.
		if (request == CHANGED_FLAGS)
			UIW_WINDOW::Information(CHANGED_FLAGS, data, ID_TOOL_BAR);

		// See if the field needs to be re-computed.
		if (objectID == ID_TOOL_BAR && FlagSet(woStatus, WOS_REDISPLAY))
		{
			UI_EVENT event(S_INITIALIZE, 0);
			Event(event);
			event.type = S_CREATE;
			Event(event);
		}
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
UIW_TOOL_BAR::UIW_TOOL_BAR(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	UIW_WINDOW(0, 0, 20, 6, WOF_NO_FLAGS)
{
	// Initialize the text information.
	UIW_TOOL_BAR::Load(name, directory, file);
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
	UIW_WINDOW::Information(INITIALIZE_CLASS, NULL);
	UIW_TOOL_BAR::Information(INITIALIZE_CLASS, NULL);
}

void UIW_TOOL_BAR::Load(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Load the toolbar information.
	UIW_WINDOW::Load(name, directory, file);
}

void UIW_TOOL_BAR::Store(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Store the toolbar information.
	UIW_WINDOW::Store(name, directory, file);
}
#endif

