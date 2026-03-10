//	Zinc Interface Library - Z_DEVICE.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_evt.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif

UI_DEVICE::UI_DEVICE(DEVICE_TYPE _type, DEVICE_STATE _state) :
	installed(FALSE), type(_type), state(_state)
{
#if defined(ZIL_LINKBUG)
	extern void z_device_dummy(void);		// Bug fix for broken linkers.
	z_device_dummy();
#endif
}

UI_DEVICE::~UI_DEVICE(void)
{
}

int UI_DEVICE::CompareDevices(void *device1, void *device2)
{
	return ((int)(((UI_DEVICE *)device1)->type - ((UI_DEVICE *)device2)->type));
}

