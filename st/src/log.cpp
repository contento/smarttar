//
// [ LOG.CPP ]
//

#include "stdst.h"

const char * LOG_FILENAME = "LOG.DAT";

Log::Log(int openMode)
{
	this->openMode = openMode;
	strcat(_GetAppPath(filename), LOG_FILENAME);
	open(filename, openMode);
}

Log::Log(const char *filename, int openMode)
{
    this->openMode = openMode;
    strcpy(this->filename, filename);
    open(filename, openMode);
}

BOOL Log::open(const char *filename, int openMode)
{
    if (openMode & IN)
    {
        file = new fstream(filename, ios::in|ios::binary);
    }
    else
    {
        if (access(filename, 6) != 0)
            chmod(filename, S_IREAD|S_IWRITE); // enable read/write
        file = new fstream(filename, ios::in|ios::out|ios::binary|ios::app);
    }
    openOk = (file)?TRUE:FALSE;
    FILE_HEADER header;
    file->read((char *)&header, sizeof(FILE_HEADER));
    openOk &= (file->gcount() == sizeof(FILE_HEADER)) && header.IsValid();
    if (!openOk)
    {
        if (openMode & CREATE)
        {
            delete file;
            // create/trunc
            file = new fstream(filename, ios::out|ios::binary|ios::trunc);
            FILE_HEADER header;
            file->write((char *)&header, sizeof(FILE_HEADER));
            delete file;
            // re-open
            if (openMode & IN)
            {
                file = new fstream(filename, ios::in|ios::binary);
            }
            else
            {
                file = new fstream(filename, ios::in|ios::out|ios::binary|ios::app);
            }
            openOk = (file)?TRUE:FALSE;
        }
    }
    if (!openOk)
    {
        delete file;
    }
    return openOk;
}

Log::~Log()
{
    delete file;
    if (openMode & OUT)
        chmod(filename, S_IREAD);
}

BOOL Log::put(int motif)
{
    Entry entry;
    entry.motif = motif;
    return put(entry);
}

BOOL Log::put(int date, int time, int motif)
{
    Entry entry;
    entry.date  = date;
    entry.time  = time;
    entry.motif = motif;
    return put(entry);
}

BOOL Log::put(Entry& entry)
{
    BOOL ok = openOk && (openMode & OUT);
    if (ok)
    {
        file->write((char *)&entry, sizeof(Entry));
        ok = !file->fail();
    }
    return ok;
}

BOOL Log::get(Entry& entry)
{
    BOOL ok = openOk && (openMode & IN);
    if (ok)
    {
        file->read((char *)&entry, sizeof(Entry));
        ok = (file->gcount() == sizeof(Entry));
    }
    return ok;
}

BOOL Log::restart()
{
    BOOL ok = openOk && (openMode & IN);
    if (ok)
    {
        file->seekg(sizeof(FILE_HEADER));
    }
    return ok;
}

BOOL Log::archive(void)
{
	extern CFG *g_cfg;

	file->flush();
	BOOL ok = FALSE;
	FILE_NAME arcFilename;
	_mkSysDateDir();
	_getSysDatePath(arcFilename);
	_PrefixAppPath(arcFilename);
	STR16 tmp;
	WORD year, month, day;
	_GetSysDate(year, month, day);
	sprintf(tmp, "\\LOG_%02d%02d.DAT", day, g_cfg->TURN_NUMBER);
    strcat(arcFilename, tmp);
    if (access(arcFilename, 6) != 0)
        chmod(arcFilename, S_IREAD|S_IWRITE); // enable read/write
    unlink(arcFilename); // sorry.
    ok = (rename(filename, arcFilename) == 0);
    if (ok)
        chmod(arcFilename, S_IREAD);
    delete file;
    open(filename, openMode|CREATE); // force to init
    return ok;
}

char *Log::motif2Str(char *str, int motif, BOOL shortMotif) const
{
    static char *motifs[] = {
        "Ninguno", // NONE,
        "Arranque normal", // NORMALSTART,
        "Salida normal",   // NORMALSHUTDOWN,
        "Cambio de turno",     // CHANGETURN,
        "Error en aplicación", // APPBADTRY,
        "Problemas en la configuración", // CFGBADTRY,
        "Problemas con el dongle", // DONGLEBADTRY,
        "Problemas con EEPROM",    // EEPROMBADTRY,
        "Problemas con la STM2",   // STM2BADTRY,
        "Basura en la STM2",       // STM2GARBAGE,
        "Salida ANORMAL",          // STM2BADSHUTDOWN,
        "Recuperación automática", // STM2RECOVER,
        "Recuperación IGNORADA",   // STM2IGNORERECOVER,
        //
        "No reconocido" // , UNRECOGNIZED
    };
    static char *shortMotifs[] = {
        "Ninguno", // NONE,
        "Arranque normal",  // NORMALSTART,
        "Salida normal",    // NORMALSHUTDOWN,
        "Cambio de turno",  // CHANGETURN,
        "Error en aplic.",  // APPBADTRY,
        "Error en .CFG",    // CFGBADTRY,
        "Error en dongle",  // DONGLEBADTRY,
        "Error en EEPROM",  // EEPROMBADTRY,
        "Error en STM2",    // STM2BADTRY,
        "Basura en STM2",   // STM2GARBAGE,
        "Salida ANORMAL",   // STM2BADSHUTDOWN,
        "Recup. automáti.", // STM2RECOVER,
        "Recup. IGNORADA",  // STM2IGNORERECOVER,
        //
        "No reconocido" // , UNRECOGNIZED
    };
    if (motif < 0 || motif > UNRECOGNIZED)
    {
        strcpy(str, motifs[UNRECOGNIZED]);
    }
    else
    {
        if (shortMotif)
            strcpy(str, shortMotifs[motif]);
        else
            strcpy(str, motifs[motif]);
    }
    return str;
}

