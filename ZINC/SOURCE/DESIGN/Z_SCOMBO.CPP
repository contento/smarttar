//	Zinc Interface Library Designer - Z_COMBO.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <string.h>
#include "ui_dsn.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

STORAGE_COMBO_BOX::STORAGE_COMBO_BOX(const char *name, UI_STORAGE *directory, UI_STORAGE_OBJECT *file) :
	UIW_COMBO_BOX(name, directory, file)
{
	file->Load(&childID);
	searchID = windowID[0] = ID_STORAGE_COMBO_BOX;
	windowID[1] = ID_COMBO_BOX;
	windowID[2] = ID_LIST;
	windowID[3] = ID_WINDOW;
}

void STORAGE_COMBO_BOX::Store(const char *name, UI_STORAGE *file, UI_STORAGE_OBJECT *object)
{
	UIW_COMBO_BOX::Store(name, file, object);
	object->Store(childID);
}

void STORAGE_COMBO_BOX::LoadBox(char *currentName)
{
	if (childID == ID_BITMAP_IMAGE)
		CONTROL_WINDOW::storage->ChDir("~UI_BITMAP");
	else if (childID == ID_ICON_IMAGE)
		CONTROL_WINDOW::storage->ChDir("~UI_ICON");
	else 
		CONTROL_WINDOW::storage->ChDir("~UI_HELP");
	for (char *objectName = CONTROL_WINDOW::storage->FindFirstObject("*"); objectName;
		objectName = CONTROL_WINDOW::storage->FindNextObject())
	{
		if (objectName[0] != '.')
		{
			UIW_BUTTON *button = new UIW_BUTTON(0, 0, 30, objectName,
				BTF_AUTO_SIZE | BTF_NO_3D, WOF_NO_FLAGS);

			if (childID != ID_HELP_CONTEXT &&
				FlagSet(_config->options, OPTION_MENU_BITMAPS))
			{
				UI_STORAGE_OBJECT sObject(*CONTROL_WINDOW::storage, objectName, 0, UIS_READ);
				int bitmapWidth;
				int bitmapHeight;
				USHORT value;
				sObject.Load(&value); bitmapWidth = value;
				sObject.Load(&value); bitmapHeight = value;
				if (bitmapWidth <= 32 && bitmapHeight <=32)
				{
					UCHAR *bitmapArray = new UCHAR[bitmapWidth * bitmapHeight];
					sObject.Load(bitmapArray, bitmapWidth, bitmapHeight);
					button->Information(SET_BITMAP_WIDTH, &bitmapWidth);
					button->Information(SET_BITMAP_HEIGHT, &bitmapHeight);
					button->Information(SET_BITMAP_ARRAY, bitmapArray);
					delete bitmapArray;
				}
			}
			if (currentName && !strcmp(objectName, currentName))
				button->woStatus |= WOS_CURRENT;
			(void *)&(*this + button);
		}
	}
}

void STORAGE_COMBO_BOX::LoadBox(UI_HELP_CONTEXT helpNumber)
{
	CONTROL_WINDOW::storage->ChDir("~UI_HELP");
	static char currentName[64] = "";
	char *objectName = CONTROL_WINDOW::storage->FindFirstID(helpNumber);
	if (objectName)
		strcpy(currentName, objectName);
	LoadBox(currentName);
}

char *STORAGE_COMBO_BOX::GetName(void)
{
	char *returnName = (char *)Information(GET_TEXT, NULL);
	if (!strcmp(returnName, "(None)"))
		returnName = NULL;
	return returnName;
}

int STORAGE_COMBO_BOX::GetID(void)
{
	char *name = (char *)UIW_COMBO_BOX::Information(GET_TEXT, NULL);
	if (name && *name == '\0')
		name = NULL;
	if (name)
	{
		if (childID == ID_BITMAP_IMAGE)
			CONTROL_WINDOW::storage->ChDir("~UI_BITMAP");
		else if (childID == ID_ICON_IMAGE)
			CONTROL_WINDOW::storage->ChDir("~UI_ICON");
		else
			CONTROL_WINDOW::storage->ChDir("~UI_HELP");
		UI_STORAGE_OBJECT sObject(*CONTROL_WINDOW::storage, name, 0, UIS_READ);
		if (!sObject.objectError)
			return sObject.objectID;
	}
	return 0;
}


