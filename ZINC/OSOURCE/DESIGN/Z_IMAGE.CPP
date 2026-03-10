//	Zinc Interface Library Designer - Z_IMAGE.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
#include <mem.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui_dsn.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

static UCHAR _colorBitmap[] =
{
	2, 8, 20, 15,			// width, height, ratioWidth, ratioHeight
	BLACK, WHITE,
	DARKGRAY, LIGHTGRAY,
	BLUE, LIGHTBLUE,
	GREEN, LIGHTGREEN,
	CYAN, LIGHTCYAN,
	RED, LIGHTRED,
	MAGENTA, LIGHTMAGENTA,
	BROWN, YELLOW
};

static UCHAR _monoBitmap[] =
{
	1, 2, 40, 40,			// width, height, ratioWidth, ratioHeight
	BLACK, WHITE
};

IMAGE_EDITOR::IMAGE_EDITOR(void) :
	UIW_WINDOW("IMAGE_EDITOR", defaultStorage),	imageID(ID_ICON_IMAGE),
	imageWidth(32),	imageHeight(32), drawMode(IMAGE_PENCIL), brushSize(3)
{
	// Initialize data.
	strcpy(imageName, "");
	leftRightColors[0] = (UCHAR)BLACK;
	leftRightColors[1] = (UCHAR)WHITE;
	screenColor = display->isMono ? (UCHAR)WHITE : (UCHAR)LIGHTGRAY;
	imageArray = new UCHAR[imageWidth * imageHeight];
	undoArray = new UCHAR[imageWidth * imageHeight];
	memset(imageArray, (int)WHITE, imageWidth * imageHeight);
	memset(undoArray, (int)WHITE, imageWidth * imageHeight);

 	// Initialize bitmap pointers.
	colorBitmap = (BITMAP_OBJECT *)Information(GET_STRINGID_OBJECT, "COLOR_BITMAP");
	colorBitmap->editor = this;
	if (display->isMono)
	{
		colorBitmap->width = (USHORT)_monoBitmap[0];
		colorBitmap->height = (USHORT)_monoBitmap[1];
		colorBitmap->ratioWidth = (USHORT)_monoBitmap[2];
		colorBitmap->ratioHeight = (USHORT)_monoBitmap[3];
		colorBitmap->bitmapArray = &_monoBitmap[4];
	}
	else
	{
		colorBitmap->width = (USHORT)_colorBitmap[0];
		colorBitmap->height = (USHORT)_colorBitmap[1];
		colorBitmap->ratioWidth = (USHORT)_colorBitmap[2];
		colorBitmap->ratioHeight = (USHORT)_colorBitmap[3];
		colorBitmap->bitmapArray = &_colorBitmap[4];
	}

	editBitmap = (BITMAP_OBJECT *)Information(GET_STRINGID_OBJECT, "EDIT_BITMAP");
	editBitmap->editor = this;
	editBitmap->width = imageWidth;
	editBitmap->height = imageHeight;
	editBitmap->bitmapArray = imageArray;

	viewBitmap = (BITMAP_OBJECT *)Information(GET_STRINGID_OBJECT, "VIEW_BITMAP");
	viewBitmap->editor = this;
	viewBitmap->width = imageWidth;
	viewBitmap->height = imageHeight;
	viewBitmap->ratioWidth = viewBitmap->ratioHeight = 1;
	viewBitmap->bitmapArray = imageArray;

	screenBitmap = (BITMAP_OBJECT *)Information(GET_STRINGID_OBJECT, "SCREEN_BITMAP");
	screenBitmap->editor = this;
	screenBitmap->width = screenBitmap->height = 1;
	screenBitmap->ratioWidth = 40;
	screenBitmap->ratioHeight = 10;
	screenBitmap->bitmapArray = &screenColor;

	leftRightBitmap = (BITMAP_OBJECT *)Information(GET_STRINGID_OBJECT, "LT_RT_BITMAP");
	leftRightBitmap->editor = this;
	leftRightBitmap->width = 2;
	leftRightBitmap->height = 1;
	leftRightBitmap->ratioWidth = 20;
	leftRightBitmap->ratioHeight = 10;
	leftRightBitmap->bitmapArray = leftRightColors;

	// Initialize default menu selections.
	((UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "PENCIL_BUTTON"))->woStatus |= WOS_SELECTED;
	((UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "PENCIL_ITEM"))->woStatus |= WOS_SELECTED;
	((UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "GRID_ITEM"))->woStatus |= WOS_SELECTED;
	((UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "DEFAULT_BRUSH"))->woStatus |= WOS_SELECTED;
}

IMAGE_EDITOR::~IMAGE_EDITOR(void)
{
	delete imageArray;
	delete undoArray;
}

