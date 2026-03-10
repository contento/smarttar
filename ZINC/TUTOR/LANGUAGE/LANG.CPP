//	LANG.CPP (LANG) - Country code dependant windows.
//	COPYRIGHT (C) 1990-1993.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

// NOTE: This is program makes use of international characters.  By default
//       the library uses the ISO8859-1 standard.  What characters appear
//       on the screen depend on the font used.  Fonts included in the 
//       libraries installed make use of ISO8859-1 standard.  Mapping is
//       provided for text mode for code page 850.  In order to have the 
//       proper characters appear in the text mode version of this program
//       you must have code page 850 on your machine.

#include <string.h>
#include <stdio.h>

#define ZIL_PERSISTENCE
#include <ui_win.hpp>

#include "lang_win.hpp"

// Country codes defined.
#define US			 1
#define GERMANY		49
#define SPAIN		34

class EXPORT INTL_WINDOW : public UIW_WINDOW
{
public:
	static UI_WINDOW_OBJECT *New(const char *name, UI_STORAGE *file = NULL,
		UI_STORAGE_OBJECT *object = NULL);

protected:
	// This constructor is protected since the name must be 
	// ``internationalized'' before it is called.
	INTL_WINDOW(const char *name, UI_STORAGE *file = NULL,
		UI_STORAGE_OBJECT *object = NULL) : UIW_WINDOW(name, file, object) {}
};

UI_WINDOW_OBJECT *INTL_WINDOW::New(const char *name, UI_STORAGE *file,
	UI_STORAGE_OBJECT *object)
{
	if (!UI_INTERNATIONAL::initialized)
		UI_INTERNATIONAL::Initialize();

	// Get the current country code.
	char langID[5];
	switch (UI_INTERNATIONAL::countryCode)
	{
	case US:
		strcpy(langID, "US_");
		break;

	case SPAIN:
		strcpy(langID, "SPA_");
		break;

	case GERMANY:
		strcpy(langID, "GER_");
		break;

	default:
		strcpy(langID, "US_");
	}

	// Seperate the object's name request.
	char pathName[128], fileName[32], objectName[32], objectPathName[128];
	UI_STORAGE::StripFullPath(name, pathName, fileName, objectName, objectPathName);

	// Put the new name request back together.
	char langObjectName[330];
	sprintf(langObjectName, "%s%s~%s%s%s", pathName, fileName, objectPathName,
		langID, objectName);

	// Load the window.
	UI_WINDOW_OBJECT *obj = new INTL_WINDOW(langObjectName, file, object);

	return obj;
}

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the 
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

	UI_WINDOW_OBJECT::helpSystem = new UI_HELP_SYSTEM("lang_win.dat",		// BUG.1992
		windowManager, HELP_GENERAL);

#if defined(ZIL_MSDOS)
	// Translations taken from IBM SAA CUA Advanced Interface Design Guide
	static char *germanSysPrompts[] =
	{
		"&Zur\373ckspeichern",			// option 1 (restore)
		"&Verschieben",					// option 2 (move)
		"&Gr\366\337e \344ndern",		// option 3 (size)
		"Mi&nimieren",					// option 4 (minimize)
		"Ma&ximieren",					// option 5 (maximize)
		"",								// option 6
		"&Schliessen"					// option 7 (close)
	};
	static char *spanishSysPrompts[] =
	{
		"&Restaurar",				  	// option 1 (restore)
		"&Mover",						// option 2 (move)
		"&Tama\361o",	  			  	// option 3 (size)
		"Mi&nimizar",				  	// option 4 (minimize)
		"Ma&ximizar",				  	// option 5 (maximize)
		"",							  	// option 6
		"&Cerrar"					  	// option 7 (close)
	};

	UI_INTERNATIONAL::Initialize();
	switch(UI_INTERNATIONAL::countryCode)
	{
	case GERMANY:
		UIW_SYSTEM_BUTTON::sysPrompts = germanSysPrompts;
		break;
	case SPAIN:
		UIW_SYSTEM_BUTTON::sysPrompts = spanishSysPrompts;
		break;
	}
#endif

	// Get the generic ``window'' from the .DAT file.
	UI_WINDOW_OBJECT *window = INTL_WINDOW::New("lang_win.dat~WINDOW");
	*windowManager + window;

	// Process user responses.
	UI_APPLICATION::Control();

	delete UI_WINDOW_OBJECT::helpSystem;									// BUG.1992

	return (0);
}
