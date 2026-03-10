//	Zinc Interface Library Designer - Z_CTRL.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define UIF_CONTROL						CONTROL_WINDOW
#define USE_HELP_CONTEXTS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui_dsn.hpp"
#include "p_design.hpp"
#if defined(ZIL_MSDOS)
#	if defined(__ZTC__) || defined(_MSC_VER) || defined(__WATCOMC__)
#		include <direct.h>
#		if defined(_MSC_VER)
#			pragma hdrstop			// Microsoft pre-compiled header pragma.
#		endif
#	else
#		include <dir.h>
#	endif
#elif defined(ZIL_POSIX)
#	include <unistd.h>
#	if defined(_SUNOS4)
#		define remove unlink
#	endif
#endif

int CONTROL_WINDOW::testMode = FALSE;
int CONTROL_WINDOW::saveNeeded = FALSE;
UI_STORAGE *CONTROL_WINDOW::storage = NULL;
UI_ITEM *CONTROL_WINDOW::deriveTable;

CONTROL_WINDOW::CONTROL_WINDOW(char *fileName) :
	UIW_WINDOW("CONTROL_WINDOW", defaultStorage),
	toolBar(NULL), statusBar(NULL), helpBar(NULL), currentObject(NULL), place(FALSE),
	paste(FALSE)
{
	mainMenu = (UIW_PULL_DOWN_MENU *)Information(GET_STRINGID_OBJECT, "MAIN_MENU");

	// Initialize the deriveTable
	for (int i = 0; UI_WINDOW_OBJECT::objectTable[i].data; i++)
		;
	deriveTable = &UI_WINDOW_OBJECT::objectTable[i];

	// Initialize the storage object.
	if (UI_STORAGE::ValidName(fileName, FALSE))
		FileAction(FILE_FOPEN, fileName);
	else if (ui_stricmp(fileName, "untitled.dat") == 0)
		FileAction(FILE_NEW, fileName);
	else if (UI_STORAGE::ValidName(fileName, TRUE))
	{
		DIALOG_WINDOW dialog("Open", "QUESTION", DIF_YES | DIF_NO,
			"File %s is not found.\r\n\r\nDo you wish to create it?", fileName);
		if (dialog.Responce() == DIALOG_YES)
			FileAction(FILE_NEW, fileName);
	}
	else
	{
		DIALOG_WINDOW dialog("Open", "QUESTION", DIF_OK, "Invalid file name:\r\n\r\n%s", fileName);
		dialog.Responce();
	}
	if (!storage)
	{
		fileName = "untitled.dat";
		if (UI_STORAGE::ValidName(fileName, FALSE))
			FileAction(FILE_FOPEN, fileName);
		else
			FileAction(FILE_NEW, fileName);
	}


	// Build the window and center horizontally.
	int height = BuildWindow();
	int width = relative.Width();
	relative.left = (display->columns / display->cellWidth - width) / 2;
	relative.bottom = relative.top + height - 1;
	relative.right = relative.left + width - 1;

	if (display->isText)
	{
		++relative.bottom;
		UIW_WINDOW::Get("IMAGE_EDIT_ITEM")->woFlags |= WOF_NON_SELECTABLE;
	}

	UpdateStatusBar();
	UpdatePlaceString(0);
	saveNeeded = FALSE;
}

CONTROL_WINDOW::~CONTROL_WINDOW(void)
{
	delete storage;
}

EVENT_TYPE CONTROL_WINDOW::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	// Switch on the type of event.
	EVENT_TYPE ccode = event.type;
	if (ccode >= FILE_FIRST && ccode <= FILE_LAST)
		FileOption(event);
	else if (ccode >= EDIT_FIRST && ccode <= EDIT_LAST)
		EditOption(event);
	else if (ccode >= RESOURCE_FIRST && ccode <= RESOURCE_LAST)
		ResourceOption(event);
	else if (ccode >= UTIL_FIRST && ccode <= UTIL_LAST)
		UtilOption(event);
	else
		switch (ccode)
		{
		case D_CONFIG_CHANGED:
			{

			int height = BuildWindow();
			if (!FlagSet(woStatus, WOS_MAXIMIZED))
				relative.bottom = relative.top + height *
					display->cellHeight;

//#if defined(ZIL_OS2) || defined(ZIL_MOTIF)								// BUG.General
#if defined(ZIL_OS2)
			UIW_WINDOW::Event(UI_EVENT(S_CREATE));
#endif
			(void *)&(*windowManager + this);
			windowManager->screenID = screenID;
			UpdateStatusBar();
			if (placeID)
				UpdatePlaceString(placeID);
			else if (paste)
			{
				UI_STORAGE_OBJECT sObject(*storage, "paste", 0, UIS_READ);
				if (!sObject.objectError)
					UpdatePlaceString(sObject.objectID);
			}
			else
				UpdatePlaceString(0);
			}
			break;

		case D_EDIT_OBJECT:
			{
			object = (UI_WINDOW_OBJECT *)event.data;
			char frameName[64];
			OBJECTID searchID = object->windowID[0];
			if (searchID == ID_STORAGE_COMBO_BOX)
				searchID = ID_COMBO_BOX;
			else if (searchID == ID_OBJECT_LIST)
				searchID = ID_VT_LIST;
			else if (searchID == ID_WINDOW_OBJECT)
				searchID = ID_DERIVE_OBJECT;
			sprintf(frameName, "UIF_%d", searchID);
			if (display->isText)
				strcat(frameName, "_TEXT");
			defaultStorage->ChDir("~UIW_WINDOW");
			if (defaultStorage->FindFirstObject(frameName))
				(void *)&(*windowManager + new UIW_FRAME_WINDOW(frameName, object));
			}
			break;

		case D_SET_OBJECT:
			currentObject = (UI_WINDOW_OBJECT *)event.data;
			UpdateStatusBar();
			break;

		case D_SET_POSITION:
			if (FlagSet(event.rawCode, M_RIGHT))
			{
				if (place || paste)
				{
					// Cancel the object placement.
					paste = FALSE;
					place = FALSE;
			   	 	UpdatePlaceString(0);
				}
				else
				{
					if (placeID)
					{
						place = TRUE;
					 	UpdatePlaceString(placeID);
					}
					else
						EditOption(UI_EVENT(EDIT_PASTE, 0));
				}
			}
			else if ((place || paste) && currentObject)
			{
				// Create the object.
				UI_WINDOW_OBJECT *newObject = NULL;
				if (place)
					newObject = CreateObject(placeID);
				else
				{
					storage->ChDir("~");
					UI_STORAGE_OBJECT sObject(*storage, "paste", 0, UIS_READ);
					if (!sObject.objectError)
						newObject =	UI_WINDOW_OBJECT::New("paste", storage, &sObject);
					UI_WINDOW_OBJECT *resource = currentObject;
					while (resource->parent)
						resource = resource->parent;

					// Regenerate object's stringID if not unique.
					if (resource->Information(GET_STRINGID_OBJECT, newObject->StringID()))
						newObject->StringID("");
					newObject->numberID = 0;
				}
				if (newObject)
				{
					// Position the object.
					int column = event.position.column / display->cellWidth;
					int line = event.position.line / display->cellHeight;
//					if (placeID == ID_BUTTON)								// BUG.????
//						++line;
					int width = newObject->relative.Width();
					int height = newObject->relative.Height();
					newObject->relative.left = column;
					newObject->relative.top = line;
					newObject->relative.right = newObject->relative.left + width - 1;
					newObject->relative.bottom = newObject->relative.top + height - 1;

					// Set the edit status of the object.
					newObject->woStatus |= WOS_EDIT_MODE;

					// Place the object.
					if (AddAllowed(currentObject, newObject))
					{
						UI_EVENT tEvent;
						tEvent.type = S_ADD_OBJECT;
						tEvent.data = newObject;
						
						//Attach the object.
						currentObject->Event(tEvent);

						//Make the object current.
						currentObject->Event(tEvent);

						if (FlagSet(currentObject->woFlags, WOF_NON_FIELD_REGION))
							currentObject->parent->Information(CHANGED_FLAGS, NULL);
						else if FlagSet(newObject->woFlags, WOF_SUPPORT_OBJECT)		// BUG.1535
							currentObject->Information(CHANGED_FLAGS, NULL);

						UI_WINDOW_OBJECT *root = currentObject;
						while (root->parent)
							root = root->parent;
						root->Event(UI_EVENT(S_SIZE));

						if (FlagSet(currentObject->woFlags, WOF_NON_FIELD_REGION))
							currentObject->parent->Event(UI_EVENT(S_REDISPLAY, 0));
						else
							currentObject->Event(UI_EVENT(S_REDISPLAY, 0));
						currentObject = newObject;
					}
					else
					{
						errorSystem->Beep();
						delete newObject;
						break;
					}
				}
				paste = FALSE;
				place = FALSE;
				UpdateStatusBar();
				UpdatePlaceString(0);
			}
			break;

		case D_UPDATE_STATUS:
			UpdateStatusBar();
			break;

	 	case S_INITIALIZE:
		case S_CREATE:
			ccode = UIW_WINDOW::Event(event);
			break;

		default:
			{
			if (ccode >= D_CREATE_OBJECT && ccode <= D_DESIGNER_LAST)
			{
				place = TRUE;
				placeID = (OBJECTID)(ccode - D_CREATE_OBJECT);
				UpdatePlaceString(placeID);
			}
			else
				ccode = UIW_WINDOW::Event(event);
			UI_WINDOW_OBJECT *helpObject = Current();
			if (helpObject == mainMenu)
				helpObject = mainMenu->Current();
			if (helpBar)
				helpBar->Update(helpObject);
			}
			break;
		}

	// Return the control code.
 	return (ccode);
}

