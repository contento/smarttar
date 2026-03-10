//	Zinc Interface Library - D_OBJECT.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include <stddef.h>
#include "ui_dsn.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UIW_DERIVE_OBJECT ---------------------------------------------------------

EVENT_TYPE UIW_DERIVE_OBJECT::Event(const UI_EVENT &event)
{
	return UI_WINDOW_OBJECT::Event(event);
}
