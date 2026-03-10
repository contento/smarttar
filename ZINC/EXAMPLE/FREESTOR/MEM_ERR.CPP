//	MEM_ERR.CPP (FREESTOR) - Demonstrate the use of the Free Store Exception Handler.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#if defined(_MSC_VER)
#include <new.h>
#endif
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <ui_win.hpp>
#include "mem_err.hpp"

#if defined(_MSC_VER)
#define getch _getch
int NewHandler(size_t size)
{
	MEMORY_ALLOCATION_ERROR_SYSTEM::FreeStoreException();
	return (0);
}
#else
void NewHandler(void)
{
	MEMORY_ALLOCATION_ERROR_SYSTEM::FreeStoreException();
}
#endif

// Static variable definitions.
char * MEMORY_ALLOCATION_ERROR_SYSTEM::errorMem;
UIW_WINDOW *MEMORY_ALLOCATION_ERROR_SYSTEM::errorWindow = NULL;

UI_DISPLAY *MEMORY_ALLOCATION_ERROR_SYSTEM::display;
UI_EVENT_MANAGER *MEMORY_ALLOCATION_ERROR_SYSTEM::eventManager;
UI_WINDOW_MANAGER *MEMORY_ALLOCATION_ERROR_SYSTEM::windowManager;

static UI_PALETTE_MAP _memErrorPalette[] =
{
	{ ID_WINDOW_OBJECT, PM_ANY,
		{ ' ', attrib(BLACK, RED), attrib(MONO_BLACK, MONO_NORMAL),
		PTN_SOLID_FILL, BLACK, RED, BW_BLACK, BW_WHITE, GS_BLACK, GS_WHITE } },
	{ ID_END, 0, { 0, 0, 0, 0, 0, 0 } }
};

MEMORY_ALLOCATION_ERROR_SYSTEM::MEMORY_ALLOCATION_ERROR_SYSTEM(UI_DISPLAY *_display,
	UI_EVENT_MANAGER *_eventManager, UI_WINDOW_MANAGER *_windowManager)
{
	installed = FALSE;
	if (!errorWindow)
	{
		// Hold on to these for clean-up.
		display = _display;
		eventManager = _eventManager;
		windowManager = _windowManager;

		// Set up the errorWindow here just in case a
		//  Free store exception is encountered.
		int x = ((display->columns / display->cellWidth) - 50) / 2;
		errorWindow = new UIW_WINDOW(x, 1, 50, 8, WOF_NO_FLAGS,
			WOAF_MODAL | WOAF_LOCKED);
		UIW_SYSTEM_BUTTON *sysButton = new UIW_SYSTEM_BUTTON;
		if (display->isText)
			sysButton->paletteMapTable = _memErrorPalette;
		UIW_TITLE *title = new UIW_TITLE("Out of Memory",WOF_JUSTIFY_CENTER);
		title->paletteMapTable = _memErrorPalette;
		*errorWindow
			+ new UIW_BORDER
			+ sysButton
			+ title
			+ new UIW_TEXT(0, 0, 1, 1,
				"The system is out of memory.  Press any key to exit to DOS.",
				256, WNF_NO_FLAGS, WOF_NON_FIELD_REGION | WOF_NON_SELECTABLE);
   	
		// Request some memory that will be required to add the error
		// to the _windowManager.  4000 is an arbitrary value.
		errorMem = new char[4000];

		if (errorMem)
		{
			// Reset the free store exception handler to the handler in this file.
#if defined(__ZTC__)
			extern void (*_new_handler)();
			_new_handler = NewHandler;
#endif
#if defined(_MSC_VER)
			_set_new_handler(NewHandler);
#endif
#if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
			extern void (*_new_handler)();
			_new_handler = NewHandler;
#endif
			installed = TRUE;
		}
	}
}

MEMORY_ALLOCATION_ERROR_SYSTEM::~MEMORY_ALLOCATION_ERROR_SYSTEM()
{
	delete errorMem;
	delete errorWindow;
}

// This is the actual FreeStoreException handler.
void MEMORY_ALLOCATION_ERROR_SYSTEM::FreeStoreException(void)
{
	// Free up the space needed to be able to add the error window.
	delete errorMem;

	// add the error window.
	*windowManager
		+ errorWindow;

	// Wait for the keypress to initiate the exit sequence.
	getch();

	// Emergency clean up and exit.
	delete windowManager;
	delete eventManager;
	delete display;

	exit(0);
}
