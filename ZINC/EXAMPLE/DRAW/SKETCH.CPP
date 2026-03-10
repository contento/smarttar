//	SKETCH.CPP (DRAW) - Sketch pad example program.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
#include <mem.h>
#endif
#include <string.h>
#include <ui_win.hpp>
#include "sketch.hpp"

static UCHAR colorArray[16] =
{
	BLACK, WHITE, DARKGRAY, LIGHTGRAY, RED, LIGHTRED, BROWN, YELLOW, 
	GREEN, LIGHTGREEN, CYAN, LIGHTCYAN, BLUE, LIGHTBLUE, MAGENTA, LIGHTMAGENTA
};

SKETCH_PAD::SKETCH_PAD(int left, int top) :
//	UIW_WINDOW(left, top, 62, 14), bitmapArray(NULL), bitmapWidth(32), bitmapHeight(32)	// BUG.2237
	UIW_WINDOW(left, top, 62, 15), bitmapArray(NULL), bitmapWidth(32), bitmapHeight(32)
{
	// Add objects to the window.
	*this
		+ new UIW_BORDER
		+ new UIW_MAXIMIZE_BUTTON
		+ new UIW_MINIMIZE_BUTTON
		+ new UIW_SYSTEM_BUTTON(SYF_GENERIC)
		+ new UIW_TITLE("Sketch Pad")
		+ (colorBitmap = new UIW_BITMAP(4, 1, NULL, ID_COLOR_BITMAP, 24, 24))
		+ new UIW_PROMPT(50, 5, "Screen")
		+ (screenBitmap = new UIW_BITMAP(50, 6, NULL, ID_SCREEN_BITMAP, 40, 10))
		+ new UIW_PROMPT(50, 7, "left/right")
		+ (leftColor = new UIW_BITMAP(50, 8, NULL, ID_LEFT_BITMAP, 19, 10))
		+ (rightColor = new UIW_BITMAP(53, 8, NULL, ID_RIGHT_BITMAP, 19, 10))
//		+ new UIW_BUTTON(7, 12, 10, "&Clear", BTF_NO_TOGGLE | BTF_AUTO_SIZE |	// BUG.2237
		+ new UIW_BUTTON(7, 12, 10, "&Clear", BTF_NO_TOGGLE | BTF_AUTO_SIZE |
			BTF_SEND_MESSAGE, WOF_JUSTIFY_CENTER, NULL, CLEAR_BITMAP)
//		+ new UIW_BUTTON(19, 12, 10, "&Save", BTF_NO_TOGGLE | BTF_AUTO_SIZE |	// BUG.2237
		+ new UIW_BUTTON(19, 12, 10, "&Save", BTF_NO_TOGGLE | BTF_AUTO_SIZE |
			BTF_SEND_MESSAGE, WOF_JUSTIFY_CENTER, NULL, SAVE_BITMAP)
//		+ new UIW_BUTTON(31, 12, 10, "&Restore", BTF_NO_TOGGLE | BTF_AUTO_SIZE |	// BUG.2237
		+ new UIW_BUTTON(31, 12, 10, "&Restore", BTF_NO_TOGGLE | BTF_AUTO_SIZE |
			BTF_SEND_MESSAGE, WOF_JUSTIFY_CENTER, NULL, LOAD_BITMAP)
//		+ new UIW_BUTTON(43, 12, 10, "E&xit", BTF_NO_TOGGLE | BTF_AUTO_SIZE |	// BUG.2237
		+ new UIW_BUTTON(43, 12, 10, "E&xit", BTF_NO_TOGGLE | BTF_AUTO_SIZE |
			BTF_SEND_MESSAGE, WOF_JUSTIFY_CENTER, NULL, L_EXIT);

	// Initialize the color palette.
	int colorWidth = 2;
	int colorHeight = 8;
	colorBitmap->Information(SET_BITMAP_WIDTH, &colorWidth);
	colorBitmap->Information(SET_BITMAP_HEIGHT, &colorHeight);
	colorBitmap->Information(SET_BITMAP_ARRAY, colorArray);

	colorBitmap->leftObject = leftColor;
	colorBitmap->rightObject = rightColor;
	screenBitmap->leftObject = leftColor;
	screenBitmap->rightObject = rightColor;

	// Load initially blank bitmap.
	LoadImage("bitmap.dat", "SAMPLE");
}

