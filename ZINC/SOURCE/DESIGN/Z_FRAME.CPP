//	Zinc Interface Library Designer - Z_FRAME.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stdio.h>
#include <string.h>
#include "ui_dsn.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

UIW_FRAME_WINDOW::UIW_FRAME_WINDOW(char *name, UI_WINDOW_OBJECT *_editObject,
	UI_WINDOW_OBJECT *_listItem) :
	UIW_WINDOW(name, defaultStorage), editObject(_editObject), listItem(_listItem)
{
	if (FlagSet(woStatus, WOS_READ_ERROR))
		return;
	// Display a wait signal.
	eventManager->DeviceState(E_MOUSE, DM_WAIT);

	UI_WINDOW_OBJECT *root = NULL;
	if (editObject->userObject)
		root = (UI_WINDOW_OBJECT *)editObject->userObject;
	else
	{
		root = editObject;
		while (root->parent)
			root = root->parent;
	}

	windowManager->Center(this);
	// Load object specific frame fields.
	UIF_FLAGS *objectFlags[5];
	UIW_WINDOW *optionWindow = NULL;
	switch (editObject->windowID[0])
	{
	case ID_BUTTON:
		{
		STORAGE_COMBO_BOX *bitmapCombo = (STORAGE_COMBO_BOX *)Information(GET_STRINGID_OBJECT, "BITMAP_COMBO");
		bitmapCombo->LoadBox(((UIW_BUTTON *)editObject)->bitmapName);
		objectFlags[0] = &((UIW_BUTTON *)editObject)->btFlags;
		objectFlags[1] = &editObject->woFlags;
		objectFlags[2] = &editObject->designerAdvancedFlags;
		}
		break;

	case ID_COMBO_BOX:
	case ID_STORAGE_COMBO_BOX:
		{
		UIW_COMBO_BOX *combo = (UIW_COMBO_BOX *)editObject;
		UI_WINDOW_OBJECT *compareField = (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "COMPARE_FUNC_FIELD");
//		compareField->Information(SET_TEXT, ((UIW_COMBO_BOX *)editObject)->compareFunctionName);  // BUG.2100
		compareField->Information(SET_TEXT, ((UIW_COMBO_BOX *)editObject)->list.compareFunctionName);
		int height = (combo->list.relative.Height() + display->preSpace + display->postSpace) / display->cellHeight;
//		if (display->isText)												// BUG.1673
//			--height;
		UIW_INTEGER *heightField = (UIW_INTEGER *)Information(GET_STRINGID_OBJECT, "HEIGHT_FIELD");
		heightField->DataSet(&height);
		OBJECT_LIST *objectList = (OBJECT_LIST *)Information(GET_STRINGID_OBJECT, "OBJECT_LIST");
		objectList->userObject = root;
		objectList->GetObjects(&((UIW_COMBO_BOX *)editObject)->list);
		objectFlags[0] = &((UIW_COMBO_BOX *)editObject)->list.wnFlags;
		objectFlags[1] = &editObject->woFlags;
		objectFlags[2] = &editObject->designerAdvancedFlags;
		optionWindow = &((UIW_COMBO_BOX *)editObject)->list;
		}
		break;

	case ID_DATE:
		{
		UIW_DATE *date = (UIW_DATE *)Information(GET_STRINGID_OBJECT, "DATE_DATE");
		date->DataSet(((UIW_DATE *)editObject)->date);
		UI_WINDOW_OBJECT *rangeField = (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "RANGE_FIELD");
		rangeField->Information(SET_TEXT, ((UIW_DATE *)editObject)->range);
		objectFlags[0] = &((UIW_DATE *)editObject)->dtFlags;
		objectFlags[1] = &editObject->woFlags;
		objectFlags[2] = &editObject->designerAdvancedFlags;
		}
		break;

	case ID_FORMATTED_STRING:
		{
		UIW_STRING *compressedString = (UIW_STRING *)Information(GET_STRINGID_OBJECT, "COMPRESSED_TEXT_STRING");
		compressedString->DataSet(((UIW_FORMATTED_STRING *)editObject)->compressedText);
		UIW_STRING *maskString = (UIW_STRING *)Information(GET_STRINGID_OBJECT, "EDIT_MASK_STRING");
		maskString->DataSet(((UIW_FORMATTED_STRING *)editObject)->editMask);
		UIW_STRING *deleteString = (UIW_STRING *)Information(GET_STRINGID_OBJECT, "DELETE_TEXT_STRING");
		deleteString->DataSet(((UIW_FORMATTED_STRING *)editObject)->deleteText);
		objectFlags[0] = &editObject->woFlags;
		objectFlags[1] = &editObject->designerAdvancedFlags;
		}
		break;

	case ID_GROUP:
		{
		OBJECT_LIST *objectList = (OBJECT_LIST *)Information(GET_STRINGID_OBJECT, "OBJECT_LIST");
		objectList->userObject = root;
		objectList->GetObjects((UIW_GROUP *)editObject);
		objectFlags[0] = &((UIW_GROUP *)editObject)->wnFlags;
		objectFlags[1] = &editObject->woFlags;
		}
		break;

	case ID_ICON:
		{
		STORAGE_COMBO_BOX *bitmapCombo = (STORAGE_COMBO_BOX *)Information(GET_STRINGID_OBJECT, "ICON_COMBO");
		bitmapCombo->LoadBox(((UIW_ICON *)editObject)->iconName);
		objectFlags[0] = &((UIW_ICON *)editObject)->icFlags;
		objectFlags[1] = &editObject->woFlags;
		objectFlags[2] = &editObject->designerAdvancedFlags;
		}
		break;

	case ID_VT_LIST:
	case ID_OBJECT_LIST:
		{
		UI_WINDOW_OBJECT *compareField = (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "COMPARE_FUNC_FIELD");
		compareField->Information(SET_TEXT, ((UIW_VT_LIST *)editObject)->compareFunctionName);
		OBJECT_LIST *objectList = (OBJECT_LIST *)Information(GET_STRINGID_OBJECT, "OBJECT_LIST");
		objectList->userObject = root;
		objectList->GetObjects((UIW_VT_LIST *)editObject);
		objectFlags[0] = &((UIW_VT_LIST *)editObject)->wnFlags;
		objectFlags[1] = &editObject->woFlags;
		optionWindow = (UIW_VT_LIST *)editObject;
		}
		break;

	case ID_HZ_LIST:
		{
		UIW_INTEGER *cellWidth = (UIW_INTEGER *)Information(GET_STRINGID_OBJECT, "CELL_WIDTH_INTEGER");
		cellWidth->DataSet(&((UIW_HZ_LIST *)editObject)->cellWidth);
		UIW_INTEGER *cellHeight = (UIW_INTEGER *)Information(GET_STRINGID_OBJECT, "CELL_HEIGHT_INTEGER");
		cellHeight->DataSet(&((UIW_HZ_LIST *)editObject)->cellHeight);
		UI_WINDOW_OBJECT *compareField = (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "COMPARE_FUNC_FIELD");
		compareField->Information(SET_TEXT, ((UIW_HZ_LIST *)editObject)->compareFunctionName);
		OBJECT_LIST *objectList = (OBJECT_LIST *)Information(GET_STRINGID_OBJECT, "OBJECT_LIST");
		objectList->userObject = root;
		objectList->GetObjects((UIW_HZ_LIST *)editObject);
		objectFlags[0] = &((UIW_HZ_LIST *)editObject)->wnFlags;
		objectFlags[1] = &editObject->woFlags;
		optionWindow = (UIW_HZ_LIST *)editObject;
		}
		break;

	case ID_BIGNUM:
		{
		UIW_BIGNUM *bignumField = (UIW_BIGNUM *)Information(GET_STRINGID_OBJECT, "NUMBER_BIGNUM");
		bignumField->DataSet(((UIW_BIGNUM *)editObject)->DataGet());
		UI_WINDOW_OBJECT *rangeField = (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "RANGE_FIELD");
		rangeField->Information(SET_TEXT, ((UIW_BIGNUM *)editObject)->range);
		UIW_BIGNUM *bignum = (UIW_BIGNUM *)editObject;
		UIF_FLAGS *bignumFlags[3];
		bignumFlags[0] = &bignum->nmFlags;
		bignumFlags[1] = &bignum->woFlags;
		bignumFlags[2] = &bignum->designerAdvancedFlags;
		UIW_VT_LIST *flagList = (UIW_VT_LIST *)Information(GET_STRINGID_OBJECT, "BIGNUM_FLAG_LIST");
		GetFlags(flagList, bignumFlags);
		GetDecimalFlags(flagList, &bignum->nmFlags);
		}
		break;

	case ID_INTEGER:
		{
		UIW_INTEGER *integer = (UIW_INTEGER *)Information(GET_STRINGID_OBJECT, "NUMBER_INTEGER");
		int value = ((UIW_INTEGER *)editObject)->DataGet();
		integer->DataSet(&value);
		UI_WINDOW_OBJECT *rangeField = (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "RANGE_FIELD");
		rangeField->Information(SET_TEXT, ((UIW_INTEGER *)editObject)->range);
		objectFlags[0] = &editObject->woFlags;
		objectFlags[1] = &editObject->designerAdvancedFlags;
		}
		break;

	case ID_PROMPT:
		objectFlags[0] = &editObject->woFlags;
		break;

	case ID_POP_UP_ITEM:
		{
		OBJECT_LIST *objectList = (OBJECT_LIST *)Information(GET_STRINGID_OBJECT, "OBJECT_LIST");
		objectList->userObject = root;
		objectList->GetObjects(&((UIW_POP_UP_ITEM *)editObject)->menu);
		objectFlags[0] = &((UIW_POP_UP_ITEM *)editObject)->menu.wnFlags;
		objectFlags[1] = &((UIW_POP_UP_ITEM *)editObject)->mniFlags;
		objectFlags[2] = &((UIW_POP_UP_ITEM *)editObject)->btFlags;
		objectFlags[3] = &editObject->woFlags;
		objectFlags[4] = &editObject->designerAdvancedFlags;
		}
		break;

	case ID_POP_UP_MENU:
		{
		OBJECT_LIST *objectList = (OBJECT_LIST *)Information(GET_STRINGID_OBJECT, "OBJECT_LIST");
		objectList->userObject = root;
		objectList->GetObjects((UIW_POP_UP_MENU *)editObject);
		objectFlags[0] = &((UIW_POP_UP_MENU *)editObject)->wnFlags;
		objectFlags[1] = &editObject->woFlags;
		}
		break;

	case ID_PULL_DOWN_ITEM:
		{
		OBJECT_LIST *objectList = (OBJECT_LIST *)Information(GET_STRINGID_OBJECT, "OBJECT_LIST");
		objectList->userObject = root;
		objectList->GetObjects(&((UIW_PULL_DOWN_ITEM *)editObject)->menu);
		objectFlags[0] = &((UIW_PULL_DOWN_ITEM *)editObject)->menu.wnFlags;
		objectFlags[1] = &((UIW_PULL_DOWN_ITEM *)editObject)->btFlags;
		objectFlags[2] = &editObject->woFlags;
		objectFlags[3] = &editObject->designerAdvancedFlags;
		}
		break;

	case ID_PULL_DOWN_MENU:
		{
		OBJECT_LIST *objectList = (OBJECT_LIST *)Information(GET_STRINGID_OBJECT, "OBJECT_LIST");
		objectList->userObject = root;
		objectList->GetObjects((UIW_PULL_DOWN_MENU *)editObject);
		objectFlags[0] = &editObject->woFlags;
		objectFlags[1] = &((UIW_PULL_DOWN_MENU *)editObject)->wnFlags;
		}
		break;

	case ID_REAL:
		{
		UIW_REAL *real = (UIW_REAL *)Information(GET_STRINGID_OBJECT, "NUMBER_REAL");
		double value = ((UIW_REAL *)editObject)->DataGet();
		real->DataSet(&value);
		UI_WINDOW_OBJECT *rangeField = (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "RANGE_FIELD");
		rangeField->Information(SET_TEXT, ((UIW_INTEGER *)editObject)->range);
		objectFlags[0] = &editObject->woFlags;
		objectFlags[1] = &editObject->designerAdvancedFlags;
		}
		break;

	case ID_SCROLL_BAR:
		objectFlags[0] = &((UIW_SCROLL_BAR *)editObject)->sbFlags;
		objectFlags[1] = &editObject->woFlags;
		break;

	case ID_STRING:
		objectFlags[0] = &((UIW_STRING *)editObject)->stFlags;
		objectFlags[1] = &editObject->woFlags;
		objectFlags[2] = &editObject->designerAdvancedFlags;
		break;

	case ID_SYSTEM_BUTTON:
		{
		OBJECT_LIST *objectList = (OBJECT_LIST *)Information(GET_STRINGID_OBJECT, "OBJECT_LIST");
		objectList->userObject = root;
		objectList->GetObjects(&((UIW_SYSTEM_BUTTON *)editObject)->menu);
		}
		break;

	case ID_TEXT:
		objectFlags[0] = &((UIW_TEXT *)editObject)->wnFlags;
		objectFlags[1] = &editObject->woFlags;
		objectFlags[2] = &editObject->designerAdvancedFlags;
		optionWindow = (UIW_TEXT *)editObject;
		break;

	case ID_TIME:
		{
		UIW_TIME *time = (UIW_TIME *)Information(GET_STRINGID_OBJECT, "TIME_TIME");
		time->DataSet(((UIW_TIME *)editObject)->time);
		UI_WINDOW_OBJECT *rangeField = (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "RANGE_FIELD");
		rangeField->Information(SET_TEXT, ((UIW_TIME *)editObject)->range);
		objectFlags[0] = &((UIW_TIME *)editObject)->tmFlags;
		objectFlags[1] = &editObject->woFlags;
		objectFlags[2] = &editObject->designerAdvancedFlags;
		}
		break;

	case ID_TITLE:
		objectFlags[1] = &editObject->woFlags;
		break;

	case ID_TOOL_BAR:
		{
		OBJECT_LIST *objectList = (OBJECT_LIST *)Information(GET_STRINGID_OBJECT, "OBJECT_LIST");
		objectList->userObject = root;
		objectList->GetObjects((UIW_TOOL_BAR *)editObject);
		objectFlags[0] = &((UIW_TOOL_BAR *)editObject)->wnFlags;
		objectFlags[1] = &editObject->woFlags;
		}
		break;

	case ID_WINDOW:
		{
		STORAGE_COMBO_BOX *bitmapCombo = (STORAGE_COMBO_BOX *)Information(GET_STRINGID_OBJECT, "MIN_ICON_COMBO");
		for (UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)((UIW_WINDOW *)editObject)->support.First();
			object && object->SearchID() != ID_ICON; object = object->Next())
		;
		char *minIconName = NULL;
		if (object)
			minIconName = ((UIW_ICON *)object)->iconName;
		bitmapCombo->LoadBox(minIconName);
		OBJECT_LIST *objectList = (OBJECT_LIST *)Information(GET_STRINGID_OBJECT, "OBJECT_LIST");
		objectList->GetObjects((UIW_WINDOW *)editObject);
		objectFlags[0] = &editObject->woFlags;
		objectFlags[1] = &editObject->designerAdvancedFlags;
		optionWindow = (UIW_WINDOW *)editObject;
		}
		break;

	case ID_DERIVE_OBJECT:
	case ID_WINDOW_OBJECT:
		objectFlags[0] = &editObject->woFlags;
		objectFlags[1] = &editObject->designerAdvancedFlags;
		break;
	}

	// Load general information frame fields.
	UI_WINDOW_OBJECT *dataField;

	// Load text field (if it exists).
	dataField = (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "TEXT_FIELD");
	if (dataField)
		dataField->Information(SET_TEXT, (char *)editObject->Information(GET_TEXT, NULL));

	// Load value field (if it exists).
	UIW_INTEGER *valueInteger = (UIW_INTEGER *)Information(GET_STRINGID_OBJECT, "VALUE_INTEGER");
	if (valueInteger)
	{
		EVENT_TYPE value = 0; editObject->Information(GET_VALUE, &value);
		int intValue = (int)value; valueInteger->DataSet(&intValue);
	}

	// Load user function field (if it exists).
	dataField = (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "USER_FUNC_FIELD");
	if (dataField)
		dataField->Information(SET_TEXT, editObject->userFunctionName);

	// Load stringID field (if it exists).
	dataField = (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "STRINGID_FIELD");
	if (dataField)
		dataField->Information(SET_TEXT, editObject->StringID());

	// Load helpContext field (if it exists).
	STORAGE_COMBO_BOX *helpContextCombo = (STORAGE_COMBO_BOX *)Information(GET_STRINGID_OBJECT, "HELP_CONTEXT_COMBO");
	if (helpContextCombo)
		helpContextCombo->LoadBox(editObject->helpContext);


	// Load max length field (if it exists).
	UIW_INTEGER *maxLength = (UIW_INTEGER *)Information(GET_STRINGID_OBJECT, "MAX_LENGTH_INTEGER");
	if (maxLength)
		maxLength->DataSet((int *)editObject->Information(GET_TEXT_LENGTH, NULL));

	UIW_VT_LIST *flagList = (UIW_VT_LIST *)Information(GET_STRINGID_OBJECT, "FLAG_LIST");
	if (flagList)
		GetFlags(flagList, objectFlags, optionWindow);

	oldFlags = editObject->woFlags;

	helpBar = (HELP_BAR *)Information(GET_STRINGID_OBJECT, "HELP_BAR");
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
}


