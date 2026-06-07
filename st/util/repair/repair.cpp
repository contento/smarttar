//
// [ REPAIR.CPP ]
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
#include "../util_cfg.h"

extern _stklen = 0x4000; // ~%^%&$#@!&*, _stklen GRRR ...

CFG *g_cfg;

char *errorMemory;

void NewHandler(void);

int main(void)
{
    cout
    << "REPAIR 1.0 (" << APP_VER_NAME << ')' << endl
    << APP_COPYRIGHT << endl
    << endl
    ;
    //
    errorMemory = new char[0x200];
    extern void (*_new_handler)(void);
    _new_handler = NewHandler;
    //
	g_cfg = new CFG;
	if (!util_cfgLoad(g_cfg)) { delete g_cfg; return 1; }
	if (!util_authenticate(g_cfg)) { delete g_cfg; return 1; }
                STR16 dateStr;
                cout << "Fecha del �ltimo acumulado archivado (dd/mm/aaaa): ";
                cin  >> dateStr;
                // find last turn
                WORD year, month, day;
                if (_Str2Date(dateStr, year, month, day))
                {
                    FILE_NAME fnSource;
                    char *fmt = "%04d\\%02d\\rx%02d_%02d.sta";
                    int turn = 0;
                    // find last turn
                    do
                    {
                        turn++;
                        sprintf(fnSource, fmt, year, month, day, turn);
                        _PrefixAppPath(fnSource);
                    }
                    while (!access(fnSource, 0));
                    turn--;
                    if (turn)
                    {
                        sprintf(fnSource, fmt, year, month, day, turn);
                        _PrefixAppPath(fnSource);
                        cout << "  Ultimo acumulado: " << fnSource << endl;
                        //
                        // repair based on last turn.
                        //
                        FILE_NAME fnTarget;
                        strcpy(fnTarget, "rx.sta");
                        _PrefixAppPath(fnTarget);
                        // make backup copy
                        FILE_NAME fnBackup;
                        strcpy(fnBackup, "rxbak.sta");
                        _PrefixAppPath(fnBackup);
                        if (_CopyFile(fnTarget, fnBackup) == CP_OK)
                        {
                            cout << "  " << fnTarget << " -> " << fnBackup << "." << endl;
                            // copy here the last turn
                            if (_CopyFile(fnSource, fnTarget) == CP_OK)
                            {
                                cout << "  " << fnTarget << " <- " << fnSource << "." << endl;
                                // repair
                                cout << "  Reparando acumulados ..." << endl;
                                DB_ENGINE dBEngine;
                                dBEngine.Repair();
                                cout << "  Listo." << endl;
                            }
                            else
                            {
                                cout << "  " << fnTarget << " no pudo ser restaurado." << endl;
                            }
                        }
                        else
                        {
                            cout << "  " << fnTarget << " no pudo ser guardado como " << fnBackup << "." << endl;
                        }
                    }
                    else
                    {
                        cerr << "No existen ningun acumulado en " << dateStr << "." << endl;
                    }
                }
                else
                {
                    cerr << "Fecha inv�lida." << endl;
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

