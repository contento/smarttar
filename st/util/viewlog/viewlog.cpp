//
// [ VIEWLOG.CPP ]
//
#include <iostream.h>
#include <iomanip.h>
#include <string.h>

#include <st_util.h>
#include <cfg.h>
#include <log.h>
#include "../util_cfg.h"

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
	if (!util_cfgLoad(g_cfg)) { delete g_cfg; return 1; }
	if (!util_authenticate(g_cfg)) { delete g_cfg; return 1; }
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
    //
	delete g_cfg;
    return 0;
}