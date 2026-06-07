//
// [ STC.CPP ]
//

#include "stdst.h"

#include <conio.h>
#include <stc.h>
#include <modemdev.h>
#include <sid.h>
#include "../../util_cfg.h"

extern unsigned _stklen = 0x4000;

CFG *g_cfg;
int g_where = 0;

char *errorMemory;
void NewHandler(void);

int main(WORD argc, char *argv[])
{
    errorMemory = new char[0x1000];
    extern void (*_new_handler)(void);
    _new_handler = NewHandler;
    //
    // check config and passwords...
    //
    clrscr();
	cout
	<< "STC 2.20 (" << APP_VER_NAME << ')' << endl
    << APP_COPYRIGHT << endl
    << endl
    ;
    //
	g_cfg = new CFG;
	if (!util_cfgLoad(g_cfg)) { delete g_cfg; return 1; }
	if (!util_authenticate(g_cfg)) { delete g_cfg; return 1; }
	delete g_cfg;
    //
    // Set up the storage and display search path.
	// Begin 2.21.8 Build 6
	UI_STORAGE::searchPath =  new UI_PATH("..\\..\\bin\\", TRUE);
	// End 2.21.8 Build 6

	UI_WINDOW_OBJECT::defaultStorage = new UI_STORAGE("res.dat", UIS_READ);
    //
    UI_DISPLAY *display = new UI_GRAPHICS_DISPLAY;

    UI_EVENT_MANAGER *eventManager = new UI_EVENT_MANAGER(display);
    *eventManager
    + new UID_KEYBOARD
    + new UID_MOUSE
    + new UID_CURSOR
    ;
    UI_WINDOW_MANAGER *windowManager = new UI_WINDOW_MANAGER(display, eventManager);
    UID_KEYBOARD::breakHandlerSet = L_EXIT_FUNCTION;
    // Attach the error and help systems.
    UI_WINDOW_OBJECT::errorSystem = new UI_ERROR_SYSTEM;
    UI_WINDOW_OBJECT::helpSystem = new UI_HELP_SYSTEM("help.dat", windowManager);
    // Check for data file errors.
    if (UI_WINDOW_OBJECT::defaultStorage->storageError)
    {
        UI_WINDOW_OBJECT::errorSystem->ReportError(windowManager,
                WOS_NO_STATUS, "Error abriendo el archivo RES.DAT...\n");
        delete UI_WINDOW_OBJECT::helpSystem;
        delete UI_WINDOW_OBJECT::errorSystem;
        delete UI_WINDOW_OBJECT::defaultStorage;
        delete windowManager;
        delete eventManager;
        delete display;
        return (1);
    }
    UIW_WINDOW *window;
    window = new STC;
    windowManager->Center(window);
    *windowManager + window;
    //
    *eventManager	+ ( STC::modemDevice = new MODEM_DEVICE(D_ON));
    UI_EVENT event;
    // Process user events.
    EVENT_TYPE ccode;
    do
    {
        if (!eventManager->Get(event))
            ccode = windowManager->Event(event);
    }
    while (ccode != L_EXIT && ccode != S_NO_OBJECT);
    // Clean up.
    delete UI_WINDOW_OBJECT::helpSystem;
    delete UI_WINDOW_OBJECT::errorSystem;
    delete UI_WINDOW_OBJECT::defaultStorage;
    delete windowManager;
    delete eventManager;
    delete display;
    //
	if (g_where)
        cout << "where = " << g_where << endl;
    //
    return (0);
}

MODEM_DEVICE *STC::modemDevice = NULL;

STC::STC(void): UIW_WINDOW("STC", defaultStorage)
{
	g_cfg = new CFG;
	g_cfg->Load(); // load .CFG
    //
    windowManager->Center(this);
    //	helpContext = H_STC;
}


STC::~STC(void)
{
    delete g_cfg;
}

EVENT_TYPE STC::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_ACCEPT:
        {
            eventManager->Put(UI_EVENT(S_CLOSE,0));
            break;
        }
    case UE_CONNECTSERVER:
        {
            UIW_WINDOW *window = new CONNECTION(CONNECTION::ASSERVER);
            windowManager->Center(window);
            *windowManager + window;
            break;
        }
    case UE_CONNECTCLIENT:
        {
            UIW_WINDOW *window = new CONNECTION(CONNECTION::ASCLIENT);
            windowManager->Center(window);
            *windowManager + window;
            break;
        }
    case UE_ACTIVATESERVER:
        {
            UIW_WINDOW *window = new ACTIVATION(ACTIVATION::ASSERVER);
            windowManager->Center(window);
            *windowManager + window;
            break;
        }
    case UE_ACTIVATECLIENT:
        {
            UIW_WINDOW *window = new ACTIVATION(ACTIVATION::ASCLIENT);
            windowManager->Center(window);
            *windowManager + window;
            break;
        }
    case UE_CONFIG:
        {
            UIW_WINDOW *window = new MODEMCFG;
            windowManager->Center(window);
            *windowManager + window;
            break;
        }
    default:
        ccode = UIW_WINDOW::Event(event);
        break;
    }
    return ccode;
}

void NewHandler(void)
{
    if (errorMemory)
        delete [] errorMemory;
    clrscr();
    cout
    << "No hay memoria disponible" << endl
    << "por favor reporte este problema ..."
    << endl;
    exit(1);
}
