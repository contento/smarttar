//
// [ TEST.CPP ]
//
#include <iostream.h>
#include <iomanip.h>
#include <string.h>

#include <st_util.h>
#include <cfg.h>
#include <log.h>

CFG *_cfg;

int main(void)
{
    cout
    << "LOG 1.0 (" << APP_VER_NAME << ')' << endl
    << APP_COPYRIGHT << endl
    << endl
    ;
    _cfg = new CFG;
    WORD status = _cfg->Load(); // load CFG
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
        // log
        Log *log = new Log(Log::OUT|Log::CREATE);
        int i;
        randomize();
        for (i = 0; i < random(20); i++)
        {
            log->put(random(Log::UNRECOGNIZED));
        }
        delete log;
        // show
        log = new Log(Log::IN);
        Log::Entry entry;
        char timeStr[16], dateStr[16];
        char motifStr[64];
        WORD hour, minutes, year, month, day;
        i=0;
        while (log->get(entry))
        {
            _UnpackTime(entry.time, hour, minutes);
            _Time2Str(timeStr, hour, minutes);
            _UnpackDate(entry.date, year, month, day);
            _Date2Str(dateStr, year, month, day);
            cout
            << "[" << setw(2) << ++i << "] "
            << dateStr << " " << timeStr << " "
            << _ISO2ASCII(log->motif2Str(motifStr, entry.motif)) << endl
            ;
        }
        delete log;
        // archive
        log = new Log(Log::OUT);
        log->archive();
        delete log;
        //
        log = new Log(Log::OUT|Log::CREATE);
        for (i = 0; i < random(20); i++)
        {
            log->put(random(Log::UNRECOGNIZED));
        }
        delete log;
    }
    delete _cfg;
    return 0;
}
