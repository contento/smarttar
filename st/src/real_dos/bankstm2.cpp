//
// [ BANKSTM2.CPP ]
//
// BankStm2: the production STM2 backend.  Battery-backed NVRAM accessed
// as 1-2 banks of STM2_BANKSIZE bytes through port I/O.  Implements the
// hardware seam (check / read / write) declared on the STM2 base; all the
// generic record logic lives in core/stm2.cpp.  real_dos -- not linked in
// the demo_dos build (see MINI_SMARTTAR_PLAN).
//

#include "stdst.h"

#include <bankstm2.h>

static const char *YES = "YES";
static const char *NO  = "NO";
//
// ports
//
static const WORD ADDR_PORT         = 0x028C;
static const WORD FIRST_BANK_PORT   = 0x028B;
static const WORD SECOND_BANK_PORT  = 0x028E;

BankStm2::BankStm2(void)
{
    //
    // Warning !!!
    // be careful: the initialization of the Number of Receipts is
    // a responsability of the STM2 client !!!
    // check() is virtual -- call it from the derived ctor so this
    // backend's override is the one that runs.
    check();
}

BankStm2::~BankStm2(void)
{
}

void BankStm2::check(void)
{
    //
    // test bank existence
    //
    const TESTSTRSIZE = 10;
    char savedStr[TESTSTRSIZE];
    char testStr[TESTSTRSIZE];
    char recoveredStr[TESTSTRSIZE];
    strcpy(testStr, "__AM__");
    banks = 0;
    // bank 1
    banks = 1; // assume for read !!!
    read(0, savedStr, sizeof(savedStr));   // save
    write(0, testStr, sizeof(testStr));  // write
    read(0, recoveredStr, sizeof(recoveredStr)); // recover
    write(0, savedStr, sizeof(savedStr));  // restore
    if (strcmp(testStr, recoveredStr))
		banks = 0;

	if (banks == 1)
	{
		// bank 1 has to be good
		banks = 2; // assume for read !!!
		read(STM2_BANKSIZE, savedStr, sizeof(savedStr));   // save
		write(STM2_BANKSIZE, testStr, sizeof(testStr));  // write
		read(STM2_BANKSIZE, recoveredStr, sizeof(recoveredStr)); // recover
		write(STM2_BANKSIZE, savedStr, sizeof(savedStr));  // restore
		if (strcmp(testStr, recoveredStr))
			banks = 1;
	}
	status = NONE;
	if (banks > 0)
	{
		WORD tries = 0;
		get(TRIES, &tries);

		// test abnormal exit
		char exitString[STM2_EXITSTRINGSIZE];
		get(EXITSTRING, exitString);

		// BEGIN 2.30 build 19
		if (!strcmp(exitString, YES))
		{
			tries = 0;
			status = OK;
		}
		else if (!strcmp(exitString, NO))
		{
			status = BAD_SHUTDOWN;
			tries++;
			if (tries == 2)
			{
				status = GARBAGE;
				tries = 0;
			}
		}
		else
		{
			tries = 0;
			status = GARBAGE;
		}

		put(TRIES, &tries);
		// END 2.30 build 19
	}
}
//
// to test the contents of STM2
//
WORD BankStm2::fill(char c)
{
    WORD i;
    if (banks > 0)
    {
        for (i = 0; i < STM2_BANKSIZE; i++)
            out(ADDR_PORT, i, FIRST_BANK_PORT, c);
    }
    if (banks > 1)
    {
        for (i = 0; i < STM2_BANKSIZE; i++)
            out(ADDR_PORT, i, SECOND_BANK_PORT, c);
    }
    return banks;
}
//
// --- low level ------------------------------------------------------------
//

WORD BankStm2::write(WORD offset, const void *buffer, WORD bufSize)
{
    int firstLowerBound = -1, firstUpperBound = -1;
    int secondLowerBound =  -1, secondUpperBound = -1;
    if ((offset < STM2_BANKSIZE))
    {
        firstLowerBound = offset;
        if (offset+bufSize < STM2_BANKSIZE)
        {
            firstUpperBound	= offset+bufSize;
        }
        else
        {
            secondLowerBound = 0;
            secondUpperBound = offset-STM2_BANKSIZE;
        }
    }
    else
    {
        secondLowerBound = offset - STM2_BANKSIZE;
        secondUpperBound = secondLowerBound + bufSize;
    }
    int i;
    WORD ret = 0;
    if (banks > 0)
        for (i = firstLowerBound; i < firstUpperBound; i++, ret++)
            out(ADDR_PORT, i, FIRST_BANK_PORT, ((char *)(buffer))[ret]);
    if (banks > 1)
        for (i = secondLowerBound; i < secondUpperBound; i++, ret++)
            out(ADDR_PORT, i, SECOND_BANK_PORT, ((char *)(buffer))[ret]);
    return ret;
}

WORD BankStm2::read(WORD offset, void *buffer, WORD bufSize)
{
    int firstLowerBound = -1, firstUpperBound = -1;
    int secondLowerBound =  -1, secondUpperBound = -1;
    if ((offset < STM2_BANKSIZE))
    {
        firstLowerBound = offset;
        if (offset+bufSize < STM2_BANKSIZE)
        {
            firstUpperBound	= offset+bufSize;
        }
        else
        {
            secondLowerBound = 0;
            secondUpperBound = offset-STM2_BANKSIZE;
        }
    }
    else
    {
        secondLowerBound = offset - STM2_BANKSIZE;
        secondUpperBound = secondLowerBound + bufSize;
    }
    int i;
    WORD ret = 0;
    if (banks > 0) // optimize if not into for
        for (i = firstLowerBound; i < firstUpperBound; i++, ret++)
            ((char *)(buffer))[ret] = in(ADDR_PORT, i, FIRST_BANK_PORT);
    if (banks > 1)
        for (i = secondLowerBound; i < secondUpperBound; i++, ret++)
            ((char *)(buffer))[ret] = in(ADDR_PORT, i, SECOND_BANK_PORT);
    return ret;
}

BYTE BankStm2::in(WORD addressPort, WORD offset, WORD dataPort)
{
    outport(addressPort, offset);
    return inportb(dataPort);
}

void BankStm2::out(WORD addressPort, WORD offset, WORD dataPort, const unsigned char byte)
{
    outport(addressPort, offset);
    outportb(dataPort, byte);
}
