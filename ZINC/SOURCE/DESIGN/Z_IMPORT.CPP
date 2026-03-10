//	Zinc Interface Library Designer - Z_IMPORT.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <string.h>
#include "ui_dsn.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

#define bufferSize		128

static void LoadResources(UI_STORAGE *sourceStorage, UIW_VT_LIST *importList)
{
	int count = 0;
	char *objectName = sourceStorage->FindFirstObject("*");
	while (objectName)
	{
		if (strcmp(objectName, ".") && strcmp(objectName, ".."))
		{
			UIW_BUTTON *button = new UIW_BUTTON(5, 0, 30, objectName, BTF_CHECK_BOX, WOF_NO_FLAGS);
			(void *)&(*importList + button);
			++count;
		}
		objectName = sourceStorage->FindNextObject();
	}
	if (count == 0)
		(void *)&(*importList + new UIW_BUTTON(5, 0, 0, "--- None ---", BTF_NO_3D, WOF_NON_SELECTABLE));
}

static UI_WINDOW_OBJECT *CopyResources(UI_STORAGE *destStorage, UI_STORAGE *sourceStorage, UI_WINDOW_OBJECT *object, int uniqueID = 0)
{
	int objectID = 0;
	if (uniqueID)
	{
		objectID = 1;
		for (char *objectName = destStorage->FindFirstObject("*");
			objectName;	objectName = destStorage->FindNextObject())
		{
			if (*objectName != '.')
			{
				UI_STORAGE_OBJECT sObject(*destStorage, objectName, 0, UIS_READ);
				if (sObject.objectID >= objectID)
					objectID = sObject.objectID + 1;
			}
		}
	}

	while (object && object->Inherited(ID_BUTTON))
	{
		if (FlagSet(object->woStatus, WOS_SELECTED))
		{
			int import = TRUE;
			char *objectName;
			object->Information(GET_TEXT, &objectName);
			if (destStorage->FindFirstObject(objectName))
			{
				DIALOG_WINDOW dialog("Import", "QUESTION", DIF_YES | DIF_NO,
					"Resource already exists:\r\n\r\n   %s\r\n\r\nOverwrite it?",
					objectName);
				if (dialog.Responce() != DIALOG_YES)
					import = FALSE;
				object->eventManager->DeviceState(E_MOUSE, DM_WAIT);
			}
			if (import)
			{
				UI_STORAGE_OBJECT sourceObject(*sourceStorage, objectName, 0, UIS_READ);
				UI_STORAGE_OBJECT destObject(*destStorage, objectName, objectID, UIS_READWRITE | UIS_CREATE);
				if (uniqueID)
					++objectID;

				int bytesRead = 0;
				UCHAR buffer[bufferSize];
				do
				{
					bytesRead = sourceObject.Load(buffer, 1, bufferSize);
					destObject.Store(buffer, 1, bytesRead);
				} while (bytesRead);
			}
		}
		object = object->Next();

		// Poll Devices.
		UI_EVENT dummyEvent;
		object->eventManager->Get(dummyEvent, Q_NO_BLOCK | Q_NO_DESTROY | Q_POLL);
	}
	if (object)
		object = object->Next();
	return object;
}

IMPORT_WINDOW::IMPORT_WINDOW(UI_STORAGE *_destStorage, UI_STORAGE *_sourceStorage) :
	UIW_WINDOW("IMPORT_WINDOW", defaultStorage), sourceStorage(_sourceStorage), destStorage(_destStorage)
{
	windowManager->Center(this);

	importList = (UIW_VT_LIST *)Information(GET_STRINGID_OBJECT, "IMPORT_LIST");

	if (!sourceStorage->ChDir("~UIW_WINDOW"))
	{
		(void *)&(*importList + new UIW_STRING(0, 0, 0, "Resources:", -1, STF_NO_FLAGS, WOF_NON_SELECTABLE));
		LoadResources(sourceStorage, importList);
	}

	if (!sourceStorage->ChDir("~UI_BITMAP"))
	{
		(void *)&(*importList + new UIW_STRING(0, 0, 0, "Bitmaps:", -1, STF_NO_FLAGS, WOF_NON_SELECTABLE));
		LoadResources(sourceStorage, importList);
	}

	if (!sourceStorage->ChDir("~UI_ICON"))
	{
		(void *)&(*importList + new UIW_STRING(0, 0, 0, "Icons:", -1, STF_NO_FLAGS, WOF_NON_SELECTABLE));
		LoadResources(sourceStorage, importList);
	}

	if (!sourceStorage->ChDir("~UI_HELP"))
	{
		(void *)&(*importList + new UIW_STRING(0, 0, 0, "Help Contexts:", -1, STF_NO_FLAGS, WOF_NON_SELECTABLE));
		LoadResources(sourceStorage, importList);
	}
}

IMPORT_WINDOW::~IMPORT_WINDOW(void)
{
	delete sourceStorage;
}

EVENT_TYPE IMPORT_WINDOW::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	switch (event.type)
	{
	case OPTION_OK:
		{
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		UI_WINDOW_OBJECT *object = importList->First()->Next();

		if (!sourceStorage->ChDir("~UIW_WINDOW"))
		{
			destStorage->ChDir("~UIW_WINDOW");
			object = CopyResources(destStorage, sourceStorage, object);
		}

		if (!sourceStorage->ChDir("~UI_BITMAP"))
		{
			destStorage->ChDir("~UI_BITMAP");
			object = CopyResources(destStorage, sourceStorage, object);
		}

		if (!sourceStorage->ChDir("~UI_ICON"))
		{
			destStorage->ChDir("~UI_ICON");
			object = CopyResources(destStorage, sourceStorage, object);
		}

		if (!sourceStorage->ChDir("~UI_HELP"))
		{
			destStorage->ChDir("~UI_HELP");
			object = CopyResources(destStorage, sourceStorage, object, TRUE);
		}

		eventManager->Put(S_CLOSE);
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
		}
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
	}
	return ccode;
}
