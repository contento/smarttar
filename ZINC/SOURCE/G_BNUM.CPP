//	Zinc Interface Library - G_BIGNUM.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif
#if defined(ZIL_LINKBUG)
void z_bignum_dummy(void) { }	// Bug fix for broken linkers.
#endif

NMF_FLAGS UIW_BIGNUM::rangeFlags = NMF_NO_FLAGS;

static UI_ITEM _errorTable[] =
{
#if defined(__ENGLISH__)
  { 0, NULL, "The number %s is not in the range %s.", NMI_OUT_OF_RANGE }, // BUG.2083
  { 0, NULL, "The number %s is invalid.", NMI_INVALID },
#else
  { 0, NULL, "El número %s no está en el rango %s.", NMI_OUT_OF_RANGE },
  { 0, NULL, "El número %s es inválido.", NMI_INVALID },
#endif
  { 0, NULL, NULL, NMI_OK }
};

UI_ITEM *UIW_BIGNUM::errorTable = _errorTable;