EVENT_TYPE UIW_FRAME_WINDOW::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case OPTION_OK:
		{
		// Store object specific information.
		UIF_FLAGS *objectFlags[5];
		UIW_WINDOW *optionWindow = NULL;
		switch (editObject->windowID[0])
		{
		case ID_BUTTON:
			{
			STORAGE_COMBO_BOX *bitmapCombo = (STORAGE_COMBO_BOX *)Information(GET_STRINGID_OBJECT, "BITMAP_COMBO");
			ReplaceString(&((UIW_BUTTON *)editObject)->bitmapName, bitmapCombo->GetName());
			if (((UIW_BUTTON *)editObject)->bitmapName)
			{
				CONTROL_WINDOW::storage->ChDir("~UI_BITMAP");
				UI_STORAGE_OBJECT sObject(*CONTROL_WINDOW::storage, ((UIW_BUTTON *)editObject)->bitmapName, ID_BITMAP_IMAGE, UIS_READ);
				if (!sObject.objectError)
				{
					int bitmapWidth;
					int bitmapHeight;
					USHORT value;
					sObject.Load(&value); bitmapWidth = value;
					sObject.Load(&value); bitmapHeight = value;
					UCHAR *bitmapArray = new UCHAR[bitmapWidth * bitmapHeight];
					sObject.Load(bitmapArray, bitmapWidth, bitmapHeight);
					editObject->Information(SET_BITMAP_WIDTH, &bitmapWidth);
					editObject->Information(SET_BITMAP_HEIGHT, &bitmapHeight);
					editObject->Information(SET_BITMAP_ARRAY, bitmapArray);
					delete bitmapArray;
				}
			}
			else
			{
				int value = 0;
				editObject->Information(SET_BITMAP_WIDTH, &value);
				editObject->Information(SET_BITMAP_HEIGHT, &value);
				editObject->Information(SET_BITMAP_ARRAY, NULL);
			}
			objectFlags[0] = &((UIW_BUTTON *)editObject)->btFlags;
			objectFlags[1] = &editObject->woFlags;
			objectFlags[2] = &editObject->designerAdvancedFlags;
			}
			break;

		case ID_COMBO_BOX:
		case ID_STORAGE_COMBO_BOX:
			{
			UIW_COMBO_BOX *combo = (UIW_COMBO_BOX *)editObject;
			UI_WINDOW_OBJECT *compareField = (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "COMPARE_FUNC_FIELD");
//			ReplaceString(&((UIW_COMBO_BOX *)editObject)->compareFunctionName, (char *)compareField->Information(GET_TEXT, NULL));  BUG.2100
			ReplaceString(&((UIW_COMBO_BOX *)editObject)->list.compareFunctionName, (char *)compareField->Information(GET_TEXT, NULL));

			UIW_INTEGER *heightField = (UIW_INTEGER *)Information(GET_STRINGID_OBJECT, "HEIGHT_FIELD");
			int height = heightField->DataGet();
//			combo->list.relative.top = 0;
//			combo->list.relative.bottom = height * display->cellHeight;		// BUG.1493
//			combo->list.true.top = combo->true.top;
			combo->list.true.top = 0;
			combo->list.true.bottom = height * display->cellHeight - 1;
			combo->list.relative = combo->list.true;

			OBJECT_LIST *objectList = (OBJECT_LIST *)Information(GET_STRINGID_OBJECT, "OBJECT_LIST");
			objectList->SaveObjects(&((UIW_COMBO_BOX *)editObject)->list);
			objectFlags[0] = &((UIW_COMBO_BOX *)editObject)->list.wnFlags;
			objectFlags[1] = &editObject->woFlags;
			objectFlags[2] = &editObject->designerAdvancedFlags;
			optionWindow = &((UIW_COMBO_BOX *)editObject)->list;
			}
			break;

		case ID_DATE:
			{
			UIW_DATE *date = (UIW_DATE *)Information(GET_STRINGID_OBJECT, "DATE_DATE");
			((UIW_DATE *)editObject)->DataSet(date->date);
			UI_WINDOW_OBJECT *rangeField = (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "RANGE_FIELD");
			ReplaceString(&((UIW_DATE *)editObject)->range, (char *)rangeField->Information(GET_TEXT, NULL));
			objectFlags[0] = &((UIW_DATE *)editObject)->dtFlags;
			objectFlags[1] = &editObject->woFlags;
			objectFlags[2] = &editObject->designerAdvancedFlags;
			}
			break;

		case ID_FORMATTED_STRING:
			{
			UIW_STRING *compressedString = (UIW_STRING *)Information(GET_STRINGID_OBJECT, "COMPRESSED_TEXT_STRING");
			strcpy(((UIW_FORMATTED_STRING *)editObject)->compressedText, compressedString->DataGet());
			UIW_STRING *maskString = (UIW_STRING *)Information(GET_STRINGID_OBJECT, "EDIT_MASK_STRING");
			strcpy(((UIW_FORMATTED_STRING *)editObject)->editMask, maskString->DataGet());
			UIW_STRING *deleteString = (UIW_STRING *)Information(GET_STRINGID_OBJECT, "DELETE_TEXT_STRING");
			strcpy(((UIW_FORMATTED_STRING *)editObject)->deleteText, deleteString->DataGet());
			objectFlags[0] = &editObject->woFlags;
			objectFlags[1] = &editObject->designerAdvancedFlags;
			}
			break;

		case ID_GROUP:
			{
			OBJECT_LIST *objectList = (OBJECT_LIST *)Information(GET_STRINGID_OBJECT, "OBJECT_LIST");
			objectList->SaveObjects((UIW_GROUP *)editObject);
			objectFlags[0] = &((UIW_GROUP *)editObject)->wnFlags;
			objectFlags[1] = &editObject->woFlags;
			break;
			}

		case ID_ICON:
			{
			STORAGE_COMBO_BOX *bitmapCombo = (STORAGE_COMBO_BOX *)Information(GET_STRINGID_OBJECT, "ICON_COMBO");
			ReplaceString(&((UIW_ICON *)editObject)->iconName, bitmapCombo->GetName());
			if (((UIW_ICON *)editObject)->iconName)
			{
				CONTROL_WINDOW::storage->ChDir("~UI_ICON");
				UI_STORAGE_OBJECT sObject(*CONTROL_WINDOW::storage, ((UIW_ICON *)editObject)->iconName, ID_ICON_IMAGE, UIS_READ);
				if (!sObject.objectError)
				{
					UCHAR *iconArray = new UCHAR[32 * 32];
					USHORT widthHeight;
					sObject.Load(&widthHeight);
					sObject.Load(&widthHeight);
					sObject.Load(iconArray, 32, 32);
					editObject->Information(SET_BITMAP_ARRAY, iconArray);
					delete iconArray;
				}
			}
			else
			{
				defaultStorage->ChDir("~UI_ICON");
				UI_STORAGE_OBJECT sObject(*defaultStorage, "blankIcon", ID_ICON_IMAGE, UIS_READ);
				if (!sObject.objectError)
				{
					UCHAR *iconArray = new UCHAR[32 * 32];
					USHORT widthHeight;
					sObject.Load(&widthHeight);
					sObject.Load(&widthHeight);
					sObject.Load(iconArray, 32, 32);
					editObject->Information(SET_BITMAP_ARRAY, iconArray);
					delete iconArray;
				}
			}
			objectFlags[0] = &((UIW_ICON *)editObject)->icFlags;
			objectFlags[1] = &editObject->woFlags;
			objectFlags[2] = &editObject->designerAdvancedFlags;
			}
			break;

		case ID_VT_LIST:
		case ID_OBJECT_LIST:
			{
			UI_WINDOW_OBJECT *compareField = (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "COMPARE_FUNC_FIELD");
			ReplaceString(&((UIW_VT_LIST *)editObject)->compareFunctionName, (char *)compareField->Information(GET_TEXT, NULL));
			OBJECT_LIST *objectList = (OBJECT_LIST *)Information(GET_STRINGID_OBJECT, "OBJECT_LIST");
			objectList->SaveObjects((UIW_VT_LIST *)editObject);
			objectFlags[0] = &((UIW_VT_LIST *)editObject)->wnFlags;
			objectFlags[1] = &editObject->woFlags;
			optionWindow = (UIW_VT_LIST *)editObject;
			}
			break;

		case ID_HZ_LIST:
			{
			UIW_INTEGER *cellWidth = (UIW_INTEGER *)Information(GET_STRINGID_OBJECT, "CELL_WIDTH_INTEGER");
			((UIW_HZ_LIST *)editObject)->cellWidth = cellWidth->DataGet();
			UIW_INTEGER *cellHeight = (UIW_INTEGER *)Information(GET_STRINGID_OBJECT, "CELL_HEIGHT_INTEGER");
			((UIW_HZ_LIST *)editObject)->cellHeight = cellHeight->DataGet();
			UI_WINDOW_OBJECT *compareField = (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "COMPARE_FUNC_FIELD");
			ReplaceString(&((UIW_HZ_LIST *)editObject)->compareFunctionName, (char *)compareField->Information(GET_TEXT, NULL));
			OBJECT_LIST *objectList = (OBJECT_LIST *)Information(GET_STRINGID_OBJECT, "OBJECT_LIST");
			objectList->SaveObjects((UIW_HZ_LIST *)editObject);
			objectFlags[0] = &((UIW_HZ_LIST *)editObject)->wnFlags;
			objectFlags[1] = &editObject->woFlags;
			optionWindow = (UIW_HZ_LIST *)editObject;
			}
			break;

		case ID_BIGNUM:
			{
			UIW_BIGNUM *bignumField = (UIW_BIGNUM *)Information(GET_STRINGID_OBJECT, "NUMBER_BIGNUM");
			((UIW_BIGNUM *)editObject)->DataSet(bignumField->DataGet());
			UI_WINDOW_OBJECT *rangeField = (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "RANGE_FIELD");
			ReplaceString(&((UIW_BIGNUM *)editObject)->range, (char *)rangeField->Information(GET_TEXT, NULL));
			UIW_BIGNUM *bignum = (UIW_BIGNUM *)editObject;
			UIF_FLAGS *bignumFlags[3];
			bignumFlags[0] = &bignum->nmFlags;
			bignumFlags[1] = &bignum->woFlags;
			bignumFlags[2] = &bignum->designerAdvancedFlags;
			UIW_VT_LIST *flagList = (UIW_VT_LIST *)Information(GET_STRINGID_OBJECT, "BIGNUM_FLAG_LIST");
			SaveFlags(flagList, bignumFlags);
			SaveDecimalFlags(flagList, &bignum->nmFlags);
			}
			break;

		case ID_INTEGER:
			{
			UIW_INTEGER *integer = (UIW_INTEGER *)Information(GET_STRINGID_OBJECT, "NUMBER_INTEGER");
			int value = integer->DataGet();
			((UIW_INTEGER *)editObject)->DataSet(&value);
			UI_WINDOW_OBJECT *rangeField = (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "RANGE_FIELD");
			ReplaceString(&((UIW_INTEGER *)editObject)->range, (char *)rangeField->Information(GET_TEXT, NULL));
			objectFlags[0] = &editObject->woFlags;
			objectFlags[1] = &editObject->designerAdvancedFlags;
			}
			break;

		case ID_PROMPT:
			objectFlags[0] = &editObject->woFlags;
			break;

		case ID_POP_UP_ITEM:
			{
			OBJECT_LIST *objectList = (OBJECT_LIST *)Information(GET_STRINGID_OBJECT, "OBJECT_LIST");
			objectList->SaveObjects(&((UIW_POP_UP_ITEM *)editObject)->menu);
			objectFlags[0] = &((UIW_POP_UP_ITEM *)editObject)->menu.wnFlags;
			objectFlags[1] = &((UIW_POP_UP_ITEM *)editObject)->mniFlags;
			objectFlags[2] = &((UIW_POP_UP_ITEM *)editObject)->btFlags;
			objectFlags[3] = &editObject->woFlags;
			objectFlags[4] = &editObject->designerAdvancedFlags;
			}
			break;

		case ID_POP_UP_MENU:
			{
			OBJECT_LIST *objectList = (OBJECT_LIST *)Information(GET_STRINGID_OBJECT, "OBJECT_LIST");
			objectList->SaveObjects((UIW_POP_UP_MENU *)editObject);
			objectFlags[0] = &((UIW_POP_UP_MENU *)editObject)->wnFlags;
			objectFlags[1] = &editObject->woFlags;
			}
			break;

		case ID_PULL_DOWN_ITEM:
			{
			OBJECT_LIST *objectList = (OBJECT_LIST *)Information(GET_STRINGID_OBJECT, "OBJECT_LIST");
			objectList->SaveObjects(&((UIW_PULL_DOWN_ITEM *)editObject)->menu);
			objectFlags[0] = &((UIW_PULL_DOWN_ITEM *)editObject)->menu.wnFlags;
			objectFlags[1] = &((UIW_PULL_DOWN_ITEM *)editObject)->btFlags;
			objectFlags[2] = &editObject->woFlags;
			objectFlags[3] = &editObject->designerAdvancedFlags;
			}
			break;

		case ID_PULL_DOWN_MENU:
			{
			OBJECT_LIST *objectList = (OBJECT_LIST *)Information(GET_STRINGID_OBJECT, "OBJECT_LIST");
			objectList->SaveObjects((UIW_PULL_DOWN_MENU *)editObject);
			objectFlags[0] = &editObject->woFlags;
			objectFlags[1] = &((UIW_PULL_DOWN_MENU *)editObject)->wnFlags;
			}
			break;

		case ID_REAL:
			{
			UIW_REAL *real = (UIW_REAL *)Information(GET_STRINGID_OBJECT, "NUMBER_REAL");
			double value = real->DataGet();
			((UIW_REAL *)editObject)->DataSet(&value);
			UI_WINDOW_OBJECT *rangeField = (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "RANGE_FIELD");
			ReplaceString(&((UIW_INTEGER *)editObject)->range, (char *)rangeField->Information(GET_TEXT, NULL));
			objectFlags[0] = &editObject->woFlags;
			objectFlags[1] = &editObject->designerAdvancedFlags;
			}
			break;

		case ID_SCROLL_BAR:
			objectFlags[0] = &((UIW_SCROLL_BAR *)editObject)->sbFlags;
			objectFlags[1] = &editObject->woFlags;
			break;

		case ID_STRING:
			objectFlags[0] = &((UIW_STRING *)editObject)->stFlags;
			objectFlags[1] = &editObject->woFlags;
			objectFlags[2] = &editObject->designerAdvancedFlags;
			break;

		case ID_SYSTEM_BUTTON:
			{
			OBJECT_LIST *objectList = (OBJECT_LIST *)Information(GET_STRINGID_OBJECT, "OBJECT_LIST");
			objectList->SaveObjects(&((UIW_SYSTEM_BUTTON *)editObject)->menu);
			}
			break;

		case ID_TEXT:
			objectFlags[0] = &((UIW_TEXT *)editObject)->wnFlags;
			objectFlags[1] = &editObject->woFlags;
			objectFlags[2] = &editObject->designerAdvancedFlags;
			optionWindow = (UIW_TEXT *)editObject;
			break;

		case ID_TIME:
			{
			UIW_TIME *time = (UIW_TIME *)Information(GET_STRINGID_OBJECT, "TIME_TIME");
			((UIW_TIME *)editObject)->DataSet(time->time);
			UI_WINDOW_OBJECT *rangeField = (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "RANGE_FIELD");
			ReplaceString(&((UIW_TIME *)editObject)->range, (char *)rangeField->Information(GET_TEXT, NULL));
			objectFlags[0] = &((UIW_TIME *)editObject)->tmFlags;
			objectFlags[1] = &editObject->woFlags;
			objectFlags[2] = &editObject->designerAdvancedFlags;
			}
			break;

		case ID_TITLE:
			objectFlags[1] = &editObject->woFlags;
			break;

		case ID_TOOL_BAR:
			{
			OBJECT_LIST *objectList = (OBJECT_LIST *)Information(GET_STRINGID_OBJECT, "OBJECT_LIST");
			objectList->SaveObjects((UIW_TOOL_BAR *)editObject);
			objectFlags[0] = &((UIW_TOOL_BAR *)editObject)->wnFlags;
			objectFlags[1] = &editObject->woFlags;
			}
			break;

		case ID_WINDOW:
			{
			STORAGE_COMBO_BOX *bitmapCombo = (STORAGE_COMBO_BOX *)Information(GET_STRINGID_OBJECT, "MIN_ICON_COMBO");
			char *minIconName = bitmapCombo->GetName();
			if (minIconName && !*minIconName)
				minIconName = NULL;
			for (UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)((UIW_WINDOW *)editObject)->support.First();
				object && object->SearchID() != ID_ICON; object = object->Next())
			;
			if (object && (!minIconName || strcmp(minIconName, ((UIW_ICON *)object)->iconName)))
			{
				(void *)&(*(UIW_WINDOW *)editObject - object);
				delete object;
				object = NULL;
			}
			if (!object && minIconName)
			{
				object = new UIW_ICON(0, 0, minIconName, NULL, ICF_MINIMIZE_OBJECT,
					WOF_JUSTIFY_CENTER | WOF_SUPPORT_OBJECT);
				(void *)&(*(UIW_WINDOW *)editObject + object);
			}
			if (object)
			{
				UI_WINDOW_OBJECT *titleField = (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "TEXT_FIELD");
				object->Information(SET_TEXT, (char *)titleField->Information(GET_TEXT, NULL));
			}

			OBJECT_LIST *objectList = (OBJECT_LIST *)Information(GET_STRINGID_OBJECT, "OBJECT_LIST");
			objectList->SaveObjects((UIW_WINDOW *)editObject);
			objectFlags[0] = &editObject->woFlags;
			objectFlags[1] = &editObject->designerAdvancedFlags;
			optionWindow = (UIW_WINDOW *)editObject;
			}
			break;

		case ID_DERIVE_OBJECT:
		case ID_WINDOW_OBJECT:
			objectFlags[0] = &editObject->woFlags;
			objectFlags[1] = &editObject->designerAdvancedFlags;
			break;
		}


		// Store general information.
		UI_WINDOW_OBJECT *dataField;

		// Store max length field (if it exists).
		UIW_INTEGER *maxLength = (UIW_INTEGER *)Information(GET_STRINGID_OBJECT, "MAX_LENGTH_INTEGER");
		if (maxLength)
		{
			int value = maxLength->DataGet();
			editObject->Information(SET_TEXT_LENGTH, &value);
		}

		// Store text field (if it exists).
		dataField = (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "TEXT_FIELD");
		if (dataField)
			editObject->Information(SET_TEXT, (char *)dataField->Information(GET_TEXT, NULL));

		// Store value field (if it exists).
		UIW_INTEGER *valueInteger = (UIW_INTEGER *)Information(GET_STRINGID_OBJECT, "VALUE_INTEGER");
		if (valueInteger)
		{
			EVENT_TYPE value = valueInteger->DataGet();
			editObject->Information(SET_VALUE, &value);
		}

		// Store user function field (if it exists).
		dataField = (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "USER_FUNC_FIELD");
		if (dataField)
			ReplaceString(&editObject->userFunctionName, (char *)dataField->Information(GET_TEXT, NULL));

		// Store stringID field (if it exists).
		dataField = (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "STRINGID_FIELD");
		if (dataField)
		{
			editObject->StringID((char *)dataField->Information(GET_TEXT, NULL));
			if (listItem && *editObject->StringID())
				listItem->Information(SET_TEXT, editObject->StringID());
		}

		// Store helpContext field (if it exists).
		STORAGE_COMBO_BOX *helpContextCombo = (STORAGE_COMBO_BOX *)Information(GET_STRINGID_OBJECT, "HELP_CONTEXT_COMBO");
		if (helpContextCombo)
			editObject->helpContext = helpContextCombo->GetID();

		UIW_VT_LIST *flagList = (UIW_VT_LIST *)Information(GET_STRINGID_OBJECT, "FLAG_LIST");
		if (flagList)
			SaveFlags(flagList, objectFlags, optionWindow);

		if (editObject->windowID[0] == ID_SCROLL_BAR)
		{
			UIW_SCROLL_BAR *scrollBar = (UIW_SCROLL_BAR *)editObject;
			if (FlagSet(scrollBar->sbFlags, SBF_VERTICAL) && scrollBar->relative.bottom -
				scrollBar->relative.top < 2 * display->cellHeight)
				scrollBar->relative.bottom = scrollBar->relative.top + 3 * display->cellHeight;
			if (FlagSet(scrollBar->sbFlags, SBF_HORIZONTAL) && scrollBar->relative.right -
				scrollBar->relative.left < 3 * display->cellWidth)
				scrollBar->relative.right = scrollBar->relative.left + 10 * display->cellWidth;
		}

