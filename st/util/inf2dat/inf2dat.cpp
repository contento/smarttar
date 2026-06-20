//
// [ INF2DAT.CPP ]
//

#include "stdst.h"
#include "ph_eng.h"

#ifdef DOSX286
#include <phapi.h>
#endif
#include "../util_cfg.h"

extern unsigned _stklen = 0x4000; // ~%^%&$#@!&*, _stklen GRRR ...

CFG *g_cfg = NULL;

char *errorMemory;

void NewHandler(void);

int main(int argc, char *argv[])
{
	errorMemory = new char[0x200];
	extern void (*_new_handler)(void);
	_new_handler = NewHandler;

	cout
		<< "INF2DAT 1.03 (" << APP_VER_NAME << ')' << endl
		<< APP_COPYRIGHT << endl
		<< "  inf2dat [/r]" << endl
		<< "    /r para activar DAT2INF" << endl
		<< endl
	;

	BOOL fromInf = TRUE;
	if (argc > 1)
	{
		if (!strcmp(argv[1], "/r") || !strcmp(argv[1], "/R"))
		{
			fromInf = FALSE;
			cout << "DAT2INF activado !" << endl;
		}
	}

	g_cfg = new CFG;
	if (!g_cfg)
		return 1;

	if (!util_cfgLoad(g_cfg))
	{
		delete g_cfg;
		return 1;
	}

	if (!util_authenticate(g_cfg))
	{
		delete g_cfg;
		return 1;
	}

	PH_ENGINE *phEngine = new PH_ENGINE;
	if (!phEngine)
	{
		delete g_cfg;
		return 1;
	}

	cout << "Procesando ..." << endl;

	if (fromInf)
	{
		phEngine->LoadFromInfs(); // tariffs from PH_INFO.BIN + places from .inf
		phEngine->Save();         // write compiled PH_INFO.BIN
	}
	else
	{
		phEngine->Load();         // read compiled PH_INFO.BIN
		phEngine->SaveToInfs();   // export places back to .inf
	}

	cout << "Listo." << endl;

	delete phEngine;
	delete g_cfg;

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
