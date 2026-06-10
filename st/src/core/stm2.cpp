//
// [ STM2.CPP ]
//
// STM2 abstract-base implementation: the generic, hardware-independent
// record logic (login/logout, get/put, the prepare() field mapping, the
// receipt circular queue, dump/replace).  Built entirely on the virtual
// read/write seam, so it is shared by both backends (BankStm2/NullStm2).
// The hardware-specific check/read/write live in the concrete backends
// (real_dos/bankstm2.cpp, demo_dos/nullstm2.cpp).
//

#include "stdst.h"

#include <stm2.h>

#if !defined(__NOAPPINFO__)
#include <info.h>
extern APP_INFO g_appInfo;
#endif

extern CFG	*g_cfg;

static const char *YES = "YES";
static const char *NO  = "NO";

WORD STM2::login(void)
{
    if (status != NONE)
    {
        if (status == OK || status == GARBAGE)
            emptyReceipts();
        // set initial data
        setExit(NO);
        setSerial();
        int date, time;
        date = _GetSysDate();
        put(LOGINDATE, &date);
        time = _GetSysTime();
        put(LOGINTIME, &time);
    }
    //
    return status;
}

void STM2::logout(void)
{
#ifdef DOSX286
    extern BOOL _exceptionIsOn;
    if (!_exceptionIsOn)
    {
#endif
        setExit(YES);
        emptyReceipts();
#ifdef DOSX286
    }
    int date, time;
    date = _GetSysDate();
    put(LOGOUTDATE, &date);
    time = _GetSysTime();
    put(LOGOUTTIME, &time);
#endif
}

void STM2::forceOk(void)
{
    emptyReceipts();
    status = OK;
}

void STM2::setSerial(void)
{
    // since in version 1.5 we change the approach we have to crypt before
    // writing STM2.  GCC/gcc.
    SERIAL_NUMBER serialNumber;
    memset(serialNumber, 0, sizeof(SERIAL_NUMBER));
#if !defined(__NOAPPINFO__)
    memcpy(serialNumber, g_appInfo.Serial, sizeof(serialNumber)); // full
#endif
    _Encrypt(serialNumber, sizeof(serialNumber));
    put(SERIAL, serialNumber);
}

void STM2::setExit(const char *str)
{
    char exitString[STM2_EXITSTRINGSIZE];
    strcpy(exitString, str);
    put(EXITSTRING, exitString);
}

#pragma warn -def
BOOL STM2::get(WORD id, void *buffer)
{
    WORD offset;
    WORD bufSize;
    BOOL ret=FALSE;
    //
    if (id == RECEIPTS)
    {
		get(RECEIPTSFRONT, &receiptsFront);
        get(RECEIPTSREAR, &receiptsRear);
        get(RECEIPTSCOUNT, &receiptsCount);
        //
        if (receiptsFront != receiptsRear)
        {
            if (prepare(id, offset, bufSize, INPUT))
            {
                ret = read(offset, buffer, bufSize)==bufSize;
                if (ret)
                {
                    receiptsFront = (receiptsFront+1)%STM2_MAXRECEIPTS;
                    receiptsCount--;
                    //
                    put(RECEIPTSFRONT, &receiptsFront);
                    put(RECEIPTSREAR, &receiptsRear);
                    put(RECEIPTSCOUNT, &receiptsCount);
                }
            }
        }
    }
    else
    {
        if (prepare(id, offset, bufSize, INPUT))
            ret = read(offset, buffer, bufSize)==bufSize;
    }
    return ret;
}

BOOL STM2::put(WORD id, const void *buffer)
{
    WORD offset;
    WORD bufSize;
    BOOL ret=FALSE;
    //
    if (id == RECEIPTS)
    {
        get(RECEIPTSFRONT, &receiptsFront);
        get(RECEIPTSREAR, &receiptsRear);
        get(RECEIPTSCOUNT, &receiptsCount);
        //
        if ((receiptsRear+1)%STM2_MAXRECEIPTS != receiptsFront)
        { // wrap around
            if (prepare(id, offset, bufSize, OUTPUT))
            {
                ret = write(offset, buffer, bufSize)==bufSize;
                if (ret)
                {
                    receiptsRear = (receiptsRear+1)%STM2_MAXRECEIPTS;
                    receiptsCount++;
                    //
                    put(RECEIPTSFRONT, &receiptsFront);
                    put(RECEIPTSREAR, &receiptsRear);
                    put(RECEIPTSCOUNT, &receiptsCount);
                }
            }
        }
    }
    else
    {
        if (prepare(id, offset, bufSize, OUTPUT))
            return (write(offset, buffer, bufSize)==bufSize);
    }
    return FALSE;
}

#pragma argsused
BOOL STM2::prepare(WORD id, WORD& offset, WORD& bufSize, WORD direction)
{
	Data *data;

#define PREPARE(object)                           	\
	offset  = ((int)&(data->object) - (int)(data)); \
	bufSize = (sizeof(data->object));

    switch (id)
    {
    case EXITSTRING                 :
        PREPARE(exitString);
        break;
    case SERIAL                     :
        PREPARE(sysInfo.serial);
        break;
    case DATE                       :
        PREPARE(sysInfo.date);
        break;
    case TIME                       :
        PREPARE(sysInfo.time);
        break;
    case LOGINDATE                  :
        PREPARE(sysInfo.loginDate);
        break;
    case LOGINTIME                  :
        PREPARE(sysInfo.loginTime);
        break;
    case LOGOUTDATE                 :
        PREPARE(sysInfo.logoutDate);
        break;
    case LOGOUTTIME                 :
        PREPARE(sysInfo.logoutTime);
        break;
    case RECEIPTNUMBER              :
        PREPARE(sysInfo.receiptNumber);
        break;
    case EXTENSIONRECEIPTNUMBER     :
        PREPARE(sysInfo.extensionReceiptNumber);
        break;
    case TRIES                      :
        PREPARE(sysInfo.tries);
        break;
    case STATISTICSENTRIES          :
        PREPARE(statistics.entries);
        break;
    case STATISTICSDOUBLEPRNENTRIES :
        PREPARE(statistics.doublePRNEntries);
        break;
    case STATISTICSCELLULARENTRIES  :
        PREPARE(statistics.cellularEntries);
        break;
    case BOOTHCLUSTERS              :
        PREPARE(boothClusters);
		bufSize = g_cfg->ACTIVE_CLUSTERS * sizeof(BoothCluster);
        break;
    case EXTENSIONCRITICALSTATISTICS:
        PREPARE(extensionStatistics.critical);
        break;
    case RECEIPTSCOUNT              :
        PREPARE(receipts.count);
        break;
    case RECEIPTSFRONT              :
        PREPARE(receipts.front);
        break;
    case RECEIPTSREAR               :
        PREPARE(receipts.rear);
        break;
    case RECEIPTS                   :
        PREPARE(receipts.data[(direction==INPUT)?receiptsFront:receiptsRear]);
        break;
        //
    default:
        return FALSE;
    }
    return TRUE;
}

void STM2::dump(WORD offset, void *buffer, WORD bufSize)
{
    read(offset, buffer, bufSize);
}

void STM2::replace(WORD offset, void *buffer, WORD bufSize)
{
	write(offset, buffer, bufSize);
}

#pragma warn +def

void STM2::emptyReceipts(void)
{
    receiptsFront = 0;
    put(RECEIPTSFRONT, &receiptsFront);
    receiptsRear  = 0;
    put(RECEIPTSREAR, &receiptsRear);
    receiptsCount = 0;
    put(RECEIPTSCOUNT, &receiptsCount);
}
