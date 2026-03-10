//	Zinc Interface Library Designer - Z_DESIGN.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define USE_HELP_CONTEXTS
#include <ctype.h>
#include <string.h>
#include "ui_dsn.hpp"
#include "p_design.hpp"
#if defined(DO_MEM)
#include "memwin.hpp"
#endif
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// Increase stack size for the Borland compiler.
#if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
extern unsigned _stklen = 8192;
#endif

static EVENT_TYPE ExitFunction(UI_DISPLAY *, UI_EVENT_MANAGER *eventManager, UI_WINDOW_MANAGER *windowManager)
{
	if (CONTROL_WINDOW::testMode)
		return L_EXIT;
	else if (CONTROL_WINDOW::saveNeeded)
	{
		char path[MAX_PATH];
		char file[MAX_PATH];
		CONTROL_WINDOW::storage->StorageName(path);
		UI_STORAGE::StripFullPath(path, NULL, file, NULL);
		DIALOG_WINDOW dialog("Exit", "QUESTION", DIF_YES | DIF_NO | DIF_CANCEL, "Save file %s before exiting?", path);
		EVENT_TYPE responce = dialog.Responce();
		UI_WINDOW_OBJECT *controlWindow = windowManager->Get("CONTROL_WINDOW");
		if (responce == DIALOG_YES && controlWindow)
			controlWindow->Event(UI_EVENT(FILE_SAVE, 0));
		if (responce == DIALOG_YES || responce == DIALOG_NO)
			eventManager->Put(UI_EVENT(L_EXIT, 0));
	}
	else
	{
		DIALOG_WINDOW dialog("Exit", "ASTERISK", DIF_OK | DIF_CANCEL, "This will close Zinc Designer.");
		if (dialog.Responce() == DIALOG_OK)
			return L_EXIT;
	}
	return 0;
}

#if defined(ZIL_MSWINDOWS)
int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
//	UI_PATH searchPath;		  												// BUG.1583

	char pathName[MAXPATHLEN];
	GetModuleFileName(hInstance, pathName, MAXPATHLEN);
	UI_PATH searchPath(pathName, TRUE);

	char buffer[128];
#if defined(__BCPLUSPLUS__) && defined(WIN32)
	while (*lpszCmdLine && *lpszCmdLine != ' ' && *lpszCmdLine != '/')
		lpszCmdLine++;
	while (*lpszCmdLine && *lpszCmdLine == ' ')
		lpszCmdLine++;
#endif
	strcpy(buffer, lpszCmdLine);
	char *fullPath = strtok(buffer, " ");
	while (fullPath && !isalpha(*fullPath))
		fullPath = strtok(NULL, " ");
	if (!fullPath)
		fullPath = "untitled";

	UI_DISPLAY *display = new UI_MSWINDOWS_DISPLAY(hInstance, hPrevInstance, nCmdShow);
#else
main(int argc, char *argv[])
{
	UI_DISPLAY *display = NULL;
#if defined(ZIL_MOTIF)
	display = new UI_MOTIF_DISPLAY(&argc, argv);
#endif
	// Search command line arguments (design [file[~resource]] [/t] [+t]).
	UI_PATH searchPath(argv[0]);
	int forceText = FALSE;
	char *fullPath = "untitled";
	for (int argnum = 1; argnum < argc; argnum++)
	{
		if (isalpha(*argv[argnum]))
			fullPath = argv[argnum];
		else if (!ui_stricmp("/t", argv[argnum]) || !ui_stricmp("+t", argv[argnum]))
			forceText = TRUE;
	}

	// Initialize the display.
#if defined(ZIL_MSDOS)
	if (!forceText)
	{
		display = new UI_GRAPHICS_DISPLAY;
		if (!display->installed)
		{
			delete display;
			display = NULL;
		}
	}
	if (!display)
		display = new UI_TEXT_DISPLAY;
#elif defined(ZIL_OS2)
	display = new UI_OS2_DISPLAY;
#endif
#endif

	// Initialize the event manager.
	UI_EVENT_MANAGER *eventManager = new UI_EVENT_MANAGER(display);
	(void *)&(*eventManager
		+ new UID_KEYBOARD
		+ new UID_MOUSE
		+ new UID_CURSOR);

	// Initialize the windowManager.
	UI_WINDOW_MANAGER *windowManager = new UI_WINDOW_MANAGER(display, eventManager, ExitFunction);

#if defined(DO_MEM)
	// Add the memory class to the window and event managers.
	MEMORY_WINDOW *memWindow = new MEMORY_WINDOW(0, 0);
	*windowManager + memWindow->window;
	*eventManager + memWindow->device;
#endif

	// Initialize designer storage file.
	UI_STORAGE::searchPath = &searchPath;
	UI_WINDOW_OBJECT::defaultStorage = new UI_STORAGE("p_design.znc", UIS_READ);
	if (UI_WINDOW_OBJECT::defaultStorage->storageError)
	{
		// Return on error.
		DIALOG_WINDOW dialog("Error", NULL, DIF_OK, "Could not open data file 'p_design.znc.'");
		dialog.Responce();
		return 1;
	}

	// Initialize the static information.
	UID_KEYBOARD::breakHandlerSet = L_EXIT_FUNCTION;
	UI_WINDOW_OBJECT::errorSystem = new UI_ERROR_SYSTEM;
	UI_WINDOW_OBJECT::helpSystem = new UI_HELP_SYSTEM("p_design.znc", windowManager, ABOUT_DESIGN_HELP);
	ReadConfiguration();

	// Format file and resource names.
	char path[MAX_PATH];
	char fileName[MAX_PATH];
	char resourceName[MAX_PATH];
	UI_STORAGE::StripFullPath(fullPath, path, fileName, resourceName);
	if (*path)
	{
		char pathEnd = path[ui_strlen(path) - 1];
		if (pathEnd != ZIL_DIRSEP && pathEnd != ':')
			strcat(path, ZIL_DIRSEPSTR);
	}
	if (!strchr(fileName, '.'))
		UI_STORAGE::ChangeExtension(fileName, ".dat");
	strcat(path, fileName);
	ui_strupr(resourceName);

	// Initialize the control window.
	CONTROL_WINDOW *controlWindow = new CONTROL_WINDOW(path);
	(void *)&(*windowManager + controlWindow);
	windowManager->screenID = controlWindow->screenID;

	// Process events.
	UI_EVENT event;
	EVENT_TYPE ccode;
	do
	{
		eventManager->Get(event);
		if (event.type >= D_DESIGNER_FIRST && event.type <= D_DESIGNER_LAST)
			ccode = controlWindow->Event(event);
		else
			ccode = windowManager->Event(event);
	} while (ccode != L_EXIT);

	// Clean up.
	delete UI_WINDOW_OBJECT::helpSystem;
	delete UI_WINDOW_OBJECT::errorSystem;
#if defined(DO_MEM)
	*eventManager - memWindow->device;
	*windowManager - memWindow->window;
	delete memWindow;
#endif
	delete windowManager;
	delete eventManager;
	delete display;
	return 0;
}