void CONTROL_WINDOW::FileOption(const UI_EVENT &event)
{
	char path[128];
	char file[128];
	storage->StorageName(path);
	storage->StripFullPath(path, NULL, file, NULL);

	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case FILE_SAVE:
		if (!ui_stricmp(file, "UNTITLED.DAT"))
			FileAction(FILE_SAVE_AS);
		else
			FileAction(ccode);
		break;

	case FILE_NEW:
	case FILE_FOPEN:
	case FILE_SAVE_AS:
	case FILE_DELETE:
		FileAction(ccode);
		break;

	case FILE_PREFERENCES:
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		(void *)&(*windowManager + new PREFERENCES_WINDOW);
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
		break;

	case FILE_EXIT:
		eventManager->Put(UI_EVENT(L_EXIT_FUNCTION, 0));
		break;
	}
}

void CONTROL_WINDOW::EditOption(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case EDIT_OBJECT:
		if (currentObject)
		{
			UI_EVENT tEvent;
			tEvent.type = D_EDIT_OBJECT;
			tEvent.data = currentObject;
			Event(tEvent);
		}
		break;

	case EDIT_ADVANCED:
		if (currentObject)
			(void *)&(*windowManager + new UIW_ADVANCED_FRAME(currentObject));
		break;

	case EDIT_COPY:
	case EDIT_CUT:
	case EDIT_DELETE:
		if (currentObject)
		{
			if (ccode != EDIT_DELETE)
			{
				// Store the current object;
				eventManager->DeviceState(E_MOUSE, DM_WAIT);
				storage->ChDir("~");
				UI_STORAGE_OBJECT sObject(*storage, "paste",
					currentObject->SearchID(), UIS_READWRITE | UIS_CREATE);
				sObject.Store(currentObject->SearchID());
				currentObject->Store("paste", storage, &sObject);
				placeID = 0;
			}
			if (ccode != EDIT_COPY)
			{
				if (currentObject->parent)
				{
					// Clear the current object;
					UI_WINDOW_OBJECT *parentObject = currentObject->parent;
					UI_EVENT tEvent;
					tEvent.type = S_SUBTRACT_OBJECT;
					tEvent.data = currentObject;
					parentObject->Event(tEvent);
					if (FlagSet(currentObject->woFlags, WOF_NON_FIELD_REGION))
						parentObject->Information(CHANGED_FLAGS, NULL);
					parentObject->Event(UI_EVENT(S_REDISPLAY, 0));

					// Make sure object was subtracted.
					if (!parentObject->Get(currentObject->NumberID()))
						delete currentObject;
					currentObject = parentObject;
					UpdateStatusBar();
				}
				else
				{
					DIALOG_WINDOW dialog("Delete", "QUESTION", DIF_OK |
						DIF_CANCEL, "Resource %s will be deleted!",
						currentObject->StringID());
					if (dialog.Responce() != DIALOG_CANCEL)
					{
						(void *)&(*windowManager - currentObject);
						delete currentObject;
						currentObject = NULL;
						for (currentObject = windowManager->First(); currentObject &&
							!FlagsSet(currentObject->woStatus, WOS_EDIT_MODE);
							currentObject = currentObject->Next())
						;
						UpdateStatusBar();
					}
				}
			}
		}
		else
			errorSystem->Beep();
		break;

	case EDIT_PASTE:
		{
			// See if a paste object exists in the paste buffer.
			storage->ChDir("~");
			UI_STORAGE_OBJECT sObject(*storage, "paste", 0, UIS_READ);
			if (!sObject.objectError)
			{
				paste = TRUE;
				placeID = 0;
				UpdatePlaceString(sObject.objectID);
			}
			else
			{
				DIALOG_WINDOW dialog("Error!", "ASTERISK", DIF_OK,
					"The paste buffer is empty.");
				dialog.Responce();
			}
		}
		break;

	case EDIT_SIZE:
	case EDIT_MOVE:
		// Make sure we can move/size the object.
		if (currentObject && currentObject->parent && !FlagSet(currentObject->woFlags, WOF_NON_FIELD_REGION) &&
			!(ccode == EDIT_SIZE && currentObject->Inherited(ID_ICON)))
		{
			UI_WINDOW_OBJECT *currentResource = NULL;
			if (currentObject)
			{
				currentResource = currentObject;
				while(currentResource->parent)
					currentResource = currentResource->parent;
			}
			if (currentResource)
			{
				(void *)&(*windowManager + currentResource);
				UI_EVENT tEvent;
				tEvent.type = (ccode == EDIT_SIZE) ? L_SIZE : L_MOVE;
				if (ccode == EDIT_MOVE)
					tEvent.rawCode = M_LEFT_CHANGE | M_TOP_CHANGE |
						M_RIGHT_CHANGE | M_BOTTOM_CHANGE;
				else
					tEvent.rawCode = M_RIGHT_CHANGE | M_BOTTOM_CHANGE;
				tEvent.position.column = currentObject->true.right;
				tEvent.position.line = currentObject->true.bottom;
				currentObject->Modify(tEvent);
				UpdateStatusBar();
			}
		}
		else
		{
			errorSystem->Beep();
			break;
		}
		break;
	}
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
}

