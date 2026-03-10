//	Zinc Interface Library - Z_APP.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if !defined(NO_ZIL_APP)
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

void z_app_dummy(void) { }		// Bug fix for broken linkers.

#if defined(ZIL_MSWINDOWS)
extern "C"
int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	if (hPrevInstance)
		return (1);
	UI_APPLICATION *application = new UI_APPLICATION(hInstance, hPrevInstance, lpszCmdLine, nCmdShow);
#else
int main(int argc, char **argv)
{
	UI_APPLICATION *application = new UI_APPLICATION(argc, argv);
#endif

	// Call the application program.
	int ccode = application->Main();

	// Restore the system.
	delete application;

	return (ccode);
}

#if defined(ZIL_MSWINDOWS)
UI_APPLICATION::UI_APPLICATION(HANDLE _hInstance, HANDLE _hPrevInstance,
	LPSTR _lpszCmdLine, int _nCmdShow) :
	hInstance(_hInstance), hPrevInstance(_hPrevInstance),
	lpszCmdLine(_lpszCmdLine), nCmdShow(_nCmdShow)
{
	// Set up the display search path.
	char pathName[MAXPATHLEN];						// BUG.1583
	GetModuleFileName(_hInstance, pathName, MAXPATHLEN);
	searchPath = new UI_PATH(pathName, TRUE);

	// Create the display.
	display = new UI_MSWINDOWS_DISPLAY(hInstance, hPrevInstance, nCmdShow);

#else
UI_APPLICATION::UI_APPLICATION(int _argc, char **_argv) :
	argc(_argc), argv(_argv)
{
	// Set up the display search path.
	searchPath = new UI_PATH(argv[0], TRUE);

#	if defined(ZIL_MSDOS)
	// ----- Check for the specification of a text display. -----
	display = NULL;
#if defined(TEXT)
	display = new UI_TEXT_DISPLAY;
#else
	if (argc > 1)
	{
		for (int i = 0; textSwitches[i]; i++)
			if (ui_stricmp(argv[1], textSwitches[i]) == 0)
			{
				for (int j = 2; j < argc; j++)
					argv[j-1] = argv[j];
				argc--;
				display = new UI_TEXT_DISPLAY;
				break;
			}
	}
#endif

	// ----- Code for the Zinc GFX display -----
#if defined(GFX)
//	UI_GRAPHICS_DISPLAY::searchPath = searchPath;
	if (!display)
		display = new UI_GRAPHICS_DISPLAY;
#elif defined(BGI)
	// ----- Code for the Borland BGI display -----
	UI_BGI_DISPLAY::searchPath = searchPath;
	if (!display)
		display = new UI_BGI_DISPLAY;
#elif defined(FG)
	// ----- Code for the Zortech Flashgraphics display -----
//	UI_FG_DISPLAY::searchPath = searchPath;
	if (!display)
		display = new UI_FG_DISPLAY;
#elif defined(MSC)
	// ----- Code for the Microsoft graphics display -----
	UI_MSC_DISPLAY::searchPath = searchPath;
	if (!display)
		display = new UI_MSC_DISPLAY;
#elif defined(WCC)
	// ----- Code for the Watcom graphics display -----
	UI_WCC_DISPLAY::searchPath = searchPath;
	if (!display)
		display = new UI_WCC_DISPLAY;
#elif defined(TEXT)
	if (!display)
		display = new UI_TEXT_DISPLAY;
#endif

	// ----- Code for generic text display -----
	if (!display->installed)
	{
		delete display;
		display = new UI_TEXT_DISPLAY;
	}

#	elif defined(ZIL_OS2)
	// ----- Code for the OS/2 graphics display -----
	display = new UI_OS2_DISPLAY;

#	elif defined(ZIL_MOTIF)
	// ----- Code for the Motif graphics display -----
	display = new UI_MOTIF_DISPLAY(&argc, argv);

#	endif
#endif

	// Create the event manager and input devices.
	eventManager = new UI_EVENT_MANAGER(display);
	(void *)&(*eventManager
		+ new UID_KEYBOARD
#if defined(ZIL_PENDOS)
		+ new UID_PENDOS
#else
		+ new UID_MOUSE
#endif
		+ new UID_CURSOR);

	// Create the window manager.
	windowManager = new UI_WINDOW_MANAGER(display, eventManager);
}

UI_APPLICATION::~UI_APPLICATION(void)
{
	if (windowManager)
		delete windowManager;
	if (eventManager)
		delete eventManager;
	if (display)
		delete display;
	if (searchPath)
		delete searchPath;
}

EVENT_TYPE UI_APPLICATION::Control(void)
{
	// Wait for user response.
	UI_EVENT event;
	EVENT_TYPE ccode;
	do
	{
		eventManager->Get(event, Q_NORMAL);
		ccode = windowManager->Event(event);
	} while (ccode != L_EXIT && ccode != S_NO_OBJECT);
	return (ccode);
}
#endif

void UI_APPLICATION::LinkMain(void)
{
}

