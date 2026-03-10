//	Zinc Interface Library - G_TIME1.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif
#if defined(ZIL_LINKBUG)
void z_time1_dummy(void) { }	// Bug fix for broken linkers.
#endif

TMF_FLAGS UIW_TIME::rangeFlags = TMF_SECONDS | TMF_HUNDREDTHS;

static UI_ITEM _errorTable[] =
{
#if defined(__ENGLISH__)
	{ 0, NULL, "The time %s is in an invalid format.", TMI_INVALID },
	{ 0, NULL, "A time value must be entered.", TMI_VALUE_MISSING },
  { 0, NULL, "The time %s is not in the range %s.", TMI_OUT_OF_RANGE }, // BUG.2083
#else
  { 0, NULL, "La hora %s está en un formato inválido.", TMI_INVALID },
	{ 0, NULL, "Un valor de hora debe ser entrado.", TMI_VALUE_MISSING },
  { 0, NULL, "La hora %s no está en el rango %s.", TMI_OUT_OF_RANGE },
#endif
	{ 0, NULL, NULL, TMI_OK }
};

UI_ITEM *UIW_TIME::errorTable = _errorTable;
