//
// [ SPOOLER.CPP ]
//

#include "stdst.h"

#include <stdarg.h>
#include <bios.h>
#include <events.h>
#include <spooler.h>

extern CFG 	*g_cfg;

//
// --- SPOOLER ---------------------------------------------------------------
//
#define BIOS_PRINT_WRITE     0x00
#define BIOS_PRINT_STATUS    0x02
#define BIOS_PRINT_BUSY      0x80
#define BIOS_PRINT_ERROR     0x08
#define BIOS_PRINT_PAPER_OUT 0x20

SPOOLER::SPOOLER(BYTE numOfChannels)
	: UI_DEVICE(E_SPOOLER, D_ON),
	Serial(NULL)
{
	NumOfChannels = (numOfChannels > MAX_SPOOL_CHANNELS) ? MAX_SPOOL_CHANNELS : numOfChannels;

	for (int i = 0; i < NumOfChannels; i++)
		Buffers[i] = new CIRCULAR_QUEUE<char>(PRINTER_BUFFER_SIZE);

	PrintfBuffer = new char[0x400];

	InstallSerial();
}

SPOOLER::~SPOOLER()
{
    Terminate(); // change Flush for Terminate !!!. GCC/gcc
    UninstallSerial();
	for (int i = 0; i < NumOfChannels; i++)
		delete Buffers[i];

	delete [] PrintfBuffer;
}

BOOL SPOOLER::Print(BYTE channel, const char *s, BOOL with0xFF)
{
//	SpoolerQueueMutex mutex;

	if (channel >= NumOfChannels)
		return FALSE;

	int numOfChars = strlen(s);
	int i = 0;
	//
	// this function is useful to Print strings terminated in 0xFF
	// not NULL (0x00), because some controls require 0x00 as a block terminator
	//
	if (with0xFF)
	{
		while (s[i] != '\xFF')
		{
			if (i == PRINTER_BUFFER_SIZE)
			{
				return FALSE; // avoid to overflow buffer
			}
			i++;
		}
		numOfChars = i;
	}
	if (!Buffers[channel]->HasSpaceFor(numOfChars))
	{
		return FALSE;
	}

    // Buffers = Data
    for (i= 0; i<numOfChars; i++)
		Buffers[channel]->Put(s[i]);

	return TRUE;
}

BOOL SPOOLER::Print(BYTE channel, char byte)
{
//	SpoolerQueueMutex mutex;
	if (channel >= NumOfChannels)
		return FALSE;
	int retValue = Buffers[channel]->Put(byte); // Buffers = Data
	return retValue;
}

void SPOOLER::Flush(void)
{
//	SpoolerQueueMutex mutex;

	char byte;
	// no check because the program is quitting
	for (int i = 0; i < NumOfChannels; i++)
		while (Buffers[i]->Get(byte))
			PrintChar(i, byte); // !!! check is missed for printer
}

void SPOOLER::Terminate(void)
{
//	SpoolerQueueMutex mutex;

	char byte;
	for (int i = 0; i < NumOfChannels; i++)
		while (Buffers[i]->Get(byte))
			continue;  // nothing to do ...
}

//
// this a Printf useful for printer.
// In a near future we change this one for a "<<" operator with streams. GCC/gcc
//
BOOL SPOOLER::Printf(BYTE channel, const char *fmt, ...)
{
    va_list  arg;
    int retValue;
    va_start(arg, fmt);
    {
        vsprintf(PrintfBuffer, fmt, arg);
        retValue = Print(channel, PrintfBuffer);
    }
    va_end(arg);
    return(retValue);
}

EVENT_TYPE SPOOLER::Event(const UI_EVENT &event)
{
    // Switch on the event type.
    EVENT_TYPE ccode = event.type;
    switch (ccode)
    {
    case E_DEVICE:
    case E_SPOOLER:
        switch (event.rawCode)
        {
        case D_OFF:
        case D_ON:
            state = event.rawCode;
            break;
		}
        ccode = state;
        break;
    }
    return (ccode);
}

