//	Zinc Interface Library Designer - Z_UTIL.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stdio.h>
#include <string.h>
#include "ui_dsn.hpp"
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || (defined(ZIL_OS2) && !defined(__IBMCPP__))
#include <dos.h>
#if defined(__ZTC__) || defined(_MSC_VER) || defined(__IBMCPP__) || defined(__WATCOMC__)
#include <direct.h>
#else
#include <dir.h>
#endif
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif
#endif

void ReplaceString(char **oldString, char *newString)
{
	if (*oldString)
		delete *oldString;
	*oldString = ui_strdup(newString);
}

// Called by persistant objects.
EVENT_TYPE HelpFunction(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT && object->parent)
	{
		object->eventManager->DeviceState(E_MOUSE, DM_WAIT);
		if (object->parent->helpContext)
			object->helpSystem->DisplayHelp(object->windowManager, object->parent->helpContext);
		else
			object->helpSystem->DisplayHelp(object->windowManager, object->helpContext);
		object->eventManager->DeviceState(E_MOUSE, DM_VIEW);
	}
	return ccode;
}

// Called by persistant objects.
EVENT_TYPE HelpIndexFunction(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
	if (ccode == L_SELECT)
	{
		UIW_WINDOW *helpIndexWindow = new UIW_WINDOW("HELP_INDEX_WINDOW", object->defaultStorage);
		object->windowManager->Center(helpIndexWindow);
		(void *)&(*object->windowManager + helpIndexWindow);
	}
	return ccode;
}

void GetFlags(UIW_VT_LIST *flagsList, UIF_FLAGS *objectFlag[], UIW_WINDOW *window)
{
	UI_WINDOW_OBJECT *object = flagsList->First();
	if (window)
	{
		object = object->Next();
		while (object && *(char *)object->Information(GET_TEXT, NULL) != '-')
		{
			UIW_BUTTON *button = (UIW_BUTTON *)object;
			for (UI_WINDOW_OBJECT *supportObject = (UI_WINDOW_OBJECT *)window->support.First();
				supportObject; supportObject = supportObject->Next())
			{
				OBJECTID matchID = supportObject->SearchID();
				UIF_FLAGS *matchFlags = (UIF_FLAGS *)supportObject->Information(GET_FLAGS, NULL, matchID);
				if (matchID == button->value && (!button->userFlags ||
					FlagsSet(*matchFlags, button->userFlags)))
				{
					button->woStatus |= WOS_SELECTED;
					break;
				}
			}
			if (button->value != ID_SCROLL_BAR)
				button->userObject = supportObject;
			else
				button->userObject = NULL;
			object = object->Next();
		}
	}
	if (object)
	{
		int flagIndex = 0;
		for (object = object->Next(); object; object = object->Next())
		{
			if (*(char *)object->Information(GET_TEXT, NULL) == '-')
				++flagIndex;
			else
			{
				if (FlagSet(*objectFlag[flagIndex], ((UIW_BUTTON *)object)->value))
					object->woStatus |= WOS_SELECTED;
			}
		}
	}
}

void SaveFlags(UIW_VT_LIST *flagsList, UIF_FLAGS *objectFlag[],	UIW_WINDOW *window)
{
	UI_WINDOW_OBJECT *object = flagsList->First();

	// Handle the support object options.
	if (window)
	{
		// Clear the support list.
		UI_LIST supportList;
		while (window->support.First())
		{
			UI_WINDOW_OBJECT *sObject = (UI_WINDOW_OBJECT *)window->support.First();
			(void *)&(*window - sObject);
			if (sObject->SearchID() == ID_SCROLL_BAR)
				delete sObject;
			else
				(void *)&(supportList + sObject);
		}

		// Rebuild the support list according to options.
		int scrollBars = 0;
		UIW_SCROLL_BAR *corner = NULL;
		object = object->Next();
		while (object && *(char *)object->Information(GET_TEXT, NULL) != '-')
		{
			if (FlagSet(object->woStatus, WOS_SELECTED))
			{
				if (((UIW_BUTTON *)object)->value == ID_SCROLL_BAR)
				{
					if (scrollBars == 0)
						(void *)&(*window + (corner = new UIW_SCROLL_BAR(0, 0, 2, 2, SBF_CORNER)));
					++scrollBars;
				}
				if (object->userObject)
				{
					(void *)&(supportList - (UI_WINDOW_OBJECT *)object->userObject);
					(void *)&(*window + (UI_WINDOW_OBJECT *)object->userObject);
				}
				else
				{
					EVENT_TYPE createID;
					object->Information(GET_VALUE, &createID, ID_BUTTON);
					UI_WINDOW_OBJECT *newObject =
						CreateObject((short)createID, object->userFlags);
					newObject->woFlags |= WOF_NON_FIELD_REGION | WOF_SUPPORT_OBJECT;
					newObject->woStatus |= WOS_EDIT_MODE;
					(void *)&(*window + newObject);
				}
			}
			else if (object->userObject)
			{
				(void *)&(supportList - (UI_WINDOW_OBJECT *)object->userObject);
				delete object->userObject;
			}
			object = object->Next();
		}
		while (supportList.First())
		{
			UI_WINDOW_OBJECT *sObject = (UI_WINDOW_OBJECT *)supportList.First();
			(void *)&(supportList - sObject);
			(void *)&(*window + sObject);
		}
		if (corner && scrollBars < 2)
		{
			(void *)&(*window - corner);
			delete corner;
		}
	}

	// Save the flags.
	if (object)
	{
		int flagIndex = 0;
		for (object = object->Next(); object; object = object->Next())
		{
			if (*(char *)object->Information(GET_TEXT, NULL) == '-')
				++flagIndex;
			else
			{
				EVENT_TYPE flags;
				object->Information(GET_VALUE, &flags, ID_BUTTON);
				if (FlagSet(object->woStatus, WOS_SELECTED))
					*objectFlag[flagIndex] |= (short)flags;
				else
					*objectFlag[flagIndex] &= ~(short)flags;
			}
		}
	}
}

