//
// [ RXL.CPP ]
//

#include <iostream.h>
#include <stdio.h>
#include <io.h>

#include <info.h>
#include <cfg.h>
#include <st_util.h>
#include <db_eng.h>

#ifdef DOSX286
#include <phapi.h>
#endif

extern _stklen = 0x4000; // ~%^%&$#@!&*, _stklen GRRR ...
char *errorMemory;

void NewHandler(void);
BOOL CheckPassword();
BOOL CheckCfg();

CFG *_cfg;

BOOL callback(RECEIPT const &receipt)
{
	cout
    	<< receipt.Number
        << endl
		;
    return TRUE;
}

int main(void)
{
    cout
        << "CHKRX 1.0 (" << APP_VER_NAME << ')' << endl
        << APP_COPYRIGHT << endl
        << endl
        ;
    //
    errorMemory = new char[0x200];
    extern void (*_new_handler)(void);
    _new_handler = NewHandler;
    //
    if (CheckCfg())
    {
    	if (CheckPassword)
		{
        	DB_ENGINE dbEngine;
			dbEngine.EnumReceipts(callback);
        }
    }
    //
    delete _cfg;
    //
    return 0;
}

void NewHandler(void)
{
    if (errorMemory)
        delete [] errorMemory;
    cout
    << "No hay memoria disponible" << endl
    << "por favor reporte este problema ..."
    << endl
    ;
    exit(1);
}

BOOL CheckCfg()
{
    _cfg = new CFG;
    WORD status = _cfg->Load(); // load CFG
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
    }
    return (status == CFG::OK);
}

BOOL CheckPassword()
{
	BOOL retVal = FALSE;

    STR32 password;
    cout << "Presione Esc para abortar operaci¢n." << endl;
    cout << "C¢digo de acceso: ";
    strcpy(password, "Util");
    //_ReadPassword(password, sizeof(CFG::PASSWORD)-1);
    if (strlen(password))
        if (_cfg->isUtilPassword(password))
	       	retVal = TRUE;

	if (!retVal)
    {
		cerr << "Lo siento, acceso negado." << endl;
    }

    return retVal;
}