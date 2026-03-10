//
// [ VIEWER.CPP ]
//

#include <ui_win.hpp>

#include <db_eng.h>
#include <cfg.h>
#include <db_view.h>
#include <iostream.h>

DB_ENGINE 	*g_dbEngine;
CFG   		*g_cfg;

int UI_APPLICATION::Main(void)
{
	g_cfg = new CFG;
	WORD status = g_cfg->Load(); // load CFG
    if (status != CFG::OK)
    {
        char *msg = " tiene una falla general.";
        switch (status)
        {
        case CFG::NO_CFG_FILE :
            msg = "no existe."    ;
            break;
        case CFG::BAD_CFG_FILE:
            msg = "est  corrupto.";
            break;
        }
        cerr << "El archivo de configuraci¢n " << msg << endl;
		delete g_cfg;
        return 1;
    }
	g_dbEngine = new DB_ENGINE;
	//
	UI_APPLICATION::LinkMain();
	// Set up the storage and display search path.

	// Begin 2.21.8 Build 6
	UI_STORAGE::searchPath =  new UI_PATH("..\\..\\bin\\", TRUE);
	// End 2.21.8 Build 6

	UI_WINDOW_OBJECT::defaultStorage = new UI_STORAGE("res.dat", UIS_READ);
	//
    UIW_WINDOW *window = new DBView(argc>1);
    //
    windowManager->Center(window);
    *windowManager + window;
    // Wait for user response.
    UI_EVENT event;
    EVENT_TYPE ccode;
    do
    {
        // Get input from the user.
        eventManager->Get(event);
        // Send event information to the window manager.
        ccode = windowManager->Event(event);
    }
    while (ccode != L_EXIT && ccode != S_NO_OBJECT);
    //
    delete g_cfg;
	delete g_dbEngine;
    return (0);
}