void CONTROL_WINDOW::ResourceOption(const UI_EVENT &event)
{
	// Find the current resource.
	UI_WINDOW_OBJECT *currentResource = NULL;
	if (currentObject)
	{
		currentResource = currentObject;
		while(currentResource->parent)
			currentResource = currentResource->parent;
	}

	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case RESOURCE_CREATE:
		{
		// Create a generic window (resource).
		UIW_WINDOW *window = UIW_WINDOW::Generic((display->columns /
			display->cellWidth - 30) / 2, (display->lines /
			display->cellHeight - 8) / 2, 30, 8, "Title");

		// Set edit mode and save status for the window.
		window->woStatus |= WOS_EDIT_MODE;
		window->designerAdvancedFlags = window->woAdvancedFlags;
		window->woAdvancedFlags = WOAF_LOCKED;

		// Generate a unique ID for the window.
		storage->ChDir("~UIW_WINDOW");
		int resourceNumber = 1;
		for (char *objectName = storage->FindFirstObject("*"); objectName;
			objectName = storage->FindNextObject())
		{
			if (!strncmp(objectName, "RESOURCE_", 9))
			{
				int i = atoi(objectName + 9);
				if (i >= resourceNumber)
					resourceNumber = i + 1;
			}
		}
		for (UI_WINDOW_OBJECT *object = windowManager->First(); object; object = object->Next())
		{
			char *objectName = object->StringID();
			if (!strncmp(objectName, "RESOURCE_", 9))
			{
				int i = atoi(objectName + 9);
				if (i >= resourceNumber)
					resourceNumber = i + 1;
			}
		}
		char name[32];
		sprintf(name, "RESOURCE_%d", resourceNumber);
		window->StringID(name);

		// Place window on screen.
		(void *)&(*windowManager + window);
		currentObject = window;
		UpdateStatusBar();
		}
		break;

	case RESOURCE_LOAD:
	case RESOURCE_STORE_AS:
	case RESOURCE_DELETE:
		ResourceAction(ccode);
		break;

	case RESOURCE_EDIT:
		if (currentResource)
		{
			// Send edit message.
			UI_EVENT tEvent;
			tEvent.type = D_EDIT_OBJECT;
			tEvent.data = currentResource;
			Event(tEvent);
		}
		else
			errorSystem->Beep();
		break;

	case RESOURCE_STORE:
	case RESOURCE_CLEAR:
		if (currentResource)
		{

			EVENT_TYPE responce = 0;
			if (ccode == RESOURCE_CLEAR)
			{
				DIALOG_WINDOW dialog("Clear Resource", "QUESTION", DIF_YES |
					DIF_NO | DIF_CANCEL, "%s\r\n\r\nStore resource before clearing?",
					currentResource->StringID());
				responce = dialog.Responce();
			}
			if (ccode == RESOURCE_STORE || responce == DIALOG_YES)
			{
				eventManager->DeviceState(E_MOUSE, DM_WAIT);
				// Store the resource.
				currentResource->Store(currentResource->StringID(), storage);
				saveNeeded = TRUE;
			}
			if (ccode == RESOURCE_CLEAR && responce != DIALOG_CANCEL)
			{
				// Clear the resource.
				(void *)&(*windowManager - currentResource);
				delete currentResource;
				for (currentResource = windowManager->First(); currentResource &&
					!FlagsSet(currentResource->woStatus, WOS_EDIT_MODE);
					currentResource = currentResource->Next())
				;
				currentObject = currentResource;
				UpdateStatusBar();
			}
		}
		else
			errorSystem->Beep();
		break;

	case RESOURCE_IMPORT:
		{
		// Get the file name from the user.
		FILE_WINDOW *fileWindow = new FILE_WINDOW(display->isText ? "FILE_WINDOW_TEXT" : "FILE_WINDOW", "Import File", "*.dat");
		(void *)&(*windowManager + fileWindow);
		UI_STORAGE *sourceStorage = NULL;
		char fileName[MAX_PATH];
		do
		{
			UI_EVENT event;
			eventManager->Get(event);
			if (event.type == FILE_SELECTED)
			{
				// Get file name.
				strcpy(fileName, fileWindow->FileName());
				if (!strchr(fileName, '.'))
					strcat(fileName, ".dat");

				// Check for valid file name.
				if (!UI_STORAGE::ValidName(fileName, FALSE))
				{
					DIALOG_WINDOW dialog("Error!", "ASTERISK", DIF_OK, "File not found:\r\n\r\n%s", fileName);
					dialog.Responce();
				}
				else
				{
					sourceStorage = new UI_STORAGE(fileName, UIS_READ);
					if (sourceStorage->storageError)
					{
						DIALOG_WINDOW dialog("Error!", "ASTERISK", DIF_OK, "Error opening file:\r\n\r\n%s", fileName);
						dialog.Responce();
						delete sourceStorage;
						sourceStorage = NULL;
					}
					else
						eventManager->Put(S_CLOSE);
				}
			}
			else
				UI_WINDOW_OBJECT::windowManager->Event(event);
		} while (UI_WINDOW_OBJECT::windowManager->Index(fileWindow) != -1);
		if (sourceStorage)
			(void *)&(*windowManager + new IMPORT_WINDOW(storage, sourceStorage));
		}
		break;
	case RESOURCE_TEST:
		{
		char tempName[MAX_PATH];											// BUG.1401
		tmpnam(tempName);
		UI_STORAGE testStorage(tempName, UIS_READWRITE | UIS_CREATE);
		if (testStorage.storageError)
		{
			errorSystem->Beep();
			break;
		}
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		UI_HELP_SYSTEM *saveHelpSystem = UI_WINDOW_OBJECT::helpSystem;
		UI_STORAGE *saveStorage = UI_WINDOW_OBJECT::defaultStorage;
		UI_LIST saveList;
		char fileName[128];
		storage->StorageName(fileName);
		UI_WINDOW_OBJECT::helpSystem = new UI_HELP_SYSTEM(fileName);
		UI_WINDOW_OBJECT::defaultStorage = storage;
		windowManager->screenID = ID_SCREEN;
		(void *)&(*windowManager - this);
 		UI_WINDOW_OBJECT *object = windowManager->Last();
		while (object)
		{
			if (FlagSet(object->woStatus, WOS_EDIT_MODE))
				object->Store(object->StringID(), &testStorage);
			(void *)&(*windowManager - object);
			(void *)&(saveList + object);
	 		object = windowManager->Last();
		}
		UIW_WINDOW *testWindow = new UIW_WINDOW(0, 0, 11, 3, WOF_NO_FLAGS, WOAF_LOCKED);
		(void *)&(*testWindow
			+ new UIW_TITLE("Testing")
			+ new UIW_BUTTON(0, 0, 0, "Exit Test", BTF_SEND_MESSAGE,
				WOF_JUSTIFY_CENTER | WOF_NON_FIELD_REGION, NULL, RESOURCE_END_TEST));
		(void *)&(*windowManager + testWindow);
		testStorage.ChDir("~UIW_WINDOW");
		char *resourceName = testStorage.FindFirstObject("*");
		while (resourceName)
		{
			if (strcmp(resourceName, ".") && strcmp(resourceName, ".."))
			{
				UI_WINDOW_OBJECT *object = new UIW_WINDOW(resourceName, &testStorage);
				object->woAdvancedFlags &= ~WOAF_MODAL;
				if (!FlagSet(object->woStatus, WOS_READ_ERROR))
					(void *)&(*windowManager + object);
				else
					delete object;
				testStorage.ChDir("~UIW_WINDOW");
			}
			resourceName = testStorage.FindNextObject();
		}
		testMode = TRUE;
		EVENT_TYPE ccode;
  		UI_EVENT tEvent;
		do
		{
			eventManager->Get(tEvent);
			ccode = windowManager->Event(tEvent);
		} while (tEvent.type != RESOURCE_END_TEST && ccode != L_EXIT);
		testMode = FALSE;
 		object = windowManager->Last();
		while (object)
		{
			(void *)&(*windowManager - object);
			delete object;
	 		object = windowManager->Last();
		}
 		object = (UI_WINDOW_OBJECT *)saveList.First();
		while (object)
		{
			(void *)&(saveList - object);
			object->woAdvancedFlags = WOAF_LOCKED;
			(void *)&(*windowManager + object);
	 		object = (UI_WINDOW_OBJECT *)saveList.First();
		}
		(void *)&(*windowManager + this);
		windowManager->screenID = screenID;
		delete UI_WINDOW_OBJECT::helpSystem;
		UI_WINDOW_OBJECT::helpSystem = saveHelpSystem;
		UI_WINDOW_OBJECT::defaultStorage = saveStorage;
		}
	}
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
}

