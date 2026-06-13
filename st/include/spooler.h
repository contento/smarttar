#ifndef __SPOOLER_H
#define __SPOOLER_H

#if !defined(UI_ENV_HPP)
#include <ui_env.hpp>
#endif

#if !defined(UI_EVT_HPP)
#include <ui_evt.hpp>
#endif

#if !defined(__ST_DEFS_H)
#include <st_defs.h>
#endif

#if !defined(__SERIAL_H)
#include <serial.h>
#endif

#if !defined(__CQUEUE_H)
#include <cqueue.h>
#endif

#if !defined(__PDF_WR_H)
#include <pdf_wr.h>
#endif

const UINT PRINTER_BUFFER_SIZE  = 0x1000;
const UINT PRINTER_FLUSH_SIZE   = 0x0100;
const UINT PRINTER_MIN_SPACE    = 0x0200;
const BYTE MAX_SPOOL_CHANNELS   = 0x04;

class EXPORT SPOOLER : public UI_DEVICE
{
    friend CONTROLLER;
public:
    SPOOLER(BYTE numOfChannels = 1);
    ~SPOOLER();
    //
    BOOL InstallSerial(void);
    BOOL UninstallSerial(void);
    BOOL Printf(BYTE channel, const char *fmt, ...);
    inline BOOL Print(BYTE channel, char byte);
    BOOL Print(BYTE channel, const char *string, BOOL with0xFF = FALSE);
    void Flush(void);
    void Terminate(void);
    BOOL HasSpace(BYTE channel);

private:

	CIRCULAR_QUEUE<char> *Buffers[MAX_SPOOL_CHANNELS];
    char *PrintfBuffer; // useful for Printf
    SERIAL *Serial;
    BYTE   NumOfChannels;
    PDF_WR *pdfWriter; // PDF output (when P_PORT == "pdf")
    //
    EVENT_TYPE Event(const UI_EVENT &event);
    void       Poll(void);
    void       pdfWriteString(const char *s, BOOL with0xFF); // strip escapes, write to PDF
    //
    inline BOOL PrintChar(BYTE channel, char byte);
};

#endif // __SPOOLER_H
