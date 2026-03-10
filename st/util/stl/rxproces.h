#ifndef __RXPROCES_H
#define __RXPROCES_H

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

#include <db_eng.h>

struct CommandOptions
{
    BOOL
    currentTurn,
    sortByBooth,
    onlySpecialReceipts,
    onlyAutomaticReceipts,
    onlyNotPaid,
    onlyTollFree,
    appendDatResult,
    showHelp
    ;
    WORD date;
    WORD turn;
    FILE_NAME lstPath;
    FILE_NAME rxBasePath;
    FILE_NAME rxBaseFilename;
    STR32     password;
    STR32     shortSerial;
};

class RXProcessor
{
public:
    RXProcessor(const CommandOptions& cmdOptions)
    {
        this->cmdOptions = cmdOptions;
        dbEngine = new DB_ENGINE;
    }
    ~RXProcessor()
    {
        delete dbEngine;
    }
    //
    BOOL processDAT(void);
    BOOL processSTA(void);
private:
    DB_ENGINE *dbEngine;
    CommandOptions cmdOptions;
    //
    enum { LSTTARGET, PRNTARGET };
    BOOL dat2PlainFile(WORD extTarget);
    BOOL sta2PlainFile(WORD extTarget);
    //
    void listBooth(ostream& lstStream, WORD extTarget, DWORD lowerNumber, DWORD numReceipts, WORD boothNumber);
	void receipt2Line(Receipt const & receipt, ostream& lstStream, const char* separator = "");
	void receipt2LineCommon(Receipt const & receipt, ostream& lstStream, const char* separator);
    void setDatPrnHeader(ostream& lstStream, const char *separator);
    void setStaPrnHeader(ostream& lstStream, const char *separator);
	void receipt2LineTotals(Receipt const & receipt, ostream& lstStream, const char* separator);
	void receipt2LineTel(Receipt const & receipt, ostream& lstStream, const char* separator);
	void receipt2LineTelex(Receipt const & receipt, ostream& lstStream, const char* separator);
	void receipt2LineFax(Receipt const & receipt, ostream& lstStream, const char* separator);
	void receipt2LineCard(Receipt const & receipt, ostream& lstStream, const char* separator);
	void receipt2LineOther(Receipt const & receipt, ostream& lstStream, const char* separator);
    //
    void listEntry(ostream& lstStream, WORD extTarget, WORD type, const char* separator);
    //
    void newLine(ostream& lstStream, WORD indent);
};

#endif // __RXPROCES_H
