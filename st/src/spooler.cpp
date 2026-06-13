//
// [ SPOOLER.CPP ]
//

#include "stdst.h"

#include <stdarg.h>
#include <bios.h>
#include <events.h>
#include <spooler.h>
#include <pdf_wr.h>

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
	Serial(NULL),
	pdfWriter(NULL)
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

	if (pdfWriter)
		pdf_wr_close(pdfWriter);
	pdfWriter = NULL;
}
BOOL SPOOLER::Print(BYTE channel, const char *s, BOOL with0xFF)
{
//	SpoolerQueueMutex mutex;

	// PDF output: intercept before buffering
	if (!strcmp(g_cfg->P_PORT, "pdf"))
	{
		pdfWriteString(s, with0xFF);
		return TRUE;
	}

	if (channel >= NumOfChannels)
		return FALSE;

	int numOfChars;
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
	else
		numOfChars = strlen(s); // safe only when NUL-terminated (not 0xFF case)
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

// --- PDF output ------------------------------------------------------------
// Strip printer escape sequences and write plain text to the PDF writer.
// Handles: 0x1B + param sequences, \t (tab stops), \n (line break),
// 0xFF (block terminator).  All other bytes are text characters.
//
void SPOOLER::pdfWriteString(const char *s, BOOL with0xFF)
{
	if (!pdfWriter)
	{
		// Build filename PDF\RXYYMMDD.pdf -- 8.3-safe (RX + YYMMDD = 8 chars).
		// Use the numeric date overload directly: the char* _GetSysDate
		// returns DD/MM/YYYY, not the YYYY-MM-DD this once assumed.
		char path[80];
		WORD year, month, day;
		_GetSysDate(year, month, day);
		mkdir("PDF");  // ensure output dir exists; harmless if already there
		sprintf(path, "PDF\\RX%02u%02u%02u.pdf",
			(unsigned)(year % 100), (unsigned)month, (unsigned)day);
		pdfWriter = pdf_wr_open(path);
		if (!pdfWriter)
			return;
	}
	//
	// Parse the string: strip escape sequences, interpret \t and \n
	//
	int end = (int)strlen(s);
	if (with0xFF)
	{
		// Find the 0xFF terminator
		int k = 0;
		while (s[k] != '\xFF' && k < PRINTER_BUFFER_SIZE)
			k++;
		end = k;
	}
	//
	int i = 0;
	char lineBuf[256];
	int lineLen = 0;
	//
	while (i < end)
	{
		unsigned char c = (unsigned char)s[i];
		//
		if (c == 0x1B)
		{
			// ESC/P command: skip ESC + command byte + its fixed param
			// bytes.  Param counts match the codes the printer DLLs emit
			// (see src/pr/pr_*.c).  Unknown commands take no params.
			i++; // skip ESC
			if (i >= end) break;
			unsigned char cmd = (unsigned char)s[i];
			i++; // skip command byte
			int params;
			switch (cmd)
			{
			case 0x21: // ESC !  master print mode
			case 0x41: // ESC A  line spacing n/72
			case 0x43: // ESC C  page length
			case 0x7A: // ESC z  (printer-specific)
				params = 1;
				break;
			case 0x63: // ESC c 0 n  (printer-specific, 2 bytes follow)
				params = 2;
				break;
			case 0x44: // ESC D  horizontal tab stops: list ended by NUL
				while (i < end && (unsigned char)s[i] != 0x00)
					i++;
				if (i < end) i++; // skip the NUL terminator
				params = 0;
				break;
			default:   // ESC @, ESC 0, ESC P, ESC M, ESC i, ... no params
				params = 0;
				break;
			}
			while (params-- > 0 && i < end)
				i++;
			continue;
		}
		//
		if (c == '\t')
		{
			// Tab stop: advance to next multiple of 8 columns
			int nextTab = ((lineLen / 8) + 1) * 8;
			while (lineLen < nextTab && lineLen < (int)sizeof(lineBuf) - 1)
				lineBuf[lineLen++] = ' ';
			i++;
			continue;
		}
		//
		if (c == '\n' || c == '\r')
		{
			lineBuf[lineLen] = '\0';
			pdf_wr_line(pdfWriter, lineLen > 0 ? lineBuf : "");
			lineLen = 0;
			i++;
			continue;
		}
		//
		if (c == 0x0C)
		{
			// Form feed: page break
			if (lineLen > 0)
			{
				lineBuf[lineLen] = '\0';
				pdf_wr_line(pdfWriter, lineBuf);
				lineLen = 0;
			}
			pdf_wr_page_break(pdfWriter);
			i++;
			continue;
		}
		//
		if (c == 0xFF)
			break; // block terminator
		//
		if (c < 0x20)
		{
			// Strip stray printer control bytes (SO, SI, DC4, CAN, ...)
			// that aren't the tab/newline/form-feed handled above.
			i++;
			continue;
		}
		//
		// Normal text character
		if (lineLen < (int)sizeof(lineBuf) - 1)
			lineBuf[lineLen++] = (char)c;
		i++;
	}
	//
	// Flush any remaining partial line
	if (lineLen > 0)
	{
		lineBuf[lineLen] = '\0';
		pdf_wr_line(pdfWriter, lineBuf);
	}
}