//		if (editObject->screenID)
		if (editObject->parent && editObject->screenID)						// BUG.General
		{
			if (FlagSet(editObject->woFlags, WOF_MINICELL))
			{
				long miniNX = display->miniNumeratorX, miniDX = display->miniDenominatorX;
				long miniNY = display->miniNumeratorY, miniDY = display->miniDenominatorY;
				int width = editObject->relative.Width();
				int height = editObject->relative.Height();
				editObject->relative.left = (int)((editObject->relative.left * miniDX) /
					(miniNX * display->cellWidth) * miniNX * display->cellWidth / miniDX);
				editObject->relative.right = editObject->relative.left + width - 1;
				if (!editObject->Inherited(ID_BUTTON))
				{
					editObject->relative.top = (int)((editObject->relative.top * miniDY) /
						(miniNY * display->cellHeight) * miniNY * display->cellHeight / miniDY);
					editObject->relative.bottom = editObject->relative.top + height - 1;
				}
				else
				{
					BTF_FLAGS btFlags;
					editObject->Information(GET_FLAGS, &btFlags, ID_BUTTON);
					editObject->relative.bottom = (int)((editObject->relative.bottom * miniDY) /
						(miniNY * display->cellHeight) * miniNY * display->cellHeight / miniDY);
					if (FlagSet(btFlags, BTF_AUTO_SIZE | BTF_CHECK_BOX | BTF_RADIO_BUTTON))
						editObject->relative.top = editObject->relative.bottom - height + 1;
					else
						editObject->relative.top = (int)((editObject->relative.top * miniDY) /
							(miniNY * display->cellHeight) * miniNY * display->cellHeight / miniDY);
				}
			}
			else
			{
				int width = editObject->relative.Width();
				int height = editObject->relative.Height();
				editObject->relative.left = editObject->relative.left / display->cellWidth * display->cellWidth;
				editObject->relative.right = editObject->relative.left + width - 1;
				if (!editObject->Inherited(ID_BUTTON))
				{
					editObject->relative.top = editObject->relative.top / display->cellHeight *
						display->cellHeight + display->preSpace;
					editObject->relative.bottom = editObject->relative.top + height - 1;
				}
				else
				{
					BTF_FLAGS btFlags;
					editObject->Information(GET_FLAGS, &btFlags, ID_BUTTON);
					editObject->relative.bottom = (editObject->relative.bottom + display->postSpace + 1) /
						display->cellHeight * display->cellHeight - display->postSpace - 1;
					if (FlagSet(btFlags, BTF_AUTO_SIZE | BTF_CHECK_BOX | BTF_RADIO_BUTTON))
						editObject->relative.top = editObject->relative.bottom - height + 1;
					else
						editObject->relative.top = editObject->relative.top / display->cellHeight *
							display->cellHeight + display->preSpace;
				}
			}


			UI_WINDOW_OBJECT *redisplayObject;
			if (editObject->parent && (FlagSet(editObject->woFlags, WOF_NON_FIELD_REGION) ||
				FlagSet(oldFlags, WOF_NON_FIELD_REGION)))
			{
				editObject->parent->Information(CHANGED_FLAGS, NULL);
				redisplayObject = editObject->parent;
			}
			else
			{
				editObject->Information(CHANGED_FLAGS, NULL);

				// Work around for window's bug which subtracts the window
				// in CHANGED_FLAGS execution.
				if (!editObject->parent && !windowManager->Index(editObject))
					windowManager->UI_LIST::Add(editObject);

				redisplayObject = editObject;
			}

			while (FlagSet(redisplayObject->woFlags, WOF_NON_FIELD_REGION) && redisplayObject->parent)
				redisplayObject = redisplayObject->parent;

			UI_WINDOW_OBJECT *root = redisplayObject;
			while (root->parent)
				root = root->parent;
			root->Event(UI_EVENT(S_SIZE, 0));

			redisplayObject->Event(UI_EVENT(S_REDISPLAY, 0));
		}
		else if (editObject->screenID)										// BUG.General
		{
			editObject->Event(UI_EVENT(S_SIZE));
			editObject->Event(UI_EVENT(S_REDISPLAY));
		}

		eventManager->Put(D_UPDATE_STATUS);
		}
		// Continue to OPTION_CANCEL.

	case OPTION_CANCEL:
		eventManager->Put(UI_EVENT(S_CLOSE, 0));
		break;

	case OPTION_HELP:
		helpSystem->DisplayHelp(windowManager, helpContext);
		break;

	case OPTION_ADD_ITEM:
		{
		OBJECT_LIST *objectList = (OBJECT_LIST *)Information(GET_STRINGID_OBJECT, "OBJECT_LIST");
		if (editObject->SearchID() == ID_PULL_DOWN_MENU)
			objectList->NewObject(new UIW_PULL_DOWN_ITEM("Item"));
		else
			objectList->NewObject(new UIW_POP_UP_ITEM("Item"));
		}
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		helpBar->Update(Current());
		break;
	}
	return ccode;
}