void CONTROL_WINDOW::UtilOption(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case UTIL_BITMAP:
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		(void *)&(*windowManager + new IMAGE_EDITOR);
		break;

	case UTIL_HELP:
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		(void *)&(*windowManager + new HELP_EDITOR);
		break;
 	}
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
}

void CONTROL_WINDOW::FileAction(EVENT_TYPE action, char *fileName)
{
	char fileNameBuffer[MAX_PATH];

	// Save current file if necessary.
	if ((action == FILE_NEW || action == FILE_FOPEN) && saveNeeded)
	{
		storage->StorageName(fileNameBuffer);
		if (ui_stricmp(fileNameBuffer, "UNTITLED.DAT"))
		{
			DIALOG_WINDOW dialog("New File", "QUESTION", DIF_YES | DIF_NO | DIF_CANCEL, "Save current file %s?", fileNameBuffer);
			EVENT_TYPE responce = dialog.Responce();
			if (responce == DIALOG_YES)
				FileAction(FILE_SAVE);
			else if (responce == DIALOG_CANCEL)
				return;
		}
	}

	// Get file name from user.
	if (action != FILE_SAVE && !fileName)
	{
		// Initialize title for file window.
		char *title;
		switch (action)
		{
		case FILE_NEW:  title = "New";  break;
		case FILE_FOPEN:	title = "Open";	 break;
		case FILE_SAVE_AS:  title = "Save As";  break;
		case FILE_DELETE:  title = "Delete";  break;
		}

		// Get the file name from the user.
		FILE_WINDOW *fileWindow = new FILE_WINDOW(display->isText ? "FILE_WINDOW_TEXT" : "FILE_WINDOW", title, "*.dat");
		(void *)&(*windowManager + fileWindow);
		do
		{
			UI_EVENT event;
			eventManager->Get(event);
			if (event.type == FILE_SELECTED)
			{
				// Get file name.
				strcpy(fileNameBuffer, fileWindow->FileName());
				if (!strchr(fileNameBuffer, '.'))
					strcat(fileNameBuffer, ".dat");

				// Check for valid file name.
				int actionReady = FALSE;
				if (!UI_STORAGE::ValidName(fileNameBuffer, FALSE) && !UI_STORAGE::ValidName(fileNameBuffer, TRUE))
				{
					DIALOG_WINDOW dialog("Error!", "ASTERISK", DIF_OK, "Invalid file name:\r\n\r\n%s", fileNameBuffer);
					dialog.Responce();
				}
				else if ((action == FILE_FOPEN || action == FILE_DELETE) && !UI_STORAGE::ValidName(fileNameBuffer, FALSE))
				{
					DIALOG_WINDOW dialog("Error!", "ASTERISK", DIF_OK, "File not found:\r\n\r\n%s", fileNameBuffer);
					dialog.Responce();
				}
				else if ((action == FILE_NEW || action == FILE_SAVE_AS) && UI_STORAGE::ValidName(fileNameBuffer, FALSE))
				{
					// Warn user before overwriting file.
					DIALOG_WINDOW dialog("New", "QUESTION", DIF_YES | DIF_NO,
						"File %s already exists.\r\n\r\nOverwrite it?", fileNameBuffer);
					if (dialog.Responce() == DIALOG_YES)
						actionReady = TRUE;
				}
				else if (action == FILE_DELETE && UI_STORAGE::ValidName(fileNameBuffer, FALSE))
				{
					// Warn user before deleting file.
					DIALOG_WINDOW dialog("Delete File", "EXCLAMATION", DIF_OK | DIF_CANCEL,
						"%s\r\n\r\nThis file will be deleted", fileNameBuffer);
					if (dialog.Responce() == DIALOG_OK)
						actionReady = TRUE;
				}
				else
					actionReady = TRUE;
				if (actionReady)
				{
					fileName = fileNameBuffer;
					eventManager->Put(S_CLOSE);
				}
			}
			else
				UI_WINDOW_OBJECT::windowManager->Event(event);
		} while (UI_WINDOW_OBJECT::windowManager->Index(fileWindow) != -1);
		if (!fileName)
			return;
	}

	// Perform the action.
	eventManager->DeviceState(E_MOUSE, DM_WAIT);
	if (action == FILE_NEW || action == FILE_FOPEN)
	{
		UI_STORAGE *oldStorage = storage;
		storage = new UI_STORAGE(fileName, (action == FILE_NEW) ? UIS_READWRITE | UIS_CREATE : UIS_READWRITE);
		if (storage->storageError)
		{
			DIALOG_WINDOW dialog("Error!", "EXCLAMATION", DIF_OK, (action == FILE_NEW) ?
				"Error creating file." : "Error opening file.");
			dialog.Responce();
			delete storage;
			storage = oldStorage;
		}
		else
		{
			if (oldStorage)
				delete oldStorage;
			if (action == FILE_NEW)
			{
				storage->MkDir("UIW_WINDOW");
				storage->MkDir("UI_BITMAP");
				storage->MkDir("UI_ICON");
				storage->MkDir("UI_HELP");
			}
			LoadDeriveTable();
		}
	}
	else if (action == FILE_SAVE || action == FILE_SAVE_AS)
		SaveFile(fileName);
	else if (action == FILE_DELETE)
		remove(fileName);

	// Place file name in title.
	if (storage)
	{
		char title[MAX_PATH];
		storage->StorageName(title);
		UI_STORAGE::StripFullPath(title, 0, fileNameBuffer);
		sprintf(title, "Zinc Designer - %s", fileNameBuffer);
		Information(SET_TEXT, title);
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
	}
}

