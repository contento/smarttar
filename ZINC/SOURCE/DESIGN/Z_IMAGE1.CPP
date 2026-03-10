//	Zinc Interface Library Designer - Z_IMAGE1.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
#include <mem.h>
#endif
#include <string.h>
#include "ui_dsn.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

#include <stdio.h>

EVENT_TYPE ImageType(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	if (ccode == S_CURRENT || ccode == L_SELECT)
	{
		UI_WINDOW_OBJECT *root = object;
		while (root->parent)
			root = root->parent;
		object->Information(GET_VALUE, &event.type);
		root->Event(event);
	}
	return ccode;
}

EVENT_TYPE ImageNameFunction(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	// Send event on a keyboard select only.
	if (object->LogicalEvent(event, ID_WINDOW_OBJECT) == L_SELECT)
		object->eventManager->Put(OPTION_OK);
	return (ccode);
}

static EVENT_TYPE ImageItemSelect(UI_WINDOW_OBJECT *object,	UI_EVENT &event, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
		object->eventManager->Put(IMAGE_SET_NAME);

	// Complete selection on a mouse double click, or a keyboard select.
	if (ccode == L_DOUBLE_CLICK || event.type == L_SELECT)
		object->eventManager->Put(OPTION_OK);
	return (ccode);
}

IMAGE_WINDOW::IMAGE_WINDOW(char *title, IMAGE_EDITOR *editor, EVENT_TYPE action) :
	UIW_WINDOW("IMAGE_WINDOW", defaultStorage), imageID(0)
{
	Information(SET_TEXT, title);

	// Initialize child pointers.
	typeField = (UIW_COMBO_BOX *)Information(GET_STRINGID_OBJECT, "TYPE_COMBO");
 	nameField = (UIW_STRING *)Information(GET_STRINGID_OBJECT, "NAME_FIELD");
	widthField = (UIW_INTEGER *)Information(GET_STRINGID_OBJECT, "WIDTH_FIELD");
	heightField = (UIW_INTEGER *)Information(GET_STRINGID_OBJECT, "HEIGHT_FIELD");
	imageList = (UIW_HZ_LIST *)Information(GET_STRINGID_OBJECT, "IMAGE_LIST");
	helpBar = (HELP_BAR *)Information(GET_STRINGID_OBJECT, "HELP_BAR");

	// Initialize width & height fields.
	int value = editor->imageWidth;	widthField->DataSet(&value);
	value = editor->imageHeight; heightField->DataSet(&value);
	if (action != OPTION_NEW)
	{
		widthField->woFlags |= WOF_NON_SELECTABLE;
		heightField->woFlags |= WOF_NON_SELECTABLE;
		if (action == OPTION_SAVE_AS && (editor->imageWidth != 32 || editor->imageHeight != 32))
			typeField->woFlags |= WOF_NON_SELECTABLE;
	}
}

EVENT_TYPE IMAGE_WINDOW::Event(const UI_EVENT &event)
{
	// Switch on the type of event.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case IMAGE_SET_NAME:
		nameField->Information(SET_TEXT, imageList->Current()->Information(GET_TEXT, NULL));
		break;

	case S_CREATE:
		UIW_WINDOW::Event(event);
		// Continue to IMAGE_LIST.
	case IMAGE_LIST_BITMAPS:
	case IMAGE_LIST_ICONS:
		{
		// Get image type;
		OBJECTID newImageID = (ccode == IMAGE_LIST_ICONS) ? ID_ICON_IMAGE : ID_BITMAP_IMAGE;
		if (newImageID != imageID)
		{
			eventManager->DeviceState(E_MOUSE, DM_WAIT);
			imageID = newImageID;
			// Load image list.
			imageList->Destroy();
			CONTROL_WINDOW::storage->ChDir(imageID == ID_BITMAP_IMAGE ? "~UI_BITMAP" : "~UI_ICON");
			char *objectName = CONTROL_WINDOW::storage->FindFirstObject("*");
			while (objectName)
			{
				if (strcmp(objectName, ".") && strcmp(objectName, ".."))
				{
					UIW_BUTTON *button = new UIW_BUTTON(0, 0, 30, objectName, BTF_AUTO_SIZE | BTF_NO_3D, WOF_NO_FLAGS, ImageItemSelect);

					// Load button image.
					if (FlagSet(_config->options, OPTION_MENU_BITMAPS))
					{
						UI_STORAGE_OBJECT sObject(*CONTROL_WINDOW::storage, objectName, imageID, UIS_READ);
						int imageWidth;
						int imageHeight;
						USHORT value;
						sObject.Load(&value); imageWidth = value;
						sObject.Load(&value); imageHeight = value;
						if (imageWidth <= 32 && imageHeight <= 32)
						{
							UCHAR *imageArray = new UCHAR[imageWidth * imageHeight];
							sObject.Load(imageArray, imageWidth, imageHeight);
							button->Information(SET_BITMAP_WIDTH, &imageWidth);
							button->Information(SET_BITMAP_HEIGHT, &imageHeight);
							button->Information(SET_BITMAP_ARRAY, imageArray);
							delete imageArray;
						}
					}
					(void *)&(*imageList + button);
				}
				objectName = CONTROL_WINDOW::storage->FindNextObject();
			}
			if (!imageList->First())
				(void *)&(*imageList + new UIW_STRING(0, 0, 30, "(None)"));
			imageList->Event(S_REDISPLAY);
			eventManager->DeviceState(E_MOUSE, DM_VIEW);
		}
		}
		break;

	case OPTION_OK:
		eventManager->Put(UI_EVENT(IMAGE_SELECTED, 0));
		break;

	case OPTION_CANCEL:
		eventManager->Put(UI_EVENT(S_CLOSE, 0));
		break;

	case OPTION_HELP:
		helpSystem->DisplayHelp(windowManager, helpContext);
		break;

	case S_INITIALIZE:
		// Prevent help bar update before initialization.
		ccode = UIW_WINDOW::Event(event);
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		helpBar->Update(Current());
		break;
	}
	return ccode;
}
