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
		<< "UPDATE 1.01 (" << APP_VER_NAME << ')' << endl
		<< APP_COPYRIGHT << endl
		<< "  update [/c|/a]" << endl
		<< "    /c crear actualizaci�n"   << endl
		<< "    /a aplicar actualizaci�n" << endl
		<< endl
	;

	int nRet = 1;

	if (argc == 1)
	{
		return nRet; // nothing to do
	}

	BOOL creating = (!strcmp(argv[1], "/c") || !strcmp(argv[1], "/C"));
	BOOL applying = (!strcmp(argv[1], "/a") || !strcmp(argv[1], "/A"));

	if (!creating && !applying)
	{
		return nRet;
	}

	do // Non SEH
	{
		g_cfg = new CFG;
		if (!g_cfg)
		{
			break;
		}

		if (!util_cfgLoad(g_cfg)) { delete g_cfg; return 1; }

		if (!util_authenticate(g_cfg)) { delete g_cfg; return 1; }

		PH_ENGINE* phEngine;
		phEngine = new PH_ENGINE;
		if (!phEngine)
			break;

		do // Non SHE
		{
			if (creating)
			{
				cout << "  Creando actualizaci�n ..." << endl;
				if (!phEngine->CreatePatch())
				{
					cerr << "  Fall� la extracci�n de la actualizaci�n." << endl;
					break;
				}
			}
			else
			{
				cout << "  Aplicando actualizaci�n ..." << endl;
				if (!phEngine->ApplyPatch())
				{
					cerr << "  Fallo la aplicaci�n de la actualizaci�n." << endl;
					break;
				}
			}

			cout << "  Listo." << endl;
		}
		while (0);

		delete phEngine;
	}
	while (0);

	delete g_cfg;

	return nRet;
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