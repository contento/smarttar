//	Zinc Application Framework - G_SYS.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif
#if defined(ZIL_LINKBUG)
void z_sys_dummy(void) { }		// Bug fix for broken linkers.
#endif

#if defined(ZIL_MSDOS)
static char *_sysPrompts[] =
{
	"&Restore",			// option 1 (restore)
	"&Move",			// option 2 (move)
	"&Size",			// option 3 (size)
	"Mi&nimize",			// option 4 (minimize)
	"Ma&ximize",			// option 5 (maximize)
	"",				// option 6
	"&Close"			// option 7 (close)
};

char **UIW_SYSTEM_BUTTON::sysPrompts = _sysPrompts;
#else
char **UIW_SYSTEM_BUTTON::sysPrompts = NULL;
#endif