EVENT_TYPE IMAGE_EDITOR::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case OPTION_NEW:
	case OPTION_LOAD:
	case OPTION_SAVE:
	case OPTION_SAVE_AS:
	case OPTION_DELETE:
		ImageAction(ccode);
		break;

	case OPTION_IMPORT:
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2)
		{
		// Get the file name from the user.
		FILE_WINDOW *fileWindow = new FILE_WINDOW(display->isText ? "FILE_WINDOW_TEXT" : "FILE_WINDOW", "Import Image", "*.bmp");
		*windowManager + fileWindow;
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

				// Check for valid file name.
				if (!UI_STORAGE::ValidName(fileName, FALSE))
				{
					DIALOG_WINDOW dialog("Error!", "ASTERISK", DIF_OK, "File not found:\r\n\r\n%s", fileName);
					dialog.Responce();
				}
				else
				{
					if (Import(fileName))
						eventManager->Put(S_CLOSE);
				}
			}
			else
				UI_WINDOW_OBJECT::windowManager->Event(event);
		} while (UI_WINDOW_OBJECT::windowManager->Index(fileWindow) != -1);
		if (sourceStorage)
			*windowManager + new IMPORT_WINDOW(storage, sourceStorage);
		}
#endif
		break;

	case OPTION_EXIT:
		eventManager->Put(UI_EVENT(S_CLOSE, 0));
		break;

	case IMAGE_CLEAR:
		memset(imageArray, (UCHAR)WHITE, imageWidth * imageHeight);
		editBitmap->Event(S_REDISPLAY);
		viewBitmap->Event(S_REDISPLAY);
		break;

	case IMAGE_PENCIL:
	case IMAGE_BRUSH:
	case IMAGE_LINE:
	case IMAGE_RECTANGLE:
	case IMAGE_FILL_RECTANGLE:
	case IMAGE_ELLIPSE:
	case IMAGE_FILL_ELLIPSE:
	case IMAGE_FILL:
		{
		UI_WINDOW_OBJECT *modeItem = (UI_WINDOW_OBJECT *)Information(GET_STRINGID_OBJECT, "PENCIL_ITEM");
		while (modeItem)
		{
			EVENT_TYPE value;
			modeItem->Information(GET_VALUE, &value);
			if (value == ccode && !FlagSet(modeItem->woStatus, WOS_SELECTED))
			{
				modeItem->woStatus |= WOS_SELECTED;
				modeItem->Information(CHANGED_FLAGS, NULL);
			}
			else if (value != ccode && FlagSet(modeItem->woStatus, WOS_SELECTED))
			{
				modeItem->woStatus &= ~WOS_SELECTED;
				modeItem->Information(CHANGED_FLAGS, NULL);
			}
			modeItem = modeItem->Next();
		}
		drawMode = ccode;
		}
		break;

	case IMAGE_GRID:
		editBitmap->userFlags ^= BMF_GRID;
		editBitmap->Event(S_REDISPLAY);
		break;

	case IMAGE_BRUSH_SIZE:
	 	brushSize = ((UIW_POP_UP_ITEM *)Information(GET_STRINGID_OBJECT, "BRUSH_ITEM"))->
			menu.Index((UIW_POP_UP_ITEM *)event.data) + 2;
		break;

	case IMAGE_UNDO:
		memcpy(imageArray, undoArray, imageWidth * imageHeight);
		editBitmap->Event(S_REDISPLAY);
		viewBitmap->Event(S_REDISPLAY);
		break;

	case IMAGE_SAVE_UNDO:
		memcpy(undoArray, imageArray, imageWidth * imageHeight);
		break;

	case S_SIZE:
		UIW_WINDOW::Event(event);
//		Event(S_REDISPLAY);
		break;

	default:
  		ccode = UIW_WINDOW::Event(event);
	}
	return ccode;
}