void CONTROL_WINDOW::ResourceAction(EVENT_TYPE action)
{
	char resourceName[MAX_PATH];

	// Find the current resource.
	UI_WINDOW_OBJECT *currentResource = NULL;
	if (currentObject)
	{
		currentResource = currentObject;
		while(currentResource->parent)
			currentResource = currentResource->parent;
	}

	if (action != RESOURCE_LOAD && action != RESOURCE_DELETE && !currentResource)
	{
		errorSystem->Beep();
		return;
	}

	if (action != RESOURCE_STORE)
	{
		// Initialize title for file window.
		char *title;
		switch (action)
		{
		case RESOURCE_LOAD:  title = "Load";  break;
		case RESOURCE_STORE_AS:  title = "Store As";  break;
		case RESOURCE_DELETE:  title = "Delete";  break;
		}

		RESOURCE_WINDOW *resourceWindow = new RESOURCE_WINDOW(title);
		(void *)&(*windowManager + resourceWindow);
		int actionReady = FALSE;
		do
		{
			UI_EVENT event;
			eventManager->Get(event);
			if (event.type == RESOURCE_SELECTED)
			{
				// Get image name.
				char *testName = resourceWindow->ResourceName();
				storage->ChDir("~UIW_WINDOW");

				// Check for valid resource name.
				char *objectName = storage->FindFirstObject(testName);
				if ((action == RESOURCE_LOAD || action == RESOURCE_DELETE) && !objectName)
				{
					DIALOG_WINDOW dialog("Load", "ASTERISK", DIF_OK, "%s\r\n\r\nResource not found.", testName);
					dialog.Responce();
				}
				else if (action == RESOURCE_STORE_AS && objectName)
				{
					DIALOG_WINDOW dialog("Store As", "QUESTION", DIF_YES | DIF_NO,
						"Resource %s already exists.\r\n\r\nOverwrite?", testName);
					if (dialog.Responce() == DIALOG_YES)
						actionReady = TRUE;
				}
				else if (action == RESOURCE_DELETE)
				{
					DIALOG_WINDOW dialog("Delete Resource", "EXCLAMATION", DIF_OK | DIF_CANCEL,
						"%s\r\n\r\nThis resource will be deleted.", testName);
					if (dialog.Responce() == DIALOG_OK)
						actionReady = TRUE;
				}
				else
					actionReady = TRUE;
				if (actionReady)
				{
					strcpy(resourceName, testName);
					eventManager->Put(S_CLOSE);
				}
			}
			else
				UI_WINDOW_OBJECT::windowManager->Event(event);
		} while (UI_WINDOW_OBJECT::windowManager->Index(resourceWindow) != -1);
		if (!actionReady)
			return;
	}
	if (action == RESOURCE_LOAD)
	{
		UI_WINDOW_OBJECT::defaultStatus |= WOS_EDIT_MODE;
		currentObject = new UIW_WINDOW(resourceName, storage);
		UI_WINDOW_OBJECT::defaultStatus &= ~WOS_EDIT_MODE;
		currentObject->woAdvancedFlags = WOAF_LOCKED;
		(void *)&(*windowManager + currentObject);
	}
	else if (action == RESOURCE_STORE_AS)
	{
		currentResource->StringID(resourceName);
		ResourceAction(RESOURCE_STORE);
	}
	else if (action == RESOURCE_STORE)
	{
		currentResource->Store(currentResource->StringID(), storage);
		UpdateStatusBar();
	}
	else if (action == RESOURCE_DELETE)
	{
		storage->ChDir("~UIW_WINDOW");
		storage->DestroyObject(resourceName);
		storage->ChDir("~UI_HPP");
		storage->DestroyObject(resourceName);
		storage->ChDir("~UI_CPP");
		storage->ChDir(resourceName);
		storage->DestroyObject("OBJECTID");
		storage->DestroyObject("COMPARE_FUNCTION");
		storage->DestroyObject("USER_FUNCTION");
		storage->ChDir("..");
		storage->RmDir(resourceName);
		saveNeeded = TRUE;
	}
}

