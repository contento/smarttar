//	Zinc Interface Library Designer - Z_ADVANCE.HPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define UIF_CONTROL					UIW_ADVANCED_FRAME
#include <string.h>
#include "ui_dsn.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

UIW_ADVANCED_FRAME::UIW_ADVANCED_FRAME(UI_WINDOW_OBJECT *_editObject) :
	UIW_WINDOW("ADVANCED_WINDOW", defaultStorage), editObject(_editObject)
{
	for (int i = 0; objectTable[i].data && objectTable[i].value != editObject->searchID; i++)
		;
//!! This is a special case for designing the designer with the designer.
//	if (objectTable[i].data && &objectTable[i] >= CONTROL_WINDOW::deriveTable)
//		Get("CLASS_NAME")->Information(SET_TEXT, objectTable[i].text);
	UI_WINDOW_OBJECT *nameField = Get("CLASS_NAME");						// BUG.1688
	if (editObject->parent)
	{
		if (objectTable[i].data && &objectTable[i] >= CONTROL_WINDOW::deriveTable)
		nameField->Information(SET_TEXT, objectTable[i].text);
	}
	else
	{
		nameField->Information(SET_TEXT, "UIW_WINDOW");
		nameField->woFlags |= WOF_NON_SELECTABLE;
	}
	((FLAG_FIELD *)Get("FLAG_FIELD"))->DataSet(editObject->userFlags);
}

EVENT_TYPE UIW_ADVANCED_FRAME::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case OPTION_OK:
		{
		const char *className = (const char *)Get("CLASS_NAME")->Information(GET_TEXT, NULL);
		if (className && *className)
		{
			for (int i = 0; objectTable[i].data && strcmp(objectTable[i].text, className); i++)
				;
			if (!objectTable[i].data)
			{
				OBJECTID newSearchID = ID_DERIVE_START;
				for (i = 0; CONTROL_WINDOW::deriveTable[i].data; i++)
					 if (CONTROL_WINDOW::deriveTable[i].value >= newSearchID)
						newSearchID = (OBJECTID)CONTROL_WINDOW::deriveTable[i].value + 1;
				if (CONTROL_WINDOW::deriveTable[i].value != ID_DERIVE_END)
				{
					CONTROL_WINDOW::deriveTable[i].value = newSearchID;
					for (int j = 0; objectTable[j].data; j++)
						if (objectTable[j].value == editObject->searchID)
						{
							CONTROL_WINDOW::deriveTable[i].data = objectTable[j].data;
							break;
						}
					CONTROL_WINDOW::deriveTable[i].text = ui_strdup(className);
					editObject->searchID = newSearchID;
				}
			}
//!! This is a special case for designing the designer with the designer.
//			else
//				editObject->searchID = objectTable[i].value;
			else if (&objectTable[i] >= CONTROL_WINDOW::deriveTable)
				editObject->searchID = (OBJECTID)objectTable[i].value;
		}
		editObject->userFlags = ((FLAG_FIELD *)Get("FLAG_FIELD"))->DataGet();
		}
		// Continue to OPTION_CANCEL.

	case OPTION_CANCEL:
		eventManager->Put(UI_EVENT(S_CLOSE, 0));
		if (ccode == OPTION_OK)
			eventManager->Put(UI_EVENT(D_UPDATE_STATUS));
		break;

	case OPTION_HELP:
		helpSystem->DisplayHelp(windowManager, helpContext);
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}
	return ccode;
}
