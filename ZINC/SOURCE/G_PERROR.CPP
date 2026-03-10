//  Zinc Interface Library - G_PERROR.CPP
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "ui_win.hpp"
#if defined(_MSC_VER)
#pragma hdrstop					// Microsoft pre-compiled header pragma.
#endif
#if defined(ZIL_LINKBUG)
void z_perror_dummy(void) { }	// Bug fix for broken linkers.
#endif

static UI_PALETTE_MAP paletteMapTable[] =
{
	{ ID_WINDOW_OBJECT, PM_ANY,
		{ ' ', attrib(WHITE, DARKGRAY), attrib(MONO_HIGH, MONO_BLACK),
		PTN_SOLID_FILL, BLACK, LIGHTGRAY, BW_BLACK, BW_WHITE, GS_WHITE, GS_GRAY } },
	{ ID_WINDOW_OBJECT, PM_ACTIVE,
		{ ' ', attrib(WHITE, RED), attrib(MONO_HIGH, MONO_BLACK),
		PTN_SOLID_FILL, BLACK, LIGHTGRAY, BW_BLACK, BW_WHITE, GS_WHITE, GS_GRAY } },
	{ ID_TITLE, PM_ACTIVE,
		{ ' ', attrib(WHITE, RED), attrib(MONO_HIGH, MONO_BLACK),
		PTN_SOLID_FILL, WHITE, RED, BW_WHITE, BW_BLACK, GS_WHITE, GS_GRAY } },

	{ ID_STRING, PM_ANY,
		{ ' ', attrib(BLACK, WHITE), attrib(MONO_BLACK, MONO_NORMAL),
		PTN_SOLID_FILL, BLACK, WHITE, BW_BLACK, BW_WHITE, GS_BLACK, GS_WHITE } },
	{ ID_TEXT, PM_ANY,
		{ ' ', attrib(BLACK, WHITE), attrib(MONO_BLACK, MONO_NORMAL),
		PTN_SOLID_FILL, BLACK, WHITE, BW_BLACK, BW_WHITE, GS_BLACK, GS_WHITE } },

	// Special palettes.
	{ ID_OUTLINE, PM_ANY,
		{ ' ', attrib(WHITE, LIGHTGRAY), attrib(MONO_HIGH, MONO_NORMAL),
		PTN_SOLID_FILL, BLACK, BLACK, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK } },
	{ ID_WHITE_SHADOW, PM_ANY,
		{ ' ', attrib(WHITE, WHITE), attrib(MONO_HIGH, MONO_HIGH),
		PTN_SOLID_FILL, WHITE, WHITE, BW_WHITE, BW_WHITE, GS_WHITE, GS_WHITE } },
	{ ID_LIGHT_SHADOW, PM_ANY,
		{ ' ', attrib(LIGHTGRAY, LIGHTGRAY), attrib(MONO_DIM, MONO_DIM),
		PTN_SOLID_FILL, LIGHTGRAY, LIGHTGRAY, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK } },
	{ ID_DARK_SHADOW, PM_ANY,
		{ ' ', attrib(DARKGRAY, LIGHTGRAY), attrib(MONO_BLACK, MONO_BLACK),
		PTN_SOLID_FILL, DARKGRAY, DARKGRAY, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK } },
	{ ID_BLACK_SHADOW, PM_ANY,
		{ ' ', attrib(BLACK, BLACK), attrib(MONO_BLACK, MONO_BLACK),
		PTN_SOLID_FILL, BLACK, BLACK, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK } },

	// End of array.
	{ ID_END, 0, { 0, 0, 0, 0, 0, 0 } }
};
UI_PALETTE_MAP *_errorPaletteMapTable = paletteMapTable;

static char *_errorPrompts[] =
{
#if defined(__ENGLISH__)
  "Message",        // Title, before "Error"
  "&Move",          // option 1 (move)
  "&Close\tAlt+F4"  // option 2 (close)
#else
  "Aviso",          // Title
	"&Mover",			    // option 1 (move)
	"&Cerrar\tAlt+F4"	// option 2 (close)
#endif
};

char **UI_ERROR_SYSTEM::errorPrompts = _errorPrompts;

static char *_statusPrompts[] =
{
#if defined(__ENGLISH__)
	"~OK",
	"~Cancel",
#else
	"~Aceptar",
	"~Cancelar",
#endif
};

char **UI_ERROR_SYSTEM::statusPrompts = _statusPrompts;