void CONTROL_WINDOW::LoadDeriveTable(void)
{
	// Clear the derive table.
	for (int i = 0; deriveTable[i].data; i++)
	{
		deriveTable[i].value = ID_END;
		deriveTable[i].data = NULL;
		deriveTable[i].text = NULL;
		deriveTable[i].flags = 0;
	}

	// Read the derive table.
	storage->ChDir("~");
	UI_STORAGE_OBJECT deriveObject(*storage, "DERIVE_TABLE", 0, UIS_READWRITE);
	if (!deriveObject.objectError)
	{
		int index = 0;
		OBJECTID value;
		while (deriveObject.Load(&value) && value != ID_END)
		{
			deriveTable[index].value = value;
			deriveObject.Load(&value);
			for (int j = 0; UI_WINDOW_OBJECT::objectTable[j].data; j++)
				if (UI_WINDOW_OBJECT::objectTable[j].value == value)
				{
					deriveTable[index].data = UI_WINDOW_OBJECT::objectTable[j].data;
					break;
				}
			deriveObject.Load(&deriveTable[index].text);
			++index;
		}
	}
}

int CONTROL_WINDOW::BuildWindow(void)
{
	int height = 3;
	if (FlagSet(_config->options, OPTION_TOOL_BAR))
	{
		if (_config->toolBar == TOOL_BITMAP || display->isText)
			height += 2;
		else if (_config->toolBar == TOOL_TEXT)
			height += 3;
		else
			height += 6;
		// Rebuild tool bar according to configutation information.
		if (!toolBar)
		{
			if (statusBar)
			{
				(void *)&(*this - statusBar);
				delete statusBar;
				statusBar = NULL;
			}
			(void *)&(*this + (toolBar = new UIW_TOOL_BAR(0, 0, 0, 0)));
			if (display->isText)
				toolBar->paletteMapTable = _textPaletteTable;
			toolBar->helpContext = OBJECT_BAR_HELP;
		}
		toolBar->Destroy();
		for (UI_ELEMENT *element = _config->toolList.First(); element; element = element->Next())
		{
			TOOL_BUTTON_ELEMENT *toolElement = (TOOL_BUTTON_ELEMENT *)element;
			if (toolElement->data.used)
			{
				if (display->isText)
				{
					UIW_BUTTON *button = new UIW_BUTTON(0, 0, 10, toolElement->data.objectName,
							BTF_NO_TOGGLE | BTF_SEND_MESSAGE,
							WOF_BORDER, NULL, (EVENT_TYPE)toolElement->data.createID,
							NULL);
					button->paletteMapTable = _textPaletteTable;
					(void *)&(*toolBar + button);
				}
				else if (_config->toolBar == TOOL_BITMAP)
				{
					(void *)&(*toolBar
						+ new UIW_BUTTON(0, 0, 6, NULL, BTF_NO_TOGGLE |
							BTF_AUTO_SIZE | BTF_SEND_MESSAGE, WOF_JUSTIFY_CENTER,
							NULL, (EVENT_TYPE)toolElement->data.createID,
							toolElement->data.bitmapName));
				}
				else if (_config->toolBar == TOOL_TEXT)
				{
					(void *)&(*toolBar
						+ new UIW_BUTTON(0, 0, 10, toolElement->data.objectName,
							BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE,
							WOF_JUSTIFY_CENTER, NULL, (EVENT_TYPE)toolElement->data.createID,
							NULL));
				}
				else
				{
					(void *)&(*toolBar
						+ new UIW_BUTTON(0, 0, 10, toolElement->data.objectName,
							BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE,
							WOF_JUSTIFY_CENTER, NULL, (EVENT_TYPE)toolElement->data.createID,
							toolElement->data.bitmapName));
				}
			}
		}
	}
	else if (toolBar)
	{
		(void *)&(*this - toolBar);
		delete toolBar;
		toolBar = NULL;
	}
	if (FlagSet(_config->options, OPTION_STATUS_BAR))
	{
		height += 2;
		if (!statusBar)
		{
			(void *)&(*this + (statusBar = new UIW_TOOL_BAR(0, 0, 0, 2, WNF_NO_WRAP,
				WOF_BORDER | WOF_NON_FIELD_REGION, WOAF_NON_CURRENT)));
			(void *)&(*statusBar
				+ new UIW_PROMPT(1, 0, "object:")
				+ (objectString = new UIW_STRING(11, 0, 15, NULL, 15,
					STF_NO_FLAGS, WOF_BORDER | WOF_NON_SELECTABLE))
				+ new UIW_PROMPT(1, 1, "stringID:")
				+ (idString = new UIW_STRING(11, 1, 15, NULL, 15, STF_NO_FLAGS,
					WOF_BORDER | WOF_NON_SELECTABLE))
				+ new UIW_PROMPT(28, 0, "pos:")
				+ (posString = new UIW_STRING(34, 0, 14, NULL, 15, STF_NO_FLAGS,
					WOF_BORDER | WOF_NON_SELECTABLE))
				+ new UIW_PROMPT(28, 1, "size:")
				+ (sizeString = new UIW_STRING(34, 1, 14, NULL, 15, STF_NO_FLAGS,
					WOF_BORDER | WOF_NON_SELECTABLE))
				+ new UIW_PROMPT(50, 0, "place object:")
				+ (placeString = new UIW_STRING(50, 1, 15, NULL, 15, STF_NO_FLAGS,
					WOF_BORDER | WOF_NON_SELECTABLE)));
			statusBar->helpContext = OBJECT_BAR_HELP;
		}
	}
	else if (statusBar)
	{
		(void *)&(*this - statusBar);
		delete statusBar;
		statusBar = NULL;
	}
	if (FlagSet(_config->options, OPTION_HELP_BAR))
	{
		++height;
		if (!helpBar)
			(void *)&(*this + (helpBar = new HELP_BAR));
	}
	else if (helpBar)
	{
		(void *)&(*this - helpBar);
		delete helpBar;
		helpBar = NULL;
	}
	return height;
}

