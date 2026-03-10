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
	int nRet = 1;

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
	//
    BOOL fromInf = TRUE;
    if (argc > 1)
	{
		// INF2DAT
        if (!strcmp(argv[1], "/r") || !strcmp(argv[1], "/R"))
            fromInf = FALSE;
	}

	do // Non SHE
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

		PH_ENGINE* phEngine = new PH_ENGINE;

		cout << "Procesando ..." << endl;

		if (fromInf)
		{
			phEngine->Inf2Dat();
		}
		else
		{
			cout << "DAT2INF activado !" << endl;
			phEngine->Dat2Inf();
		}
		cout << "Listo..." << endl;

		delete phEngine;

		nRet = 0;
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