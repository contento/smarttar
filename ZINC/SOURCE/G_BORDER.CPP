//	Zinc Interface Library - G_BORDER.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif
#if defined(ZIL_LINKBUG)
void z_border_dummy(void) { }	// Bug fix for broken linkers.
#endif

int UIW_BORDER::width = 4;
