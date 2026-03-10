//
// [ VIEWLOG.CPP ]
//
#include <iostream.h>
#include <iomanip.h>
#include <string.h>

#include <st_util.h>
#include <cfg.h>
#include <log.h>

CFG *g_cfg;

int main(int argc, char *argv[])
{
    cout
    << "VIEWLOG 1.0 (" << APP_VER_NAME << ')' << endl
    << APP_COPYRIGHT      << endl
    << "  viewlog [filename]" << endl
    << endl
    ;
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
    }
    else
    {
        STR32 password;
        cout << "Presione Esc para abortar operaci¢n." << endl;
        cout << "C¢digo de acceso: ";
        _ReadPassword(password, sizeof(CFG::PASSWORD)-1);
        cout << endl;
        if (strlen(password))
        {
			if (g_cfg->isUtilPassword(password))
            {
                Log *log;
                if (argc > 1)
                { // INF2DAT
                    log = new Log(argv[1], Log::IN);
                }
                else
                {
                    log = new Log(Log::IN);
                }
                if (log->openFail())
                {
                    cerr << "  No se pudo abrir " << argv[1] << endl;
                }
                else
                {
                    Log::Entry entry;
                    char timeStr[16], dateStr[16];
                    char motifStr[64];
                    WORD hour, minutes, seconds, year, month, day;
                    int i=0;
                    while (log->get(entry))
                    {
                        _UnpackTime(entry.time, hour, minutes, seconds);
                        _Time2Str(timeStr, hour, minutes, seconds, TRUE);
                        _UnpackDate(entry.date, year, month, day);
                        _Date2Str(dateStr, year, month, day);
                        cout
                        << "[" << setw(2) << ++i << "] "
                        << dateStr << " " << timeStr << " "
                        << _ISO2ASCII(log->motif2Str(motifStr, entry.motif)) << endl
                        ;
                    }
                }
                delete log;
            }
            else
            {
                cerr << "Lo siento, acceso negado." << endl;
            }
        }
    }
    //
	delete g_cfg;
    return 0;
}