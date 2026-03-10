//	Zinc Interface Library - G_WIN2.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#define USE_RAW_KEYS
#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif
#if defined(ZIL_LINKBUG)
void z_win2_dummy(void) { }		// Bug fix for broken linkers.
#endif

// This is a good default stack for Zinc programs.  It may need to be
// different for some programs and can be overridden in the users program.
#if defined(__ZTC__) && !defined(DOS386)
unsigned _stack = 20480;
#endif

int UI_WINDOW_OBJECT::repeatRate = 6;
int UI_WINDOW_OBJECT::doubleClickRate = 50;
WOS_STATUS UI_WINDOW_OBJECT::defaultStatus = WOS_NO_STATUS;
UI_DISPLAY *UI_WINDOW_OBJECT::display = NULL;
UI_EVENT_MANAGER *UI_WINDOW_OBJECT::eventManager = NULL;
UI_WINDOW_MANAGER *UI_WINDOW_OBJECT::windowManager = NULL;
UI_STORAGE *UI_WINDOW_OBJECT::defaultStorage = NULL;
UI_ERROR_SYSTEM *UI_WINDOW_OBJECT::errorSystem = NULL;
UI_HELP_SYSTEM *UI_WINDOW_OBJECT::helpSystem = NULL;
