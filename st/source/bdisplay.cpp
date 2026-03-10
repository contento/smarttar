//
// [ BDISPLAY.CPP ]
//

#include "stdst.h"

#include <bdisplay.h>

BoothDisplay::BoothDisplay()
{
	installed = FALSE;
    serial    = NULL;
}

BoothDisplay::~BoothDisplay()
{
    if (serial)
    {
        delete serial;
    }
}

BOOL BoothDisplay::install(const char *message, WORD com, WORD bauds)
{
    if (serial)
    { // if exist a serial then re-install
        delete serial;
        serial = NULL;
    }
    serial = new SERIAL(com, bauds);
	installed = serial->IsInstalled();
	if (installed)
    {
        setDefaultMessage(message);
	}
    else
    {
        delete serial;
        serial = NULL;
    }
	return installed;
}

void BoothDisplay::setDefaultMessage(const char *message)
{
	if (installed)
    {
        // 0x21 or 33 set message broadcast
        char b = 0x21; // convert to byte to avoid stack surprises
        serial->Put(b);
        serial->Put(SET_DEFAULT_MESSAGE_CMD);
        serial->Put(message);
        serial->Put(SEPARATOR_CMD);
        serial->Put(END_CMD);
	}
}

void BoothDisplay::showOnHook(Info info)
{
	if (installed)
    {
        char b = info.displayNum; // convert to byte to avoid stack surprises
        serial->Put(b);
        serial->Put(ON_HOOK_CMD);
        serial->Put(END_CMD);
    }
}

void BoothDisplay::showOffHook(Info info)
{
    if (installed)
    {
        char b = info.displayNum; // convert to byte to avoid stack surprises
        serial->Put(b);
		serial->Put(OFF_HOOK_CMD);
        serial->Put(END_CMD);
    }
}

void BoothDisplay::showDialing(Info info)
{
    if (installed)
    {
        char b = info.displayNum; // convert to byte to avoid stack surprises
        serial->Put(b);
        serial->Put(DIALING_CMD);
        serial->Put(info.phone);
        serial->Put(SEPARATOR_CMD);
        serial->Put(info.cityName);
        serial->Put(SEPARATOR_CMD);
        serial->Put(END_CMD);
    }
}

void BoothDisplay::showComm(Info info)
{
    if (installed)
    {
		char b = info.displayNum; // convert to byte to avoid stack surprises
		serial->Put(b);
		serial->Put(COMM_CMD);
		STR16 s;
		sprintf(s, "%0.1f", info.elapsedTime);
		serial->Put(s);
		serial->Put(SEPARATOR_CMD);
		sprintf(s, "%0.2f", info.cost);
		serial->Put(s);
		serial->Put(SEPARATOR_CMD);
		sprintf(s, "%d", info.numOfCalls);
		serial->Put(s);
		serial->Put(SEPARATOR_CMD);
		sprintf(s, "%0.2f", info.totalCost);
		serial->Put(s);
		serial->Put(SEPARATOR_CMD);
		serial->Put(END_CMD);
    }
}

void BoothDisplay::showLastRefresh(Info info)
{
    if (installed)
    {
		char b = info.displayNum; // convert to byte to avoid stack surprises
		serial->Put(b);
		serial->Put(LAST_REFRESH_CMD);
		STR16 s;
		sprintf(s, "%0.1f", info.elapsedTime);
		serial->Put(s);
		serial->Put(SEPARATOR_CMD);
		sprintf(s, "%0.2f", info.cost);
		serial->Put(s);
		serial->Put(SEPARATOR_CMD);
		sprintf(s, "%d", info.numOfCalls);
		serial->Put(s);
		serial->Put(SEPARATOR_CMD);
		sprintf(s, "%0.2f", info.totalCost);
		serial->Put(s);
		serial->Put(SEPARATOR_CMD);
		serial->Put(END_CMD);
    }
}

void BoothDisplay::showTotalCash(Info info)
{
    if (installed)
    {
        char b = info.displayNum; // convert to byte to avoid stack surprises
		serial->Put(b);
        serial->Put(TOTAL_CASH_CMD);
        STR16 s;
        sprintf(s, "%d", info.boothNum);
        serial->Put(s);
        serial->Put(SEPARATOR_CMD);
        sprintf(s, "%d", info.numOfCalls);
        serial->Put(s);
        serial->Put(SEPARATOR_CMD);
        sprintf(s, "%0.2f", info.totalCost);
        serial->Put(s);
		serial->Put(SEPARATOR_CMD);
		serial->Put(END_CMD);
	}
}

void BoothDisplay::showLocked(Info info)
{
	if (installed)
	{
		char b = info.displayNum; // convert to byte to avoid stack surprises
		serial->Put(b);
		serial->Put(LOCKED_CMD);
		serial->Put(END_CMD);
	}
}

void BoothDisplay::showDialErr(Info info)
{
	if (installed)
	{
		char b = info.displayNum; // convert to byte to avoid stack surprises
		serial->Put(b);
		serial->Put(DIAL_ERR_CMD);
		serial->Put(END_CMD);
	}
}

void BoothDisplay::showCommErr(Info info)
{
	if (installed)
	{
		char b = info.displayNum; // convert to byte to avoid stack surprises
		serial->Put(b);
		serial->Put(COMM_ERR_CMD);
		serial->Put(END_CMD);
	}
}

void BoothDisplay::showSpy(Info info)
{
	if (installed)
	{
		char b = info.displayNum; // convert to byte to avoid stack surprises
		serial->Put(b);
		serial->Put(SPY_CMD);
		serial->Put(END_CMD);
	}
}
