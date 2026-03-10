//	Zinc Interface Library - G_REAL.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif
#if defined(ZIL_LINKBUG)
void z_real_dummy(void) { }	// Bug fix for broken linkers.
#endif

static UI_ITEM _errorTable[] =
{
	{ 0, NULL, "The number %s is not in the range %s.", NMI_OUT_OF_RANGE },	// BUG.2083
	{ 0, NULL, "The number %s is invalid.", NMI_INVALID },
	{ 0, NULL, NULL, NMI_OK }
};

UI_ITEM *UIW_REAL::errorTable = _errorTable;