void IMAGE_EDITOR::ImageAction(EVENT_TYPE action)
{
	if (action == OPTION_SAVE && !*imageName)
		action = OPTION_SAVE_AS;

	if (action == OPTION_SAVE)
	{
		// Store the image.
		CONTROL_WINDOW::storage->ChDir(imageID == ID_BITMAP_IMAGE ? "~UI_BITMAP" : "~UI_ICON");
		UI_STORAGE_OBJECT sObject(*CONTROL_WINDOW::storage, imageName, 0, UIS_READWRITE | UIS_CREATE);
		sObject.Store(imageWidth);
		sObject.Store(imageHeight);
		sObject.Store(imageArray, imageWidth, imageHeight);
		CONTROL_WINDOW::saveNeeded = TRUE;
	}
	else
	{
		// Initialize title for iamge window.
		char *title;
		switch (action)
		{
		case OPTION_NEW:  title = "New";  break;
		case OPTION_LOAD:  title = "Load";  break;
		case OPTION_SAVE_AS:  title = "Save As";  break;
		case OPTION_DELETE:  title = "Delete";  break;
		}

		// Get image info from user.
		IMAGE_WINDOW *imageWindow = new IMAGE_WINDOW(title, this, action);
		(void *)&(*windowManager + imageWindow);
		do
		{
			UI_EVENT event;
			eventManager->Get(event);
			if (event.type == IMAGE_SELECTED)
			{
				// Get image name.
				char *testName = imageWindow->ImageName();
				imageID = imageWindow->ImageID();
				CONTROL_WINDOW::storage->ChDir(imageID == ID_BITMAP_IMAGE ? "~UI_BITMAP" : "~UI_ICON");

				// Check for valid image name.
				int actionReady = FALSE;
				char *objectName = CONTROL_WINDOW::storage->FindFirstObject(testName);
				if ((action == OPTION_NEW || action == OPTION_SAVE_AS) && objectName)
				{
					DIALOG_WINDOW dialog(title, "QUESTION", DIF_YES | DIF_NO,
						"Image %s already exists.\r\n\r\nOverwrite it?", testName);
//					if (dialog.Responce() != DIALOG_YES)					// BUG.1624
					if (dialog.Responce() == DIALOG_YES)
						actionReady = TRUE;
				}
				else if ((action == OPTION_LOAD || action == OPTION_DELETE) && !objectName)
				{
					DIALOG_WINDOW dialog(title, "ASTERISK", DIF_OK,	"%s\r\n\r\n Image not found!", testName);
					dialog.Responce();
				}
				else if (action == OPTION_DELETE)
				{
					DIALOG_WINDOW dialog("Delete", "EXCLAMATION", DIF_OK | DIF_CANCEL,
						"%s\r\n\r\nThis image will be deleted.", testName);
					if (dialog.Responce() == DIALOG_OK)
						actionReady = TRUE;
				}
				else
					actionReady = TRUE;

				// Perform the action.
				if (actionReady)
				{
					if (action == OPTION_NEW)
					{
						strcpy(imageName, testName);
						imageWidth = imageWindow->ImageWidth();
						imageHeight = imageWindow->ImageHeight();
						delete imageArray;
						delete undoArray;
						imageArray = new UCHAR[imageWidth * imageHeight];
						undoArray = new UCHAR[imageWidth * imageHeight];
						memset(imageArray, (UCHAR)WHITE, imageWidth * imageHeight);
						memset(undoArray, (UCHAR)WHITE, imageWidth * imageHeight);
		 				editBitmap->width = viewBitmap->width = imageWidth;
						editBitmap->height = viewBitmap->height = imageHeight;
						editBitmap->bitmapArray = viewBitmap->bitmapArray = imageArray;
						UIW_WINDOW::Event(S_SIZE);
						UIW_WINDOW::Event(S_REDISPLAY);
					}
					else if (action == OPTION_LOAD)
					{
						strcpy(imageName, testName);
						UI_STORAGE_OBJECT sObject(*CONTROL_WINDOW::storage, imageName, imageID, UIS_READ);
						sObject.Load(&imageWidth);
						sObject.Load(&imageHeight);
						delete imageArray;
						delete undoArray;
						imageArray = new UCHAR[imageWidth * imageHeight];
						undoArray = new UCHAR[imageWidth * imageHeight];
						sObject.Load(imageArray, imageWidth, imageHeight);
						memcpy(undoArray, imageArray, imageWidth * imageHeight);
		 				editBitmap->width = viewBitmap->width = imageWidth;
						editBitmap->height = viewBitmap->height = imageHeight;
						editBitmap->bitmapArray = viewBitmap->bitmapArray = imageArray;
						UIW_WINDOW::Event(S_SIZE);
						UIW_WINDOW::Event(S_REDISPLAY);
					}
					else if (action == OPTION_SAVE_AS)
					{
						strcpy(imageName, testName);
						ImageAction(OPTION_SAVE);
					}
					else if (action == OPTION_DELETE)
					{
						CONTROL_WINDOW::storage->DestroyObject(testName);
						CONTROL_WINDOW::saveNeeded = TRUE;
					}

					// Place image name in title.
					if (action != OPTION_DELETE)
					{
						char title[MAX_PATH];
						sprintf(title, "Image Editor - %s", imageName);
						Information(SET_TEXT, title);
					}

					// Close the image window.
					eventManager->Put(S_CLOSE);
				}
			}
			else
				UI_WINDOW_OBJECT::windowManager->Event(event);
		} while (UI_WINDOW_OBJECT::windowManager->Index(imageWindow) != -1);
	}
}

EVENT_TYPE AUTO_PROMPT::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_CREATE:
		editor = (IMAGE_EDITOR *)parent;
		// Continue to S_SIZE.
	case S_SIZE:
		// Auto position the prompt.
		relative.top = Previous()->relative.bottom + 1;
		if (FlagSet(userFlags, BMF_LT_RT_BITMAP))
			relative.left = editor->client.right - editor->client.left -
				display->cellWidth - display->TextWidth(text, screenID, font);
		UIW_PROMPT::Event(event);
		break;

	default:
  		ccode = UIW_PROMPT::Event(event);

	}

	// Return the control code.
	return (ccode);
}