void CONTROL_WINDOW::UpdateStatusBar(void)
{
	if (statusBar)
	{
		if (currentObject)
		{
			if (currentObject->SearchID() < ID_DERIVE_START)
			{
				for (int i = 0; _toolData[i].data; i++)
					if (_toolData[i].value == D_CREATE_OBJECT + currentObject->SearchID())
					{
						objectString->DataSet(_toolData[i].text);
						break;
					}
			}
			else
				objectString->DataSet("Derive");
			idString->DataSet(currentObject->StringID());
			char coordinates[15];

			UI_REGION region;												// BUG.General
#if defined(ZIL_MOTIF)
				region = currentObject->relative;
#else
			if (currentObject->parent)
			{
				region = currentObject->relative;
				if (!FlagSet(currentObject->woFlags, WOF_NON_FIELD_REGION) &&
					!FlagSet(currentObject->woFlags, WOF_MINICELL))
				{
					region.top -= display->preSpace;
					region.bottom += display->postSpace;
				}
			}
			else
				region = currentObject->true;
#endif
			int width, height;
			char *minicell;
			if (FlagSet(currentObject->woFlags, WOF_MINICELL))
			{
				minicell = " (M)";
				long miniNX = display->miniNumeratorX, miniDX = display->miniDenominatorX;
				long miniNY = display->miniNumeratorY, miniDY = display->miniDenominatorY;
				width = (region.Width() * miniDX) / (miniNX * display->cellWidth);
				height = (region.Height() * miniDY) / (miniNY * display->cellHeight);
				region.left = (region.left * miniDX) / (miniNX * display->cellWidth);
				region.bottom++;
				region.bottom = (region.bottom * miniDY) / (miniNY * display->cellHeight);
				region.bottom--;
				region.right = region.left + width - 1;
				region.top = region.bottom - height + 1;
			}
			else
			{
				minicell = "";
				width = region.Width() / display->cellWidth;
				height = region.Height() / display->cellHeight;
				region.left /= display->cellWidth;
				region.bottom++;
				region.bottom /= display->cellHeight;
				region.bottom--;
				region.top = region.bottom - height + 1;
			}

			sprintf(coordinates, "%d, %d%s", region.left, region.top, minicell);
			posString->DataSet(coordinates);
			sprintf(coordinates, "%d, %d%s", width, height, minicell);
			sizeString->DataSet(coordinates);
		}
		else
		{
			objectString->DataSet("None");
			idString->DataSet("");
			posString->DataSet("");
			sizeString->DataSet("");
		}
	}
	saveNeeded = TRUE;
}

void CONTROL_WINDOW::UpdatePlaceString(OBJECTID objectID)
{
	int i;
	if (statusBar)
	{
		for (i = 0; _toolData[i].data; i++)
		{
			if (_toolData[i].value == objectID + D_CREATE_OBJECT)
			{
				placeString->DataSet(_toolData[i].text);
		 		break;
			}
		}
		if (!_toolData[i].data)
			placeString->DataSet("None");
	}
}

class USER_FUNCTION_ELEMENT : public UI_ELEMENT
{
public:
	char *userFunctionName;

	USER_FUNCTION_ELEMENT(char *_userFunctionName)
	{ userFunctionName = _userFunctionName; }
	~USER_FUNCTION_ELEMENT(void)
	{ delete userFunctionName; }
};

static int FindUserFunctionName(void *element1, void *element2)
{
	return strcmp(((USER_FUNCTION_ELEMENT *)element1)->userFunctionName, (char *)element2);
}

