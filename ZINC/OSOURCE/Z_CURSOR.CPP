//	Zinc Interface Library - Z_CURSOR.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_evt.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

// ----- UID_CURSOR ---------------------------------------------------------

// WATCOM has a compiler bug if the constructor and destructor don't
// have any other functions in the file (Poll, Event). So definition
// of constructor & destructor are in d_cursor.
#if !defined(__WATCOMC__)

UID_CURSOR::UID_CURSOR(DEVICE_STATE _state, DEVICE_IMAGE _image) :
	UI_DEVICE(E_CURSOR, _state), image(_image)
{
#if defined(ZIL_LINKBUG)
	extern void z_device_dummy(void);		// Bug fix for broken linkers.
	z_device_dummy();
#endif

	position.column = position.line = 0;
	offset.column = offset.line = 0;
	installed = TRUE;
}

UID_CURSOR::~UID_CURSOR(void)
{
	// No cursor information needs to be destroyed.
}

#endif

