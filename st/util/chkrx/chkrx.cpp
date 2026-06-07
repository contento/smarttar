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
#include "../util_cfg.h"

#ifdef DOSX286
#include <phapi.h>
#endif

extern _stklen = 0x4000; // ~%^%&$#@!&*, _stklen GRRR ...
char *errorMemory;

void NewHandler(void);
BOOL CheckPassword();
BOOL CheckCfg();

CFG *g_cfg;

BOOL callback(Receipt const &receipt)
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
    	if (CheckPassword())
		{
        	DB_ENGINE dbEngine;
			dbEngine.EnumReceipts(callback);
        }
    }
    //
	delete g_cfg;
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
	return util_cfgLoad(g_cfg);
}

BOOL CheckPassword()
{
	return util_authenticate(g_cfg);
}