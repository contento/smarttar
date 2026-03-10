//	Zinc Interface Library Designer - Z_HELPED.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <string.h>
#include <stdio.h>
#define UIF_ADVANCED					UIW_ADVANCED_FRAME
#include "ui_dsn.hpp"
#if defined(_MSC_VER)
#pragma hdrstop
#endif

HELP_EDITOR::HELP_EDITOR(char * _helpName) :
	UIW_WINDOW(display->isText ? "HELP_EDITOR_TEXT" : "HELP_EDITOR",
		defaultStorage), textField(NULL), helpName(NULL)
{
	ReplaceString(&helpName, _helpName);

	titleField = (UIW_STRING *)Information(GET_STRINGID_OBJECT, "TITLE_FIELD");
	textField = (UIW_TEXT *)Information(GET_STRINGID_OBJECT, "MESSAGE_FIELD");
}


EVENT_TYPE HELP_EDITOR::Event(const UI_EVENT &event)
{
	// Switch on the type of event.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case OPTION_NEW:
	case OPTION_LOAD:
	case OPTION_SAVE_AS:
	case OPTION_DELETE:
		(void *)&(*windowManager + new CONTEXT_WINDOW(ccode, this));
		break;

	case OPTION_EXIT:
		eventManager->Put(UI_EVENT(S_CLOSE, 0));
		break;

	case OPTION_SAVE:
		if (!helpName)
		{
			eventManager->Put(UI_EVENT(OPTION_SAVE_AS, 0));
			break;
		}
		else
		{
			eventManager->DeviceState(E_MOUSE, DM_WAIT);
			CONTROL_WINDOW::storage->ChDir("~UI_HELP");
			int helpID = 1;
			if (!CONTROL_WINDOW::storage->FindFirstObject(helpName))
			{
				for (char *objectName = CONTROL_WINDOW::storage->FindFirstObject("*");
					objectName;	objectName = CONTROL_WINDOW::storage->FindNextObject())
				{
					if (*objectName != '.')
					{
						UI_STORAGE_OBJECT sObject(*CONTROL_WINDOW::storage, objectName,
							0,	UIS_READ);
						if (sObject.objectID >= helpID)
					 		helpID = sObject.objectID + 1;
					}
				}
			}
			else
			{
				UI_STORAGE_OBJECT sObject(*CONTROL_WINDOW::storage, helpName, 0, UIS_READ);
				helpID = sObject.objectID;
			}
			UI_STORAGE_OBJECT sObject(*CONTROL_WINDOW::storage, helpName, helpID,
				UIS_READWRITE | UIS_CREATE);
			sObject.Store((char *)titleField->Information(GET_TEXT, NULL));
			sObject.Store((char *)textField->Information(GET_TEXT, NULL));
			CONTROL_WINDOW::saveNeeded = TRUE;
			eventManager->DeviceState(E_MOUSE, DM_VIEW);
		}
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
	}
	return ccode;
}

void HELP_EDITOR::LoadContext(void)
{
	if (helpName)
	{
		CONTROL_WINDOW::storage->ChDir("~UI_HELP");
		UI_STORAGE_OBJECT sObject(*CONTROL_WINDOW::storage, helpName, 0, UIS_READ);
		if (!sObject.objectError)
		{
			char *buffer = NULL;
			sObject.Load(&buffer);
			titleField->Information(SET_TEXT, buffer);
			delete buffer;
			buffer = NULL;
			sObject.Load(&buffer);
			textField->Information(SET_TEXT, buffer);
			delete buffer;
		}
	}
	else
		textField->Information(SET_TEXT, NULL);
}

