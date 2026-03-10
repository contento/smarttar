//	ZINCAPP.CPP (ZINCAPP) - This file contains the main program loop.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#if defined(_MSC_VER)
#include <new.h>
#endif
#include <ui_win.hpp>
#include "zincapp.hpp"
#define USE_HELP_CONTEXTS
#include "support.hpp"

#include <stdlib.h>
#include <stdio.h>

#if defined(__BCPLUSPLUS__) || defined(__TCPLUSPLUS__)
extern unsigned _stklen = 8192;
#endif

#if defined(ZIL_MSDOS)
#if defined(_MSC_VER)
int NewHandler(size_t size)
{
	printf("\nMemory is all gone.\n");
	return(0);
}
#else
void NewHandler(void)
{
	printf("\nMemory is all gone.\n");
	exit(0);
}
#endif
#endif

#if defined(ZIL_MSDOS)
main(int argc, char *argv[])
{
	UI_PATH searchPath(argv[0], TRUE);

	// Create the DOS display.
	UI_DISPLAY *display = new UI_GRAPHICS_DISPLAY;
	if (!display->installed)
	{
		delete display;
		display = new UI_TEXT_DISPLAY;
	}
#elif defined(ZIL_MSWINDOWS)
int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR, int nCmdShow)
{
	UI_PATH searchPath;

	// Create the Windows display.
	UI_DISPLAY *display = new UI_MSWINDOWS_DISPLAY(hInstance, hPrevInstance, nCmdShow);
#elif defined(ZIL_OS2)
main(int argc, char *argv[])
{
	UI_PATH searchPath(argv[0], TRUE);

	// Create the OS/2 display.
	UI_DISPLAY *display = new UI_OS2_DISPLAY;
#elif defined(ZIL_MOTIF)
main(int argc, char **argv)
{
	UI_PATH searchPath(argv[0], TRUE);

	// Create the Motif display.
	UI_DISPLAY *display = new UI_MOTIF_DISPLAY(&argc, argv, "ZincApp");
#endif

	// Make sure the display installed correctly.
	if (!display || !display->installed)
	{
		delete display;
		return (0);
	}

	// Create the event manager and add devices.
	UI_EVENT_MANAGER *eventManager = new UI_EVENT_MANAGER(display);
	*eventManager
		+ new UID_KEYBOARD
		+ new UID_MOUSE
		+ new UID_CURSOR;

	// Initialize the zinc application window manager and add the control window.
	ZINCAPP_WINDOW_MANAGER *windowManager = new ZINCAPP_WINDOW_MANAGER(display, eventManager);
	UIW_WINDOW *window = new CONTROL_WINDOW;
	*windowManager + window;

	// This line assigns the exit function to be called before the control
	// window is closed.  It MUST be after the window is added to windowManager.
	windowManager->screenID = window->screenID;

	// Initialize the help and error systems.
	UI_WINDOW_OBJECT::errorSystem = new UI_ERROR_SYSTEM;
	UI_WINDOW_OBJECT::helpSystem = new UI_HELP_SYSTEM("support.dat",
		windowManager, HELP_GENERAL);

	// Set up the support data storage unit.
	UI_STORAGE::searchPath = &searchPath;
	UI_WINDOW_OBJECT::defaultStorage = new UI_STORAGE("support.dat", UIS_READ);

#if defined(ZIL_MSDOS)
#if defined(_MSC_VER)
	_set_new_handler(NewHandler);
#else
	extern void (*_new_handler)();
	_new_handler = NewHandler;
#endif
#endif

	// Wait for user response.
	UI_EVENT event;
	EVENT_TYPE ccode;
	do
	{
		// Get input from the user.
		eventManager->Get(event);

		// Check for a screen reset message.
		if (event.type == S_RESET_DISPLAY)
		{
#if defined(ZIL_MSDOS)
			event.data = NULL;

			// Tell the managers we changed the display.
			windowManager->Event(event);
			eventManager->Event(event);

			delete display;
			if (event.rawCode == TDM_NONE)
			{
				display = new UI_GRAPHICS_DISPLAY;
				if (!display->installed)
				{
					delete display;
					display = new UI_TEXT_DISPLAY;
				}
			}
			else
				display = new UI_TEXT_DISPLAY(event.rawCode);

			// Tell the managers we changed the display.
			event.data = display;
			eventManager->Event(event);
			ccode = windowManager->Event(event);
			windowManager->screenID = window->screenID;
#endif
		}
		else
			ccode = windowManager->Event(event);
	} while (ccode != L_EXIT && ccode != S_NO_OBJECT);

	// Clean up.
    delete UI_WINDOW_OBJECT::defaultStorage;
	delete UI_WINDOW_OBJECT::helpSystem;
	delete UI_WINDOW_OBJECT::errorSystem;
	delete windowManager;
	delete eventManager;
	delete display;

	return (0);
}
