//	Zinc Interface Library Designer - Z_HELP1.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
#include <mem.h>
#endif
#include <string.h>
#include "ui_dsn.hpp"
#if defined(_MSC_VER)
#pragma hdrstop
#endif

EVENT_TYPE ContextNameFunction(UI_WINDOW_OBJECT *object, UI_EVENT &event,
	EVENT_TYPE ccode)
{
	if (object->LogicalEvent(event, ID_WINDOW_OBJECT) == L_SELECT)
		object->eventManager->Put(UI_EVENT(OPTION_OK, 0));
	return (ccode);
}

static EVENT_TYPE ContextSelect(UI_WINDOW_OBJECT *object,
	UI_EVENT &event, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		UI_EVENT tEvent(IMAGE_SELECT);
		object->Information(GET_TEXT, &tEvent.data);
		object->eventManager->Put(tEvent);
	}
	if (ccode == L_DOUBLE_CLICK || event.type == L_SELECT)
	{
		event.type = OPTION_OK;
		object->eventManager->Put(event);
	}
	return (ccode);
}

CONTEXT_WINDOW::CONTEXT_WINDOW(USER_EVENT _mode, HELP_EDITOR *_editor) :
	UIW_WINDOW(display->isText ? "CONTEXT_WINDOW_TEXT" : "CONTEXT_WINDOW",
		defaultStorage), editor(_editor), mode(_mode)
{
	char *title;
	if (mode == OPTION_NEW)
		title = "New";
	else if (mode == OPTION_LOAD)
		title = "Load";
	else if (mode == OPTION_SAVE_AS)
		title = "Save As";
	else if (mode == OPTION_DELETE)
		title = "Delete";
	((UIW_TITLE *)Information(GET_STRINGID_OBJECT, "NUMID_TITLE"))->Information(SET_TEXT, title);
	helpBar = (HELP_BAR *)Information(GET_STRINGID_OBJECT, "HELP_BAR");

	nameField = (UIW_STRING *)Information(GET_STRINGID_OBJECT, "NAME_FIELD");
	if (mode == OPTION_SAVE_AS)
	{
		nameField->Information(SET_TEXT, _editor->helpName);
	}
	contextMatrix = (UIW_HZ_LIST *)Information(GET_STRINGID_OBJECT, "CONTEXT_LIST");

	eventManager->DeviceState(E_MOUSE, DM_WAIT);
	contextMatrix->Destroy();
	CONTROL_WINDOW::storage->ChDir("~UI_HELP");
	char *objectName = CONTROL_WINDOW::storage->FindFirstObject("*");
	while (objectName)
	{
		if (objectName[0] != '.')
		{
			UI_STORAGE_OBJECT sObject(*CONTROL_WINDOW::storage, objectName,
				ID_HELP_CONTEXT, UIS_READ);
			char *buffer = NULL;
			sObject.Load(&buffer);
			UIW_BUTTON *button = new UIW_BUTTON(0, 0, 30, objectName,
				BTF_NO_3D | BTF_NO_TOGGLE, WOF_NO_FLAGS, ContextSelect);
			(void *)&(*contextMatrix + button);
			delete buffer;
		}
		CONTROL_WINDOW::storage->ChDir("~UI_HELP");
		objectName = CONTROL_WINDOW::storage->FindNextObject();
	}
	if (!contextMatrix->First())
		(void *)&(*contextMatrix + new UIW_STRING(0, 0, 30, "(None)"));
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
}


EVENT_TYPE CONTEXT_WINDOW::Event(const UI_EVENT &event)
{
	// Switch on the type of event.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case IMAGE_SELECT:
		nameField->Information(SET_TEXT, event.data);
		break;

	case OPTION_OK:
		{
		char *name = nameField->DataGet();
		if (name && *name == '\0')
			name = NULL;

		// Test for valid name.
		if (name)
		{
			char *title = (char *)Information(GET_TEXT, NULL);
			char *objectName = CONTROL_WINDOW::storage->FindFirstObject(name);
			if (objectName && (mode == OPTION_NEW || mode == OPTION_SAVE_AS))
			{
				DIALOG_WINDOW dialog(title, "QUESTION", DIF_YES | DIF_NO,
					"Context %s already exists.\r\n\r\nOverwrite it?", name);
				if (dialog.Responce() != DIALOG_YES)
					break;
			}
			else if (!objectName && (mode == OPTION_LOAD || mode == OPTION_DELETE))
			{
				DIALOG_WINDOW dialog(title, "EXCLAMATION", DIF_OK,
					"%s\r\n\r\nContext not found!", name);
				dialog.Responce();
				break;
			}
		}
		else if (mode != OPTION_NEW)
		{
			errorSystem->Beep();
			break;
		}

		ReplaceString(&editor->helpName, name);
		if (mode == OPTION_LOAD || mode == OPTION_NEW)
			editor->LoadContext();
		else if (mode == OPTION_SAVE_AS)
			eventManager->Put(UI_EVENT(OPTION_SAVE, 0));
		else
		{
			DIALOG_WINDOW dialog("Delete Context", "EXCLAMATION", DIF_OK | DIF_CANCEL,
				"%s\r\n\r\nThis help context will be deleted", name);
			if (dialog.Responce() == DIALOG_OK)
			{
				CONTROL_WINDOW::storage->ChDir("~UI_HELP");
				CONTROL_WINDOW::storage->DestroyObject(name);
				CONTROL_WINDOW::saveNeeded = TRUE;
			}
		}
		eventManager->Put(UI_EVENT(S_CLOSE), Q_BEGIN);
		}
		break;

	case OPTION_CANCEL:
		eventManager->Put(UI_EVENT(S_CLOSE, 0));
		break;

	case OPTION_HELP:
		helpSystem->DisplayHelp(windowManager, helpContext);
		break;

 	case S_INITIALIZE:
	case S_CREATE:
		ccode = UIW_WINDOW::Event(event);
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		helpBar->Update(Current());
		break;
	}
	return ccode;
}
