//	Zinc Interface Library - G_TIME.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_gen.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif
#if defined(ZIL_LINKBUG)
void z_time_dummy(void) { }		// Bug fix for broken linkers.
#endif

static char _am[] = "a.m.";															// BUG.Internal
static char _pm[] = "p.m.";
char *UI_TIME::amPtr = _am;
char *UI_TIME::pmPtr = _pm;
