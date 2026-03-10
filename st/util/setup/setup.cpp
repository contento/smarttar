//
// [ SETUP.CPP ]
//

#include "stdst.h"

#include <conio.h>
#include <ui_win.hpp>

#include <ph_eng.h>
#include <events.h>
#include <eeprom.h>
#include <info.h>

#ifndef USE_HELP_CONTEXTS
#define USE_HELP_CONTEXTS
#include <help.hpp>
#endif

CFG *g_cfg;

class EXPORT SETUP : public UIW_WINDOW
{
public:
	SETUP(void);
	~SETUP(void);
	//
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	static  EVENT_TYPE Ini2Cfg(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
	static  EVENT_TYPE Cfg2Ini(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
	static  EVENT_TYPE Inf2Dat(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
	static  EVENT_TYPE Dat2Inf(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
};

class EXPORT INSTALL : public UIW_WINDOW
{
public:
	INSTALL(void);
	~INSTALL(void);
	//
	virtual EVENT_TYPE Event(const UI_EVENT &event);
};

extern unsigned _stklen = 0x4000;
char *errorMemory;
void NewHandler(void);

int main(WORD argc, char *argv[])
{
	BOOL install = FALSE;
	//
	errorMemory = new char[0x1000];
	extern void (*_new_handler)(void);
	_new_handler = NewHandler;
	//
	// check config and passwords...
	//
    clrscr();
    cout
	<< "SETUP 1.06 (" << APP_VER_NAME << ')' << endl
	<< APP_COPYRIGHT << endl
	;
	//
	// check to see if it's install ...
	//
	if (access("STELLA", 0) == 0)
	{
		unlink("STELLA"); // not to run again
		install = TRUE;
	}
	//
    // It's a typical session
    //
    if (!install)
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
    }
    if (!install)
    {
        STR32 password;
        if (argc > 1)
            strcpy(password, argv[1]);
        else
        {
            cout << "Presione Esc para abortar operaci¢n." << endl;
            cout << "C¢digo de acceso: ";
            _ReadPassword(password, sizeof(CFG::PASSWORD)-1);
            if (!strlen(password))
            {
				delete g_cfg;
                return 2;
            }
        }
		if (!g_cfg->isUtilPassword(password))
        {
            cerr << "Lo siento, acceso negado." << endl;
			delete g_cfg;
            return 3;
        }
		delete g_cfg;
    }
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
    if (install)
        window = new INSTALL;
    else
        window = new SETUP;
    windowManager->Center(window);
    *windowManager + window;
    //
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
    return (0);
}

//
// --- SETUP -----------------------------------------------------------------
//

SETUP::SETUP(void): UIW_WINDOW("SETUP", defaultStorage)
{
	g_cfg = new CFG;
	g_cfg->Load(); // load .CFG
    //
    Get("INI2CFG")->userFunction = Ini2Cfg;
    Get("CFG2INI")->userFunction = Cfg2Ini;
    Get("INF2DAT")->userFunction = Inf2Dat;
    Get("DAT2INF")->userFunction = Dat2Inf;
    //
    windowManager->Center(this);
    helpContext = H_SETUP;
}

SETUP::~SETUP(void)
{
	delete g_cfg;
}

EVENT_TYPE SETUP::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_ACCEPT:
        eventManager->Put(UI_EVENT(S_CLOSE,0));
        break;
    default:
        ccode = UIW_WINDOW::Event(event);
        break;
    }
    return ccode;
}

static char *CONVERT_MSG  = "Procesando, por favor espere ...";
static char *CHANGING_MSG = "Está usted a punto de cambiar información crítica.\n";

EVENT_TYPE SETUP::Ini2Cfg(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
    if (ccode != L_SELECT)
        return ccode;
    if (UI_WINDOW_OBJECT::errorSystem->ReportError(windowManager, WOS_INVALID, CHANGING_MSG) == WOS_NO_STATUS)
        return ccode;
    //
    UIW_STRING *wStr = (UIW_STRING *) object->parent->parent->Get("W_MSG");
    wStr->DataSet(CONVERT_MSG);
    eventManager->DeviceState(E_MOUSE, DM_WAIT);
    // process
	g_cfg->Load(NULL, TRUE); // load .INI
	g_cfg->Save();
    //
    wStr->DataSet("");
    //
    eventManager->DeviceState(E_MOUSE, DM_VIEW);
    return ccode;
}

EVENT_TYPE SETUP::Cfg2Ini(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
    if (ccode != L_SELECT)
        return ccode;
    if (UI_WINDOW_OBJECT::errorSystem->ReportError(windowManager, WOS_INVALID, CHANGING_MSG) == WOS_NO_STATUS)
        return ccode;
    //
    UIW_STRING *wStr = (UIW_STRING *) object->parent->parent->Get("W_MSG");
    wStr->DataSet(CONVERT_MSG);
    eventManager->DeviceState(E_MOUSE, DM_WAIT);
    // process
	g_cfg->Save();
    //
    wStr->DataSet("");
    eventManager->DeviceState(E_MOUSE, DM_VIEW);
    return ccode;
}

EVENT_TYPE SETUP::Inf2Dat(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
    if (ccode != L_SELECT)
        return ccode;
    if (UI_WINDOW_OBJECT::errorSystem->ReportError(windowManager, WOS_INVALID, CHANGING_MSG) == WOS_NO_STATUS)
        return ccode;
    //
    UIW_STRING *wStr = (UIW_STRING *) object->parent->parent->Get("W_MSG");
    wStr->DataSet(CONVERT_MSG);
    eventManager->DeviceState(E_MOUSE, DM_WAIT);
    // process
    PH_ENGINE *phEngine = new PH_ENGINE;
    phEngine->Inf2Dat();
    delete phEngine;
    //
    wStr->DataSet("");
    eventManager->DeviceState(E_MOUSE, DM_VIEW);
    return ccode;
}

EVENT_TYPE SETUP::Dat2Inf(UI_WINDOW_OBJECT *object, UI_EVENT &, EVENT_TYPE ccode)
{
    if (ccode != L_SELECT)
        return ccode;
    if (UI_WINDOW_OBJECT::errorSystem->ReportError(windowManager, WOS_INVALID, CHANGING_MSG) == WOS_NO_STATUS)
        return ccode;
    // put a message window to indicate a wait state
    UIW_STRING *wStr = (UIW_STRING *) object->parent->parent->Get("W_MSG");
    wStr->DataSet(CONVERT_MSG);
    eventManager->DeviceState(E_MOUSE, DM_WAIT);
    // process
    PH_ENGINE *phEngine = new PH_ENGINE;
    phEngine->Dat2Inf();
    delete phEngine;
    //
    eventManager->DeviceState(E_MOUSE, DM_VIEW);
    wStr->DataSet("");
    //
    return ccode;
}

//
// --- INSTALL ---------------------------------------------------------------
//

INSTALL::INSTALL(void): UIW_WINDOW("W_INSTALL", defaultStorage)
{
	g_cfg = new CFG;
	g_cfg->Load(); // load CFG
	g_cfg->Load(NULL, TRUE); // load INI
    //
    UIW_COMBO_BOX *wCombo;
    UIW_STRING *wStr;
    //
    wCombo = (UIW_COMBO_BOX *) Get("W_COUNTRY");
    for (wStr = (UIW_STRING *)wCombo->First(); wStr; wStr=(UIW_STRING *)wStr->Next())
    {
		if (!strcmp(wStr->DataGet(), g_cfg->COUNTRY))
            wCombo->list.SetCurrent(wStr);
    }
    //
    wCombo = (UIW_COMBO_BOX *) Get("W_CURRENCY");
    for (wStr = (UIW_STRING *)wCombo->First(); wStr; wStr=(UIW_STRING *)wStr->Next())
    {
		if (!strcmp(wStr->DataGet(), g_cfg->CURRENCY))
            wCombo->list.SetCurrent(wStr);
    }
    //
    wCombo = (UIW_COMBO_BOX *) Get("W_TAX_NAME");
    for (wStr = (UIW_STRING *)wCombo->First(); wStr; wStr=(UIW_STRING *)wStr->Next())
    {
		if (!strcmp(wStr->DataGet(), g_cfg->TAX_NAME))
            wCombo->list.SetCurrent(wStr);
    }
	((UIW_BIGNUM *)Get("W_TAX_PERCENT"))->DataSet(&UI_BIGNUM(g_cfg->TAX_PERCENT));
    //
    wCombo = (UIW_COMBO_BOX *) Get("W_DEALER");
    for (wStr = (UIW_STRING *)wCombo->First(); wStr; wStr=(UIW_STRING *)wStr->Next())
    {
		if (g_cfg->DEALER == wCombo->Index(wStr))
            wCombo->list.SetCurrent(wStr);
    }
    //
    char str[0xA];
    wCombo = (UIW_COMBO_BOX *) Get("W_ACCESS");
    for (wStr = (UIW_STRING *)wCombo->First(); wStr; wStr=(UIW_STRING *)wStr->Next())
    {
		if (!strcmp(wStr->DataGet(), itoa(g_cfg->ACCESS, str, 10)))
            wCombo->list.SetCurrent(wStr);
    }
	((UIW_STRING *)Get("W_CITY"))->DataSet(g_cfg->CITY);
	((UIW_STRING *)Get("W_COMPANY"))->DataSet(g_cfg->COMPANY);
	((UIW_STRING *)Get("W_ID"))->DataSet(g_cfg->ID);
    //
    wCombo = (UIW_COMBO_BOX *) Get("W_P_FORM");
    for (wStr = (UIW_STRING *)wCombo->First(); wStr; wStr=(UIW_STRING *)wStr->Next())
    {
		if (!strcmp(wStr->DataGet(), g_cfg->P_FORM))
            wCombo->list.SetCurrent(wStr);
    }
    //
    wnFlags |= WNF_SELECT_MULTIPLE;
    windowManager->Center(this);
    helpContext = H_INSTALL;
}

INSTALL::~INSTALL(void)
{
	g_cfg->AdjustForm();
	g_cfg->Save();
	delete g_cfg;
}

EVENT_TYPE INSTALL::Event(const UI_EVENT &event)
{
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case UE_ACCEPT:
        // Activate EEPROM.  version 2.19b
        BOOL ok = TRUE;
		extern SUPER_APP_INFO g_superAppInfo;
        if (!g_superAppInfo.Attr.NoEEPROM)
        {
            EEPROM eeprom;
            ok = eeprom.isCandidateVersion();
            if (ok)
			{
				// Force MicroDise¤o to program EEPROM before doing this.
				// eeprom.writeVersionId();
            }
        }
        if (!ok)
        {
            UI_WINDOW_OBJECT::errorSystem->ReportError(windowManager,
                    WOS_NO_STATUS, "\n\n      Versión no válida\n");
        }
        else
        {
            UIW_COMBO_BOX *wCombo;
            wCombo = (UIW_COMBO_BOX *) Get("W_COUNTRY");
			strcpy(g_cfg->COUNTRY, ((UIW_STRING *)wCombo->Current())->DataGet());
            wCombo = (UIW_COMBO_BOX *) Get("W_CURRENCY");
			strcpy(g_cfg->CURRENCY, ((UIW_STRING *)wCombo->Current())->DataGet());
            wCombo = (UIW_COMBO_BOX *) Get("W_TAX_NAME");
			strcpy(g_cfg->TAX_NAME, ((UIW_STRING *)wCombo->Current())->DataGet());
			((UIW_BIGNUM *)Get("W_TAX_PERCENT"))->DataGet()->Export(&g_cfg->TAX_PERCENT );
            wCombo = (UIW_COMBO_BOX *) Get("W_DEALER");
			g_cfg->DEALER = wCombo->Index(wCombo->Current());
            wCombo = (UIW_COMBO_BOX *) Get("W_ACCESS");
			g_cfg->ACCESS = ((UIW_INTEGER *)wCombo->Current())->DataGet();
			strcpy(g_cfg->CITY, ((UIW_STRING *)Get("W_CITY"))->DataGet());
			strcpy(g_cfg->COMPANY, ((UIW_STRING *)Get("W_COMPANY"))->DataGet());
			strcpy(g_cfg->ID, ((UIW_STRING *)Get("W_ID"))->DataGet());
            wCombo = (UIW_COMBO_BOX *) Get("W_P_FORM");
            strcpy(g_cfg->P_FORM, ((UIW_STRING *)wCombo->Current())->DataGet());
            // put a message window to indicate a wait state
            UIW_STRING *wStr = (UIW_STRING *) Get("W_MSG");
            wStr->DataSet(CONVERT_MSG);
            eventManager->DeviceState(E_MOUSE, DM_WAIT);
            // convert to PH_INFO
            if (FlagSet(((UIW_BUTTON *)Get("W_INFO"))->woStatus, WOS_SELECTED))
            {
                PH_ENGINE *phEngine = new PH_ENGINE;
                if (!phEngine->Inf2Dat())
                {
                    UI_WINDOW_OBJECT::errorSystem->ReportError(windowManager,
                            WOS_NO_STATUS, "\n\nNo se pudo ejecutar INF2DAT\n");
                }
                delete phEngine;
            }
            // update with patch
            if (FlagSet(((UIW_BUTTON *)Get("W_PATCH"))->woStatus, WOS_SELECTED))
            {
                PH_ENGINE *phEngine = new PH_ENGINE;
                if (!phEngine->ApplyPatch())
                {
                    UI_WINDOW_OBJECT::errorSystem->ReportError(windowManager,
                            WOS_NO_STATUS, "\n\nNo se pudo actualizar tarifas\n");
                }
                delete phEngine;
            }
            //
            eventManager->DeviceState(E_MOUSE, DM_VIEW);
            wStr->DataSet("");
        }
        //
        woAdvancedFlags &= ~WOAF_LOCKED; // disconnect lock !!!
        eventManager->Put(UI_EVENT(S_CLOSE,0));
        break;
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
    << endl
    ;
    exit(1);
}
