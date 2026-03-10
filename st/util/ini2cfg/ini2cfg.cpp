//
// [ INI2CFG.CPP ]
//

#include "stdst.h"

#ifdef DOSX286
#include <phapi.h>
#endif

extern unsigned _stklen = 0x4000; // ~%^%&$#@!&*, _stklen GRRR ...

char *errorMemory;

void NewHandler(void);

int main(int argc, char *argv[])
{
	errorMemory = new char[0x200];
	extern void (*_new_handler)(void);
	_new_handler = NewHandler;

	cout
	   << "INI2CFG 1.11 (" << APP_VER_NAME << ')' << endl
	   << APP_COPYRIGHT << endl
	   << "  ini2cfg [/r]" << endl
	   << "    /r para activar CFG2INI" << endl
	   << endl
	;

	BOOL fromIni = TRUE;
	if (argc > 1)
	{ // INI2CFG
		if (!strcmp(argv[1], "/r") || !strcmp(argv[1], "/R"))
		{
			fromIni = FALSE;
			cout << "CFG2INI activado !" << endl;
		}
	}

	CFG *g_cfg;
	g_cfg = new CFG;
	if (!g_cfg)
	{
		return 1;
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

		return 1;
	}

	if (!TraceInfo::s_bTest)
	{
		STR32 password;
		cout << "Presione Esc para abortar operaci¢n." << endl;
		cout << "C¢digo de acceso: ";
		_ReadPassword(password, sizeof(CFG::PASSWORD)-1);
		if (!strlen(password))
			return 0;

		if (!g_cfg->isUtilPassword(password))
		{
			cerr << "Lo siento, acceso negado." << endl;
			return 1;
		}
	}
	delete g_cfg; // bye

	g_cfg = new CFG;
	if (fromIni)
	{
		cout << "  Convirtiendo .INI a .CFG..." << endl;
		g_cfg->Load(NULL, TRUE); // load .INI
	}
	else
	{
		cout << "  Convirtiendo .CFG a .INI..." << endl;
		g_cfg->Load(); // load CFG
	}

	g_cfg->Save();
	delete g_cfg;

	cout << "  Listo." << endl;

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