void SPOOLER::Poll(void)
{
    // Check to see if the device is on.
    if (state != D_ON)
        return;
	//
    //	if (lastHundreths != hundreth) {
    //		lastTime.Import();
    for (int channel = 0; channel < NumOfChannels; channel++)
    {
        // for each channel try to print
        for (int i = 0; i < PRINTER_FLUSH_SIZE; i++)
		{
			//SpoolerQueueMutex mutex;
			{
				char byte;
				if (Buffers[channel]->Get2(byte))
				{
					// send to the printer and check device
					int i = 0;
					BOOL printed = FALSE;
					while (i < 0x05 && !printed)
					{ // try
						printed = PrintChar(channel, byte);
						i++;
					}
					if (printed)
					{
                        Buffers[channel]->Get(byte); // ... now extract
					}
					else
					{
						break; // out, don't wait
					}
				}
				else
				{
					break; // out, don't wait
				}


			}
		}
	}
}

BOOL SPOOLER::PrintChar(BYTE channel, char byte)
{
	byte = _ISO2ASCII(byte);
	if (!strcmp(g_cfg->P_PORT, "lpt"))
	{
		// via PRN
		BYTE port = (!g_cfg->DOUBLE_PRN)?g_cfg->LPT-1:channel;
		WORD status = biosprint(BIOS_PRINT_STATUS, 0, port);
		if ((status&BIOS_PRINT_BUSY) && !(status&BIOS_PRINT_ERROR) && !(status&BIOS_PRINT_PAPER_OUT))
		{
            biosprint(BIOS_PRINT_WRITE, byte, port);
            return TRUE;
        }
    }
    else
    {
		// via COM
        if (Serial)
        {
            WORD msr = Serial->GetStatus() >> 8;
            if ((msr & SERIAL::CTS) && Serial->Put(byte))
                return TRUE;
        }
    }
    return FALSE;
}

BOOL SPOOLER::HasSpace(BYTE channel)
{
    if (channel < NumOfChannels)
        return Buffers[channel]->HasSpaceFor(PRINTER_MIN_SPACE);
    else
        return FALSE;
}

BOOL SPOOLER::InstallSerial(void)
{
	delete Serial;

	if (!strcmp(g_cfg->P_PORT, "lpt"))
		return TRUE; // everything is ok but nothing to do

	// build parameters based on Config
	int port, speed, parity, bits, stopBits;
	extern char *_COM_FMT;
	STR16 parityStr;
	sscanf(g_cfg->COM, _COM_FMT, &port, &speed, &bits, parityStr, &stopBits);
	port = (port == 2)?SERIAL::COM2:SERIAL::COM1;
	switch (speed)
	{
	case 1200 :
		speed = SERIAL::S1200;
		break;
	case 2400 :
		speed = SERIAL::S2400;
        break;
    case 4800 :
        speed = SERIAL::S4800;
        break;
	case 9600 :
        speed = SERIAL::S9600;
        break;
    case 19200:
        speed = SERIAL::S19200;
        break;
    default   :
        speed = SERIAL::S2400;
    }
    if (!strcmp(parityStr, "even"))
        parity = SERIAL::EVEN;
    else if (!strcmp(parityStr, "odd"))
        parity = SERIAL::ODD;
    else
        parity = SERIAL::NONE;
    bits = (bits == 7)?SERIAL::S7:SERIAL::S8;
    stopBits = (stopBits == 1)?SERIAL::S1:SERIAL::S2;
    //
    Serial = new SERIAL(port, speed, parity, bits, stopBits, 0x100);
    if (!Serial->IsInstalled())
    {
        delete Serial;
        Serial = NULL;
        return FALSE;
    }
    return TRUE;
}

BOOL SPOOLER::UninstallSerial(void)
{
	delete Serial;
	Serial = NULL;

	return TRUE;
}
