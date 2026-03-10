//	Zinc Interface Library - Z_FMTSTR.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_FORMATTED_STRING -----------------------------------------------

UIW_FORMATTED_STRING::UIW_FORMATTED_STRING(int left, int top, int width,
	char *_compressedText, char *_editMask, char *_deleteText,
	WOF_FLAGS _woFlags, USER_FUNCTION _userFunction) :
	UIW_STRING(left, top, width, NULL, 64, STF_NO_FLAGS, _woFlags, _userFunction)
{
	// Initialize the formatted string information.
	if (FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
	{
		text = new char[65];
		compressedText = _compressedText;
	}
	else
	{
		compressedText = new char[64];
		strcpy(compressedText, _compressedText ? _compressedText : "");
	}
	editMask = new char[64];
	strcpy(editMask, _editMask ? _editMask : "");
	deleteText = new char[64];
	strcpy(deleteText, _deleteText ? _deleteText : "");
	UIW_FORMATTED_STRING::Information(INITIALIZE_CLASS, NULL);
}

UIW_FORMATTED_STRING::~UIW_FORMATTED_STRING(void)
{
	// Restore the formatted string information.
	if (FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete text;
	else
		delete compressedText;
	delete editMask;
	delete deleteText;
}

char *UIW_FORMATTED_STRING::DataGet(int _compressedText)
{
	// Return the formatted string.
	UIW_STRING::DataGet();
	return (_compressedText ? compressedText : text);
}

void UIW_FORMATTED_STRING::DataSet(char *_text)
{
	// Reset the formatted string.
	if (_text == compressedText || FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		compressedText = _text;
	Import(_text);
	Export(text, TRUE);
	UIW_STRING::DataSet(text);
}

EVENT_TYPE UIW_FORMATTED_STRING::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;

	// Check for environment specific messages.
#if defined(ZIL_MSWINDOWS)
	if (ccode == E_MSWINDOWS && (event.message.message < WM_KEYFIRST ||
		event.message.message > WM_KEYLAST))
		return (UIW_STRING::Event(event));
#elif defined(ZIL_OS2)
	if (ccode == E_OS2 && event.message.msg != WM_CHAR)
		return (UIW_STRING::Event(event));
#endif

	// Check for Zinc specific messages.
	ccode = LogicalEvent(event, ID_FORMATTED_STRING);
	switch (ccode)
	{
	case L_SELECT:
	case S_NON_CURRENT:
		if (!FlagSet(woStatus, WOS_INTERNAL_ACTION))
		{
			UIW_STRING::DataGet();
			Import(text);
			Export(text, TRUE);
			UIW_STRING::DataSet(text);
		}
		ccode = UIW_STRING::Event(event);
		break;

	case S_CREATE:
		Export(text, TRUE);
		ccode = UIW_STRING::Event(event);
		break;

	default:
		ccode = UIW_STRING::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void UIW_FORMATTED_STRING::Export(char *destination, int expanded)
{
	// Export the formatted string value.
	if (expanded)
	{
		char *source = compressedText;
		for (int i = 0; editMask[i]; i++)
			if (editMask[i] != 'L' && *source)
				*destination++ = *source++;
			else
				*destination++ = deleteText[i];
		*destination = '\0';
	}
	else
		strcpy(destination, compressedText);
}

FMI_RESULT UIW_FORMATTED_STRING::Import(char *source)
{
	// Import the formatted string value.
	char *destination = compressedText;
	char *tdeleteText = deleteText;
	for (char *mask = editMask; *mask && *source; mask++ && tdeleteText++)
		if (*mask != 'L')
		{
			for (int processed = FALSE; !processed && *source; source++)
			{
				int editValue = *mask;
				int textValue = *source;
				int isAlpha = (textValue == ' ' || isalpha(textValue));
				int isDigit = isdigit(textValue);
				if (isupper(editValue))
					textValue = toupper(textValue);
				editValue = toupper(editValue);
				if ((editValue == 'X' && textValue >= ' ' && textValue <= '~') ||
					(editValue == 'N' && isDigit) ||
					(editValue == 'A' && isAlpha) ||
					(editValue == 'C' && (isDigit || isAlpha)))
				{
					*destination++ = *source = textValue;
					processed = TRUE;
				}
			}
		}
		else if (*tdeleteText == *source)
			source++;

	*destination = '\0';
	Export(text, TRUE);

	// Return success.
	return (FMI_OK);
}

void *UIW_FORMATTED_STRING::Information(INFO_REQUEST request, void *data, OBJECTID objectID)
{
	// Switch on the request.
	if (!objectID) objectID = ID_FORMATTED_STRING;
	switch (request)
	{
	case INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = windowID[0] = ID_FORMATTED_STRING;
		windowID[1] = ID_STRING;
		break;

	case CHANGED_FLAGS:
		// Check the object and base class flag settings.
		UIW_STRING::Information(CHANGED_FLAGS, data, ID_FORMATTED_STRING);

		// See if the field needs to be re-computed.
		if (objectID == ID_FORMATTED_STRING && FlagSet(woStatus, WOS_REDISPLAY))
		{
			UI_EVENT event(S_INITIALIZE, 0);
			Event(event);
			event.type = S_CREATE;
			Event(event);
		}
		break;

	case SET_TEXT:
		Export(text, TRUE);
		UIW_FORMATTED_STRING::DataSet((char *)data);
		break;

	default:
		data = UIW_STRING::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_PERSISTENCE)
UIW_FORMATTED_STRING::UIW_FORMATTED_STRING(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	UIW_STRING(0, 0, 10, NULL, 64, STF_NO_FLAGS, WOF_NO_FLAGS)
{
	// Initialize the formatted string information.
	compressedText = new char[64];
	editMask = new char[64];
	deleteText = new char[64];
	UIW_FORMATTED_STRING::Load(name, directory, file);
	UI_WINDOW_OBJECT::Information(INITIALIZE_CLASS, NULL);
	UIW_STRING::Information(INITIALIZE_CLASS, NULL);
	UIW_FORMATTED_STRING::Information(INITIALIZE_CLASS, NULL);
}

void UIW_FORMATTED_STRING::Load(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Load the formatted string information.
	UIW_STRING::Load(name, directory, file);
	file->Load(compressedText, 64);
	file->Load(editMask, 64);
	file->Load(deleteText, 64);
}

void UIW_FORMATTED_STRING::Store(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file)
{
	// Store the formatted string information.
	UIW_STRING::Store(name, directory, file);
	file->Store(compressedText);
	file->Store(editMask);
	file->Store(deleteText);
}
#endif


