//
// [ INF2DAT.CPP ]
//
#include "stdst.h"

#include "ph_eng.h"

#ifdef DOSX286
#include <phapi.h>
#endif

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
		<< "    /c crear actualizaci¢n"   << endl
		<< "    /a aplicar actualizaci¢n" << endl
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
			break;
		}

		if (!TraceInfo::s_bTest)
		{
			STR32 password;
			cout << "Presione Esc para abortar operaci¢n." << endl;
			cout << "C¢digo de acceso: ";
			_ReadPassword(password, sizeof(CFG::PASSWORD)-1);
			if (!strlen(password))
			{
				nRet = 0;
				break;
			}

			if (!g_cfg->isUtilPassword(password))
			{
				cerr << "Lo siento, acceso negado." << endl;
				break;
			}
		}

		PH_ENGINE* phEngine;
		phEngine = new PH_ENGINE;
		if (!phEngine)
			break;

		do // Non SHE
		{
			if (creating)
			{
				cout << "  Creando actualizaci¢n ..." << endl;
				if (!phEngine->CreatePatch())
				{
					cerr << "  Fall¢ la extracci¢n de la actualizaci¢n." << endl;
					break;
				}
			}
			else
			{
				cout << "  Aplicando actualizaci¢n ..." << endl;
				if (!phEngine->ApplyPatch())
				{
					cerr << "  Fallo la aplicaci¢n de la actualizaci¢n." << endl;
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