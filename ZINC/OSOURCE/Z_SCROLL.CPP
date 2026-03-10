//	Zinc Interface Library - Z_SCROLL.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define INCL_WINMLE				// OS/2 multi-line messages and flags.
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_SCROLL_BAR -----------------------------------------------------

UIW_SCROLL_BAR::UIW_SCROLL_BAR(int left, int top, int width, int height,
	SBF_FLAGS _sbFlags, WOF_FLAGS _woFlags) :
	UIW_WINDOW(left, top, width, height, _woFlags), sbFlags(_sbFlags)
{
	// Initialize the slide information.
	UIW_SCROLL_BAR::Information(INITIALIZE_CLASS, NULL);

#if defined(ZIL_MSWINDOWS)
	if (FlagSet(sbFlags, SBF_VERTICAL) && !width)
		dwStyle |= SBS_LEFTALIGN;
	else if (FlagSet(sbFlags, SBF_HORIZONTAL) && !height)
		dwStyle |= SBS_TOPALIGN;
	else if (FlagSet(sbFlags, SBF_CORNER) && !width && !height)
		dwStyle |= SBS_SIZEBOXTOPLEFTALIGN;
#endif
}

UIW_SCROLL_BAR::UIW_SCROLL_BAR(int left, int top, int width, int height,
	UI_SCROLL_INFORMATION *_scroll, SBF_FLAGS _sbFlags,
	WOF_FLAGS _woFlags, WOAF_FLAGS _woAdvancedFlags, USER_FUNCTION _userFunction) :
	UIW_WINDOW(left, top, width, height, _woFlags, _woAdvancedFlags),
	sbFlags(_sbFlags)
{
	// Initialize the slide information.
	UIW_SCROLL_BAR::Information(INITIALIZE_CLASS, NULL);
	scroll = *_scroll;
	userFunction = _userFunction;
}
	

UIW_SCROLL_BAR::~UIW_SCROLL_BAR(void)
{
	// No scroll-bar information needs to be destroyed.
}

void *UIW_SCROLL_BAR::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_SCROLL_BAR;
	switch (request)
	{
	case INITIALIZE_CLASS:
		// Set the object identification and variables.
#if defined(ZIL200_PERSISTENCE)
		if (FlagSet(woFlags, WOF_NON_FIELD_REGION))
			woFlags |= WOF_SUPPORT_OBJECT;
#endif
		if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
			woAdvancedFlags |= WOAF_NON_CURRENT;
		if (FlagSet(sbFlags, SBF_CORNER | SBF_HORIZONTAL))
			woFlags |= WOF_JUSTIFY_RIGHT;
		searchID = windowID[0] = ID_SCROLL_BAR;
		windowID[1] = ID_WINDOW;
		scroll.minimum = scroll.current = 0;
		scroll.maximum = 1;
		// Continue to CHANGED_FLAGS.

	case CHANGED_FLAGS:
		if (request == CHANGED_FLAGS)
			UIW_WINDOW::Information(CHANGED_FLAGS, data, ID_SCROLL_BAR);

#if defined(ZIL_MSDOS)
		// Delete old buttons if necessary.		 							// BUG.1305B5B
		if (request == CHANGED_FLAGS && screenID)
		{
			Subtract(lButton);
			delete lButton;
			Subtract(mButton);
			delete mButton;
			Subtract(rButton);
			delete rButton;
		}
		// Add the scroll buttons.
		if (FlagSet(sbFlags, SBF_VERTICAL))									// BUG.1305B5B (moved from INITIALIZE_CLASS)
		{
			Add((lButton = new UIW_SCROLL_BUTTON(VTOP_BUTTON)));
			Add((mButton = new UIW_SCROLL_BUTTON(VMIDDLE_BUTTON)));
			Add((rButton = new UIW_SCROLL_BUTTON(VBOTTOM_BUTTON)));
		}
		else if (FlagSet(sbFlags, SBF_HORIZONTAL))
		{
			Add((lButton = new UIW_SCROLL_BUTTON(HLEFT_BUTTON)));
			Add((mButton = new UIW_SCROLL_BUTTON(HMIDDLE_BUTTON)));
			Add((rButton = new UIW_SCROLL_BUTTON(HRIGHT_BUTTON)));
		}
#endif
		if (!parent)
			break;

		if (FlagsSet(woFlags, WOF_SUPPORT_OBJECT))
		{
#if defined(ZIL_MSWINDOWS)
			parent->Information(SET_DWSTYLE, &dwStyle);
#elif defined(ZIL_OS2)
			parent->Information(SET_FLFLAG, &flFlag);
#endif
			break;
		}

		// See if the field needs to be re-computed.
		if (objectID == ID_SCROLL_BAR && FlagSet(woStatus, WOS_REDISPLAY))
		{
			UI_EVENT event(S_INITIALIZE, 0);								// BUG.1305B5B
			Event(event);
			event.type = S_CREATE;
			Event(event);
		}
		break;

	case GET_FLAGS:
	case SET_FLAGS:
	case CLEAR_FLAGS:
		// Get, set or clear the flag settings.
		if (objectID && objectID != ID_SCROLL_BAR)
			data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		else if (request == GET_FLAGS && !data)
			data = &sbFlags;
		else if (request == GET_FLAGS)
			*(SBF_FLAGS *)data = sbFlags;
		else if (request == SET_FLAGS)
			sbFlags |= *(SBF_FLAGS *)data;
		else
			sbFlags &= ~(*(SBF_FLAGS *)data);
		break;

	case SET_VALUE:
		scroll.current = *(int *)data;
#if defined(ZIL_MSWDOS)
#elif defined(ZIL_MSWINDOWS)
#elif defined(ZIL_OS2)
#elif defined(ZIL_MOTIF)
		if (screenID)
			XtVaSetValues(screenID, XmNvalue, scroll.current, NULL);
#endif
		break;

	case GET_VALUE:
#if defined(ZIL_MSWDOS)
#elif defined(ZIL_MSWINDOWS)
#elif defined(ZIL_OS2)
#elif defined(ZIL_MOTIF)
		if (screenID)
			XtVaGetValues(screenID, XmNvalue, &scroll.current, NULL);
#endif
		*(int *)data = scroll.current; 
		break;

	case PRINT_INFORMATION:
	case PRINT_USER_FUNCTION:
		UI_WINDOW_OBJECT::Information(request, data, objectID);
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
UIW_SCROLL_BAR::UIW_SCROLL_BAR(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	UIW_WINDOW(0, 0, 2, 6, WOF_NO_FLAGS)
{
	// Initialize the slide information.
	Load(name, directory, file);
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
	UIW_WINDOW::Information(INITIALIZE_CLASS, NULL);
	UIW_SCROLL_BAR::Information(INITIALIZE_CLASS, NULL);
}

void UIW_SCROLL_BAR::Load(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Load the scroll-bar information.
	UI_WINDOW_OBJECT::Load(name, directory, file);
	file->Load(&sbFlags);
	short _value; file->Load(&_value); scroll.minimum = _value;
	file->Load(&_value); scroll.maximum = _value;
	file->Load(&_value); scroll.current = _value;
}

void UIW_SCROLL_BAR::Store(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Store the scroll-bar information.
	UI_WINDOW_OBJECT::Store(name, directory, file);
	file->Store(sbFlags);
	short _value = scroll.minimum; file->Store(_value);
	_value = scroll.maximum; file->Store(_value);
	_value = scroll.current; file->Store(_value);
}
#endif

