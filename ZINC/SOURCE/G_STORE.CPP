//	Zinc Interface Library - G_STORE.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_gen.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif
#if defined(ZIL_LINKBUG)
void z_store_dummy(void) { }	// Bug fix for broken linkers.
#endif

#define DEFAULT_CACHE_SIZE		32
int UI_STORAGE::cacheSize = DEFAULT_CACHE_SIZE;
UI_PATH *UI_STORAGE::searchPath = NULL;
