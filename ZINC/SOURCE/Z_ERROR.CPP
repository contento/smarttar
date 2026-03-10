//	Zinc Interface Library - Z_ERROR.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UI_ERROR_SYSTEM ----------------------------------------------------

UI_ERROR_SYSTEM::UI_ERROR_SYSTEM(void)
{
#if defined(ZIL_LINKBUG)
	extern void z_perror_dummy(void);		// Bug fix for broken linkers.
	z_perror_dummy();
#endif
}

UI_ERROR_SYSTEM::~UI_ERROR_SYSTEM(void)
{
}