EVENT_TYPE SKETCH_PAD::Event(const UI_EVENT &event)
{
	// Switch on the type of event.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case CLEAR_BITMAP:
		LoadImage(NULL, NULL);
		Event(UI_EVENT(S_REDISPLAY, 0));
		break;

	case SAVE_BITMAP:
		SaveImage("bitmap.dat", "SAMPLE");
		break;

	case LOAD_BITMAP:
		LoadImage("bitmap.dat", "SAMPLE");
		Event(UI_EVENT(S_REDISPLAY, 0));
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}
	return ccode;
}

void SKETCH_PAD::LoadImage(char *storageName, char *imageName)
{
	if (bitmapArray)
	{
		delete bitmapArray;
		*this - editBitmap;
		delete editBitmap;
		*this - viewBitmap;
		delete viewBitmap;
		bitmapArray = NULL;
	}

	// Load in the image from disk.
	if (storageName && imageName)
	{
		// Open the storage file.
		char pathName[128], fileName[32];
		UI_STORAGE::StripFullPath(storageName, pathName, fileName);
		UI_STORAGE::AppendFullPath(pathName, pathName, fileName);
		UI_STORAGE::ChangeExtension(pathName, ".dat");
		UI_STORAGE *storage = new UI_STORAGE(pathName, UIS_READ);
		if (!storage->storageError)
		{
			// Create the storage object and load bitmap.
			storage->ChDir("~UI_BITMAP");
			UI_STORAGE_OBJECT sObject(*storage, imageName, ID_BITMAP_IMAGE, UIS_READ);
			if (!sObject.objectError)
			{
				USHORT value;
				sObject.Load(&value); bitmapWidth = value;
				sObject.Load(&value); bitmapHeight = value;
				bitmapArray = new UCHAR[bitmapWidth * bitmapHeight];
				sObject.Load(bitmapArray, bitmapWidth, bitmapHeight);
			}
		}
		delete storage;
	}

	// Create a blank bitmap.
	if (!bitmapArray)
	{
		bitmapArray = new UCHAR[bitmapWidth * bitmapHeight];
		memset(bitmapArray, 15, bitmapWidth * bitmapHeight);
	}

	// Create the bitmaps.
	int pixelSize = Min(192 / bitmapWidth, 192 / bitmapHeight);
	editBitmap = new UIW_BITMAP(17, 1, NULL, ID_EDIT_BITMAP, pixelSize, pixelSize);
	viewBitmap = new UIW_BITMAP(50, 1, NULL, ID_VIEW_BITMAP, 1, 1);
	editBitmap->Information(SET_BITMAP_WIDTH, &bitmapWidth);
	editBitmap->Information(SET_BITMAP_HEIGHT, &bitmapHeight);
	editBitmap->Information(SET_BITMAP_ARRAY, bitmapArray);
	viewBitmap->Information(SET_BITMAP_WIDTH, &bitmapWidth);
	viewBitmap->Information(SET_BITMAP_HEIGHT, &bitmapHeight);
	viewBitmap->Information(SET_BITMAP_ARRAY, bitmapArray);
	editBitmap->viewObject = viewBitmap;
	viewBitmap->editObject = editBitmap;
	viewBitmap->screenObject = screenBitmap;
	*this
		+ editBitmap
		+ viewBitmap;
}

void SKETCH_PAD::SaveImage(char *storageName, char *imageName)
{
	// Load in the image from disk.
	if (storageName && imageName)
	{
		// Open the storage file.
		char pathName[128], fileName[32];
		UI_STORAGE::StripFullPath(storageName, pathName, fileName);
		UI_STORAGE::AppendFullPath(pathName, pathName, fileName);
		UI_STORAGE::ChangeExtension(pathName, ".dat");
		UI_STORAGE *storage = new UI_STORAGE(pathName, UIS_READWRITE | UIS_CREATE);
		if (!storage->storageError)
		{
			// Create the storage object and load bitmap.
			storage->MkDir("~UI_BITMAP");
			storage->ChDir("~UI_BITMAP");

			UI_STORAGE_OBJECT sObject(*storage, imageName, ID_BITMAP_IMAGE,
				UIS_READWRITE | UIS_CREATE);
			if (!sObject.objectError)
			{
				sObject.Store((USHORT)bitmapWidth);
				sObject.Store((USHORT)bitmapHeight);
				sObject.Store(bitmapArray, bitmapWidth, bitmapHeight);

				// Write the file.
				storage->Save();
			}
		}
		delete storage;
	}
}