void GetDecimalFlags(UIW_VT_LIST *flagsList, NMF_FLAGS *nmFlags)
{
	UI_WINDOW_OBJECT *object = flagsList->First()->Next();
	while (object && *(char *)object->Information(GET_TEXT, NULL) != '-')
	{
		EVENT_TYPE flags;
		object->Information(GET_VALUE, &flags, ID_BUTTON);
		NMF_FLAGS decimalFlags = (short)flags & NMF_DECIMAL_FLAGS;
		if (decimalFlags)
		{
			if (decimalFlags == (*nmFlags & NMF_DECIMAL_FLAGS))
				object->woStatus |= WOS_SELECTED;
			else
				object->woStatus &= ~WOS_SELECTED;
		}
		object = object->Next();
	}
}

void SaveDecimalFlags(UIW_VT_LIST *flagsList, NMF_FLAGS *nmFlags)
{
	*nmFlags &= ~NMF_DECIMAL_FLAGS;
	UI_WINDOW_OBJECT *object = flagsList->First()->Next();
	while (object && *(char *)object->Information(GET_TEXT, NULL) != '-')
	{
		EVENT_TYPE flags;
		object->Information(GET_VALUE, &flags, ID_BUTTON);
		NMF_FLAGS decimalFlags = (short)flags & NMF_DECIMAL_FLAGS;
		if (decimalFlags && FlagSet(object->woStatus, WOS_SELECTED))
			*nmFlags |= decimalFlags;
		object = object->Next();
	}
}

int AddAllowed(UI_WINDOW_OBJECT *parent, UI_WINDOW_OBJECT *child)
{
	if (parent->SearchID() >= 3500)
		return TRUE;
	OBJECTID parentID = parent->SearchID();
	OBJECTID childID = child->SearchID();

	switch (parentID)
	{
	case ID_WINDOW:
		return TRUE;

	case ID_PULL_DOWN_ITEM:
	case ID_POP_UP_MENU:
	case ID_POP_UP_ITEM:
	case ID_SYSTEM_BUTTON:
		if (childID == ID_POP_UP_ITEM)
			return TRUE;
		break;

	case ID_PULL_DOWN_MENU:
		if (childID == ID_PULL_DOWN_ITEM)
			return TRUE;
		break;

	case ID_COMBO_BOX:
		if (childID == ID_COMBO_BOX || childID == ID_SCROLL_BAR)
			return FALSE;

		// Continue

	case ID_VT_LIST:
	case ID_HZ_LIST:
		if (childID == ID_VT_LIST || childID == ID_HZ_LIST ||
			childID == ID_DERIVE_OBJECT || childID == ID_COMBO_BOX ||
			childID == ID_WINDOW)
			return FALSE;

		// Continue

	case ID_GROUP:
	case ID_TOOL_BAR:
		if (childID == ID_PULL_DOWN_MENU || childID == ID_TOOL_BAR ||
			childID == ID_SCROLL_BAR)
			return FALSE;
		return TRUE;
	}
	return FALSE;
}

#if defined(ZIL_MSWINDOWS)
int _dosError;
EVENT_TYPE GetErrorResponce()
{
	return S_UNKNOWN;
}
#endif

