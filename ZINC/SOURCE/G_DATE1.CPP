//	Zinc Interface Library - G_DATE1.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif
#if defined(ZIL_LINKBUG)
void z_date1_dummy(void) { }	// Bug fix for broken linkers.
#endif

DTF_FLAGS UIW_DATE::rangeFlags = DTF_US_FORMAT;

// Set up the date error messages.
static UI_ITEM _errorTable[] =
{
#if defined(__ENGLISH__)
	{ 0, NULL, "The date %s is in an invalid format.", DTI_INVALID },
	{ 0, NULL, "The date %s has an ambiguous month name.", DTI_AMBIGUOUS },
	{ 0, NULL, "The date %s has an invalid month name.", DTI_INVALID_NAME },
	{ 0, NULL, "A date value must be entered.", DTI_VALUE_MISSING },
  { 0, NULL, "The date %s must be in the range %s.", DTI_OUT_OF_RANGE },  // BUG.2083
#else
	{ 0, NULL, "La fecha %s está en un formato inválido.", DTI_INVALID },
	{ 0, NULL, "La fecha %s tiene un nombre de mes ambiguo.", DTI_AMBIGUOUS },
	{ 0, NULL, "La fecha %s tiene un nombre de mes inválido.", DTI_INVALID_NAME },
	{ 0, NULL, "Una valor de fecha debe ser entrado.", DTI_VALUE_MISSING },
	{ 0, NULL, "La fecha %s debe estar en el rango %s.", DTI_OUT_OF_RANGE },
#endif
	{ 0, NULL, NULL, DTI_OK }
};

UI_ITEM *UIW_DATE::errorTable = _errorTable;