void CONTROL_WINDOW::SaveFile(char *newName, int memErr)
{

	char fileName[256];
	if (newName)
		strcpy(fileName, newName);
	else
		storage->StorageName(fileName);

	UIW_WINDOW *saveWindow = new UIW_WINDOW("SAVE_WINDOW", defaultStorage);
	((UIW_TITLE *)saveWindow->Information(GET_STRINGID_OBJECT, "NUMID_TITLE"))->Information(SET_TEXT, fileName);
	windowManager->Center(saveWindow);
	(void *)&(*windowManager + saveWindow);
	eventManager->DeviceState(E_MOUSE, DM_WAIT);
	UI_WINDOW_OBJECT *messageField = (UI_WINDOW_OBJECT *)saveWindow->Information(GET_STRINGID_OBJECT, "MESSAGE_FIELD");

#if defined(ZIL_MSDOS)
	messageField->paletteMapTable = _textPaletteTable;
#endif

 	storage->ChDir("~UIW_WINDOW");

	// Store all resources on the screen before saveing.
	messageField->Information(SET_TEXT, "Storing resources...");
#if defined(ZIL_MSWINDOWS)
	UpdateWindow(messageField->screenID);
#endif
	UI_WINDOW_OBJECT *object = windowManager->First();
	while (object)
	{
		UI_WINDOW_OBJECT *tObject = object->Next();
		if (FlagSet(object->woStatus, WOS_EDIT_MODE))
		{
			object->Store(object->StringID(), storage);
			if (memErr)
			{
				(void *)&(*windowManager - object);
				delete object;
			}
		}
		object = tObject;
	}
	
	// Generate Tables
	messageField->Information(SET_TEXT, "Generating Tables...");
#if defined(ZIL_MSWINDOWS)
	UpdateWindow(messageField->screenID);
#endif
	for (int i = 0; objectTable[i].data; i++)
		objectTable[i].flags = FALSE;
	storage->ChDir("~UI_CPP");
	UI_LIST userFunctionList;
	UI_LIST compareFunctionList;
	for (char *resourceDir = storage->FindFirstObject("*"); resourceDir;
		resourceDir = storage->FindNextObject())
	{
		if (resourceDir[0] != '.')
		{
			storage->ChDir(resourceDir);
			UI_STORAGE_OBJECT objectIDEntry(*storage, "OBJECTID", 0, UIS_READ);
			if (!objectIDEntry.objectError)
			{
				OBJECTID objectID;
				do
				{
					objectIDEntry.Load(&objectID);
					for (int i = 0; objectTable[i].data; i++)
						if (objectTable[i].value == objectID)
							objectTable[i].flags = TRUE;
				} while (objectID);
			}
			UI_STORAGE_OBJECT userFuncEntry(*storage, "USER_FUNCTION", 0, UIS_READ);
			if (!userFuncEntry.objectError)
			{
				char *functionName;
				do
				{
					userFuncEntry.Load(&functionName);
					if (functionName && !userFunctionList.Get(FindUserFunctionName, functionName))
						userFunctionList.Add(new USER_FUNCTION_ELEMENT(functionName));
				} while (functionName);
			}
			UI_STORAGE_OBJECT compareFuncEntry(*storage, "COMPARE_FUNCTION", 0, UIS_READ);
			if (!compareFuncEntry.objectError)
			{
				char *functionName;
				do
				{
					compareFuncEntry.Load(&functionName);
					if (functionName && !compareFunctionList.Get(FindUserFunctionName, functionName))
						compareFunctionList.Add(new USER_FUNCTION_ELEMENT(functionName));
				} while (functionName);
			}
			storage->ChDir("..");
		}
	}

	// Generate the HPP file.
	messageField->Information(SET_TEXT, "Generating .HPP and .CPP files...");
#if defined(ZIL_MSWINDOWS)
	UpdateWindow(messageField->screenID);
#endif
	UI_STORAGE::ChangeExtension(fileName, ".hpp");
	FILE *includeFile = fopen(fileName, "w+");

	if (!includeFile)
		return;

	if (deriveTable[0].data)
		fprintf(includeFile, "//  Derived Classes...\n");
	for (i = 0; deriveTable[i].data; i++)
		if (deriveTable[i].flags)
			fprintf(includeFile, "const OBJECTID\tID_%-32s\t = %d;\n",
				deriveTable[i].text, deriveTable[i].value);
	if (deriveTable[0].data)
		fprintf(includeFile, "\n\n");

	fprintf(includeFile, "//  Help Contexts...\n");
	storage->ChDir("~UI_HELP");
	for (char *helpName = storage->FindFirstObject("*"); helpName;
		helpName = storage->FindNextObject())
	{
		if (helpName[0] != '.')
		{
			UI_STORAGE_OBJECT sObject(*storage, helpName, 0, UIS_READ);
			char *title = NULL;
			sObject.Load(&title);
			fprintf(includeFile, "const UI_HELP_CONTEXT %-32s = 0x%04X; // %s\n", helpName, sObject.objectID, title);
		}
	}
	fprintf(includeFile, "\n\n");

	char line[256];
	if (!storage->ChDir("~UI_HPP"))											// BUG.1693
	{
		for (char *entry = storage->FindFirstObject("*"); entry; entry = storage->FindNextObject())
			if (strcmp(entry, ".") && strcmp(entry, "..") && strcmp(entry, "HELP_CONTEXTS"))
			{
				fprintf(includeFile, "#ifdef USE_%s\n", entry);
				UI_STORAGE_OBJECT hppEntry(*storage, entry, 0, UIS_READ);
				for (hppEntry.Load(line, 256); strcmp(line, ""); hppEntry.Load(line, 256))
					fprintf(includeFile, "%s\n", line);
				fprintf(includeFile, "#endif // USE_%s\n\n", entry);
			}
	}
	fclose (includeFile);

	// Write the .cpp file.
	UI_STORAGE::ChangeExtension(fileName, ".cpp");
	FILE *jumpFile = fopen(fileName, "w+");

	if (!jumpFile)
		return;

	char includeFileName[MAX_PATH];											// BUG.????
	UI_STORAGE::StripFullPath(fileName, NULL, includeFileName);
	UI_STORAGE::ChangeExtension(includeFileName, ".hpp");
	fprintf(jumpFile, "#include <ui_win.hpp>\n");
	fprintf(jumpFile, "#include \"%s\"\n", includeFileName);
 	fprintf(jumpFile, "#if defined(_MSC_VER)\n#pragma hdrstop\n#endif\n");
	fprintf(jumpFile, "#if defined(ZIL_LINKBUG)\nvoid z_jump_dummy(void) { }   // Bug fix for broken linkers.\n#endif\n\n");

	for (UI_ELEMENT *element = userFunctionList.First(); element; element = element->Next())
		fprintf(jumpFile, "extern EVENT_TYPE %s(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE);\n",
			((USER_FUNCTION_ELEMENT *)element)->userFunctionName);

	for (element = compareFunctionList.First(); element; element = element->Next())
		fprintf(jumpFile, "extern int %s(void *, void *);\n",
			((USER_FUNCTION_ELEMENT *)element)->userFunctionName);

	fprintf(jumpFile, "\nstatic UI_ITEM _userTable[] =\n{\n");
	for (element = userFunctionList.First(); element; element = element->Next())
		fprintf(jumpFile, "\t{ 0, VOIDF(%s), \"%s\", 0 },\n", ((USER_FUNCTION_ELEMENT *)element)->userFunctionName,
			((USER_FUNCTION_ELEMENT *)element)->userFunctionName);

	for (element = compareFunctionList.First(); element; element = element->Next())
		fprintf(jumpFile, "\t{ 0, VOIDF(%s), \"%s\", 0 },\n", ((USER_FUNCTION_ELEMENT *)element)->userFunctionName,
			((USER_FUNCTION_ELEMENT *)element)->userFunctionName);
	fprintf(jumpFile, "\t{ ID_END, NULL, NULL, 0 }\n};\n");
	fprintf(jumpFile, "UI_ITEM *UI_WINDOW_OBJECT::userTable = _userTable;\n\n");

	fprintf(jumpFile, "static UI_ITEM _objectTable[] =\n{\n");
	for (i = 0; objectTable[i].data; i++)
		if (objectTable[i].flags)
		{
			if (objectTable[i].value < ID_DERIVE_START)
				fprintf(jumpFile, "\t{ ID_%s, VOIDF(UIW_%s::New), \"%s\", 0 },\n",
					objectTable[i].text, objectTable[i].text,
					objectTable[i].text);
			else
				fprintf(jumpFile, "\t{ ID_%s, VOIDF(%s::New), \"%s\", 0 },\n",
					objectTable[i].text, objectTable[i].text,
					objectTable[i].text);
		}

	fprintf(jumpFile, "\t{ ID_END, NULL, NULL, 0 }\n};\n");
	fprintf(jumpFile, "UI_ITEM *UI_WINDOW_OBJECT::objectTable = _objectTable;\n");
	fclose (jumpFile);

	messageField->Information(SET_TEXT, "Writing data file...");
#if defined(ZIL_MSWINDOWS)
	UpdateWindow(messageField->screenID);
#endif
	storage->ChDir("~");
	UI_STORAGE_OBJECT deriveTableObject(*storage, "DERIVE_TABLE", 0, UIS_READWRITE | UIS_CREATE);
	if (!deriveTableObject.objectError)
	{
		for (i = 0; deriveTable[i].data; i++)
			if (deriveTable[i].flags)
			{
				deriveTableObject.Store((OBJECTID)deriveTable[i].value);
				for (int j = 0; objectTable[j].data; j++)
					if (objectTable[j].data == deriveTable[i].data)
					{
						deriveTableObject.Store((OBJECTID)objectTable[j].value);
						break;
					}
				deriveTableObject.Store(deriveTable[i].text);
			}
		deriveTableObject.Store(ID_END);
	}
	if (newName)
	 	storage->SaveAs(newName, _config->backups);
	else
	 	storage->Save(_config->backups);
	saveNeeded = FALSE;

	(void *)&(*windowManager - saveWindow);
	delete saveWindow;

	eventManager->DeviceState(E_MOUSE, DM_VIEW);
}
