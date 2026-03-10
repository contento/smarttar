//	Zinc Interface Library Designer - Z_RESRC.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stdio.h>
#include "ui_dsn.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

EVENT_TYPE ResourceNameFunction(UI_WINDOW_OBJECT *object, UI_EVENT &event,
	EVENT_TYPE ccode)
{
	// Perform action on keyboard select only.
	if (object->LogicalEvent(event, ID_WINDOW_OBJECT) == L_SELECT)
		object->eventManager->Put(OPTION_OK);
	return (ccode);
}

static EVENT_TYPE ResourceSelect(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
		object->eventManager->Put(RESOURCE_SELECT);

	// Complete selection on a mouse double click, or a keyboard select.
	if (ccode == L_DOUBLE_CLICK || event.type == L_SELECT)
		object->eventManager->Put(OPTION_OK);
	return (ccode);
}

RESOURCE_WINDOW::RESOURCE_WINDOW(char *title) :
	UIW_WINDOW(display->isText ? "RESOURCE_WINDOW_TEXT" : "RESOURCE_WINDOW", defaultStorage)
{
	windowManager->Center(this);
	Information(SET_TEXT, title);
	nameField = (UIW_STRING *)Information(GET_STRINGID_OBJECT, "NAME_FIELD");
	resourceList = (UIW_HZ_LIST *)Information(GET_STRINGID_OBJECT, "RESOURCE_LIST");
	helpBar = (HELP_BAR *)Information(GET_STRINGID_OBJECT, "HELP_BAR");

	// Initialize resource selection list.
	CONTROL_WINDOW::storage->ChDir("~UIW_WINDOW");
	char *objectName = CONTROL_WINDOW::storage->FindFirstObject("*");
	while (objectName)
	{
		if (objectName[0] != '.')
		{
			(void *)&(*resourceList
				+ new UIW_BUTTON(0, 0, 35, objectName, BTF_NO_TOGGLE | BTF_NO_3D |
					BTF_SEND_MESSAGE | BTF_DOUBLE_CLICK, WOF_NO_FLAGS, ResourceSelect));
		}
		objectName = CONTROL_WINDOW::storage->FindNextObject();
	}
	if (!resourceList->First())
		(void *)&(*resourceList	+ new UIW_STRING(0, 0, 35, "(None)"));
}


EVENT_TYPE RESOURCE_WINDOW::Event(const UI_EVENT &event)
{
	// Switch on the type of event.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case RESOURCE_SELECT:
		nameField->Information(SET_TEXT, resourceList->Current()->Information(GET_TEXT, NULL));
		break;

	case OPTION_OK:
		eventManager->Put(RESOURCE_SELECTED);
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

