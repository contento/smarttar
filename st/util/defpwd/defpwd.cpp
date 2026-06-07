//
// [ DEFPWD.CPP ]
//
#include <iostream.h>
#include <cfg.h>
#include <st_util.h>
#include "../util_cfg.h"

#ifdef DOSX286
#include <phapi.h>
#endif

extern _stklen = 0x4000; // ~%^%&$#@!&*, _stklen GRRR ...

char *errorMemory;

void NewHandler(void);

int main(void)
{
    cout
    << "DEFPWD 1.0 (" << APP_VER_NAME << ')' << endl
    << APP_COPYRIGHT << endl
    << endl
    ;
    //
    errorMemory = new char[0x200];
    extern void (*_new_handler)(void);
    _new_handler = NewHandler;
    //
    CFG *_cfg;
    _cfg = new CFG;
    if (!util_cfgLoad(_cfg)) { delete _cfg; return 0; }
    STR32 password;
    cout << "Presione Esc para abortar operaci\xE2n." << endl;
    cout   << "C\xF3digo de acceso: ";
    _ReadPassword(password, sizeof(CFG::PASSWORD)-1);
    if (strlen(password))
    {
        if (_isDatePassword(password))
        {
            _cfg->setDefaultPasswords();
            cout << "  Listo." << endl;
            _cfg->Save();
        }
        else
        {
            cerr << "Lo siento, acceso negado." << endl;
        }
    }
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